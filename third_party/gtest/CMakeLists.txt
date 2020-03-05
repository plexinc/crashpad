add_library(gtest OBJECT)
crashpad_set_props(gtest)

target_include_directories(gtest PUBLIC gtest/googletest/include PRIVATE gtest/googletest)
target_compile_definitions(gtest PUBLIC GUNIT_NO_GOOGLE3=1)

if(NOT MSVC)
  target_compile_options(gtest PUBLIC -Wno-inconsistent-missing-override)
endif()

target_sources(gtest
  PRIVATE
  gtest/googletest/include/gtest/gtest-death-test.h
  gtest/googletest/include/gtest/gtest-matchers.h
  gtest/googletest/include/gtest/gtest-message.h
  gtest/googletest/include/gtest/gtest-param-test.h
  gtest/googletest/include/gtest/gtest-printers.h
  gtest/googletest/include/gtest/gtest-spi.h
  gtest/googletest/include/gtest/gtest-test-part.h
  gtest/googletest/include/gtest/gtest-typed-test.h
  gtest/googletest/include/gtest/gtest.h
  gtest/googletest/include/gtest/gtest_pred_impl.h
  gtest/googletest/include/gtest/gtest_prod.h
  gtest/googletest/include/gtest/internal/custom/gtest-port.h
  gtest/googletest/include/gtest/internal/custom/gtest-printers.h
  gtest/googletest/include/gtest/internal/custom/gtest.h
  gtest/googletest/include/gtest/internal/gtest-death-test-internal.h
  gtest/googletest/include/gtest/internal/gtest-filepath.h
  gtest/googletest/include/gtest/internal/gtest-internal.h
  gtest/googletest/include/gtest/internal/gtest-param-util.h
  gtest/googletest/include/gtest/internal/gtest-port-arch.h
  gtest/googletest/include/gtest/internal/gtest-port.h
  gtest/googletest/include/gtest/internal/gtest-string.h
  gtest/googletest/include/gtest/internal/gtest-type-util.h
  gtest/googletest/src/gtest-death-test.cc
  gtest/googletest/src/gtest-filepath.cc
  gtest/googletest/src/gtest-internal-inl.h
  gtest/googletest/src/gtest-matchers.cc
  gtest/googletest/src/gtest-port.cc
  gtest/googletest/src/gtest-printers.cc
  gtest/googletest/src/gtest-test-part.cc
  gtest/googletest/src/gtest-typed-test.cc
  gtest/googletest/src/gtest.cc
)

add_library(gmock OBJECT)
crashpad_set_props(gmock)
target_include_directories(gmock PUBLIC gtest/googlemock/include)
target_link_libraries(gmock PRIVATE gtest)

if(NOT MSVC)
  target_compile_options(gmock PUBLIC -Wno-inconsistent-missing-override)
endif()

target_sources(gmock
  PRIVATE
  gtest/googlemock/include/gmock/gmock-actions.h
  gtest/googlemock/include/gmock/gmock-cardinalities.h
  gtest/googlemock/include/gmock/gmock-function-mocker.h
  gtest/googlemock/include/gmock/gmock-generated-actions.h
  gtest/googlemock/include/gmock/gmock-generated-function-mockers.h
  gtest/googlemock/include/gmock/gmock-generated-matchers.h
  gtest/googlemock/include/gmock/gmock-matchers.h
  gtest/googlemock/include/gmock/gmock-more-actions.h
  gtest/googlemock/include/gmock/gmock-more-matchers.h
  gtest/googlemock/include/gmock/gmock-nice-strict.h
  gtest/googlemock/include/gmock/gmock-spec-builders.h
  gtest/googlemock/include/gmock/gmock.h
  gtest/googlemock/include/gmock/internal/custom/gmock-generated-actions.h
  gtest/googlemock/include/gmock/internal/custom/gmock-matchers.h
  gtest/googlemock/include/gmock/internal/custom/gmock-port.h
  gtest/googlemock/include/gmock/internal/gmock-internal-utils.h
  gtest/googlemock/include/gmock/internal/gmock-port.h
  gtest/googlemock/include/gmock/internal/gmock-pp.h
  gtest/googlemock/src/gmock-cardinalities.cc
  gtest/googlemock/src/gmock-internal-utils.cc
  gtest/googlemock/src/gmock-matchers.cc
  gtest/googlemock/src/gmock-spec-builders.cc
  gtest/googlemock/src/gmock.cc
)
