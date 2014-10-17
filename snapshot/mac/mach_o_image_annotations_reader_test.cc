// Copyright 2014 The Crashpad Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "snapshot/mac/mach_o_image_annotations_reader.h"

#include <dlfcn.h>
#include <mach/mach.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <map>
#include <string>
#include <vector>

#include "base/basictypes.h"
#include "client/crashpad_info.h"
#include "client/simple_string_dictionary.h"
#include "gtest/gtest.h"
#include "snapshot/mac/process_reader.h"
#include "util/file/fd_io.h"
#include "util/mac/mac_util.h"
#include "util/mach/exc_server_variants.h"
#include "util/mach/exception_ports.h"
#include "util/mach/mach_message_server.h"
#include "util/test/errors.h"
#include "util/test/mac/mach_errors.h"
#include "util/test/mac/mach_multiprocess.h"

namespace crashpad {
namespace test {
namespace {

class TestMachOImageAnnotationsReader final : public MachMultiprocess,
                                              public UniversalMachExcServer {
 public:
  enum TestType {
    // Don’t crash, just test the CrashpadInfo interface.
    kDontCrash = 0,

    // The child process should crash by calling abort(). The parent verifies
    // that the system libraries set the expected annotations.
    kCrashAbort,

    // The child process should crash by setting DYLD_INSERT_LIBRARIES to
    // contain a nonexistent library. The parent verifies that dyld sets the
    // expected annotations.
    kCrashDyld,
  };

  explicit TestMachOImageAnnotationsReader(TestType test_type)
      : MachMultiprocess(),
        UniversalMachExcServer(),
        test_type_(test_type) {
  }

  ~TestMachOImageAnnotationsReader() {}

  // UniversalMachExcServer:
  kern_return_t CatchMachException(exception_behavior_t behavior,
                                   exception_handler_t exception_port,
                                   thread_t thread,
                                   task_t task,
                                   exception_type_t exception,
                                   const mach_exception_data_type_t* code,
                                   mach_msg_type_number_t code_count,
                                   thread_state_flavor_t* flavor,
                                   const natural_t* old_state,
                                   mach_msg_type_number_t old_state_count,
                                   thread_state_t new_state,
                                   mach_msg_type_number_t* new_state_count,
                                   bool* destroy_complex_request) override {
    *destroy_complex_request = true;

    EXPECT_EQ(ChildTask(), task);

    ProcessReader process_reader;
    bool rv = process_reader.Initialize(task);
    if (!rv) {
      ADD_FAILURE();
    } else {
      const std::vector<ProcessReader::Module>& modules =
          process_reader.Modules();
      std::vector<std::string> all_annotations_vector;
      for (const ProcessReader::Module& module : modules) {
        MachOImageAnnotationsReader module_annotations_reader(
            &process_reader, module.reader, module.name);
        std::vector<std::string> module_annotations_vector =
            module_annotations_reader.Vector();
        all_annotations_vector.insert(all_annotations_vector.end(),
                                      module_annotations_vector.begin(),
                                      module_annotations_vector.end());
      }

      // Mac OS X 10.6 doesn’t have support for CrashReporter annotations
      // (CrashReporterClient.h), so don’t look for any special annotations in
      // that version.
      int mac_os_x_minor_version = MacOSXMinorVersion();
      if (mac_os_x_minor_version > 7) {
        EXPECT_GE(all_annotations_vector.size(), 1u);

        const char* expected_annotation = nullptr;
        switch (test_type_) {
          case kCrashAbort:
            // The child process calls abort(), so the expected annotation
            // reflects this, with a string set by 10.7.5
            // Libc-763.13/stdlib/abort-fbsd.c abort(). This string is still
            // present in 10.9.5 Libc-997.90.3/stdlib/FreeBSD/abort.c abort(),
            // but because abort() tests to see if a message is already set and
            // something else in Libc will have set a message, this string is
            // not the expectation on 10.9 or higher. Instead, after fork(), the
            // child process has a message indicating that a fork() without
            // exec() occurred. See 10.9.5 Libc-997.90.3/sys/_libc_fork_child.c
            // _libc_fork_child().
            expected_annotation =
                mac_os_x_minor_version <= 8
                    ? "abort() called"
                    : "crashed on child side of fork pre-exec";
            break;

          case kCrashDyld:
            // This is independent of dyld’s error_string, which is tested
            // below.
            expected_annotation = "dyld: launch, loading dependent libraries";
            break;

          default:
            ADD_FAILURE();
            break;
        }

        size_t expected_annotation_length = strlen(expected_annotation);
        bool found = false;
        for (const std::string& annotation : all_annotations_vector) {
          // Look for the expectation as a leading susbtring, because the actual
          // string that dyld uses will have the contents of the
          // DYLD_INSERT_LIBRARIES environment variable appended to it on Mac
          // OS X 10.10.
          if (annotation.substr(0, expected_annotation_length) ==
                  expected_annotation) {
            found = true;
            break;
          }
        }
        EXPECT_TRUE(found);
      }

      // dyld exposes its error_string at least as far back as Mac OS X 10.4.
      if (test_type_ == kCrashDyld) {
        const char kExpectedAnnotation[] = "could not load inserted library";
        size_t expected_annotation_length = strlen(kExpectedAnnotation);
        bool found = false;
        for (const std::string& annotation : all_annotations_vector) {
          // Look for the expectation as a leading substring, because the actual
          // string will contain the library’s pathname and, on Mac OS X 10.9
          // and later, a reason.
          if (annotation.substr(0, expected_annotation_length) ==
                  kExpectedAnnotation) {
            found = true;
            break;
          }
        }

        EXPECT_TRUE(found);
      }
    }

    return ExcServerSuccessfulReturnValue(behavior, false);
  }

 private:
  // MachMultiprocess:

  void MachMultiprocessParent() override {
    ProcessReader process_reader;
    ASSERT_TRUE(process_reader.Initialize(ChildTask()));

    // Wait for the child process to indicate that it’s done setting up its
    // annotations via the CrashpadInfo interface.
    char c;
    CheckedReadFD(ReadPipeFD(), &c, sizeof(c));

    // Verify the “simple map” annotations set via the CrashpadInfo interface.
    const std::vector<ProcessReader::Module>& modules =
        process_reader.Modules();
    std::map<std::string, std::string> all_annotations_simple_map;
    for (const ProcessReader::Module& module : modules) {
      MachOImageAnnotationsReader module_annotations_reader(
          &process_reader, module.reader, module.name);
      std::map<std::string, std::string> module_annotations_simple_map =
          module_annotations_reader.SimpleMap();
      all_annotations_simple_map.insert(module_annotations_simple_map.begin(),
                                        module_annotations_simple_map.end());
    }

    EXPECT_GE(all_annotations_simple_map.size(), 5u);
    EXPECT_EQ("crash", all_annotations_simple_map["#TEST# pad"]);
    EXPECT_EQ("value", all_annotations_simple_map["#TEST# key"]);
    EXPECT_EQ("y", all_annotations_simple_map["#TEST# x"]);
    EXPECT_EQ("shorter", all_annotations_simple_map["#TEST# longer"]);
    EXPECT_EQ("", all_annotations_simple_map["#TEST# empty_value"]);

    // Tell the child process that it’s permitted to crash.
    CheckedWriteFD(WritePipeFD(), &c, sizeof(c));

    if (test_type_ != kDontCrash) {
      // Handle the child’s crash. Further validation will be done in
      // CatchMachException().
      mach_msg_return_t mr =
          MachMessageServer::Run(this,
                                 LocalPort(),
                                 MACH_MSG_OPTION_NONE,
                                 MachMessageServer::kOneShot,
                                 MachMessageServer::kBlocking,
                                 MACH_MSG_TIMEOUT_NONE);
      EXPECT_EQ(MACH_MSG_SUCCESS, mr)
          << MachErrorMessage(mr, "MachMessageServer::Run");

      switch (test_type_) {
        case kCrashAbort:
          SetExpectedChildTermination(kTerminationSignal, SIGABRT);
          break;

        case kCrashDyld:
          // dyld fatal errors result in the execution of an int3 instruction on
          // x86 and a trap instruction on ARM, both of which raise SIGTRAP.
          // 10.9.5 dyld-239.4/src/dyldStartup.s _dyld_fatal_error.
          SetExpectedChildTermination(kTerminationSignal, SIGTRAP);
          break;

        default:
          FAIL();
          break;
      }
    }
  }

  void MachMultiprocessChild() override {
    CrashpadInfo* crashpad_info = CrashpadInfo::GetCrashpadInfo();

    // This is “leaked” to crashpad_info.
    SimpleStringDictionary* simple_annotations = new SimpleStringDictionary();
    simple_annotations->SetKeyValue("#TEST# pad", "break");
    simple_annotations->SetKeyValue("#TEST# key", "value");
    simple_annotations->SetKeyValue("#TEST# pad", "crash");
    simple_annotations->SetKeyValue("#TEST# x", "y");
    simple_annotations->SetKeyValue("#TEST# longer", "shorter");
    simple_annotations->SetKeyValue("#TEST# empty_value", "");

    crashpad_info->set_simple_annotations(simple_annotations);

    // Tell the parent that the environment has been set up.
    char c = '\0';
    CheckedWriteFD(WritePipeFD(), &c, sizeof(c));

    // Wait for the parent to indicate that it’s safe to crash.
    CheckedReadFD(ReadPipeFD(), &c, sizeof(c));

    // Direct an exception message to the exception server running in the
    // parent.
    ExceptionPorts exception_ports(ExceptionPorts::kTargetTypeTask,
                                   mach_task_self());
    ASSERT_TRUE(exception_ports.SetExceptionPort(
        EXC_MASK_CRASH, RemotePort(), EXCEPTION_DEFAULT, THREAD_STATE_NONE));

    switch (test_type_) {
      case kDontCrash:
        break;

      case kCrashAbort:
        abort();
        break;

      case kCrashDyld: {
        // Set DYLD_INSERT_LIBRARIES to contain a library that does not exist.
        // Unable to load it, dyld will abort with a fatal error.
        ASSERT_EQ(
            0,
            setenv(
                "DYLD_INSERT_LIBRARIES", "/var/empty/NoDirectory/NoLibrary", 1))
            << ErrnoMessage("setenv");

        // The actual executable doesn’t matter very much, because dyld won’t
        // ever launch it. It just needs to be an executable that uses dyld as
        // its LC_LOAD_DYLINKER (all normal executables do). /usr/bin/true is on
        // every system, so use it.
        ASSERT_EQ(0, execl("/usr/bin/true", "true", nullptr))
            << ErrnoMessage("execl");
        break;
      }

      default:
        break;
    }
  }

  TestType test_type_;

  DISALLOW_COPY_AND_ASSIGN(TestMachOImageAnnotationsReader);
};

TEST(MachOImageAnnotationsReader, DontCrash) {
  TestMachOImageAnnotationsReader test_mach_o_image_annotations_reader(
      TestMachOImageAnnotationsReader::kDontCrash);
  test_mach_o_image_annotations_reader.Run();
}

TEST(MachOImageAnnotationsReader, CrashAbort) {
  TestMachOImageAnnotationsReader test_mach_o_image_annotations_reader(
      TestMachOImageAnnotationsReader::kCrashAbort);
  test_mach_o_image_annotations_reader.Run();
}

TEST(MachOImageAnnotationsReader, CrashDyld) {
  TestMachOImageAnnotationsReader test_mach_o_image_annotations_reader(
      TestMachOImageAnnotationsReader::kCrashDyld);
  test_mach_o_image_annotations_reader.Run();
}

}  // namespace
}  // namespace test
}  // namespace crashpad