
from plexconantool import PlexConanFile

class Crashpad(PlexConanFile):
  name = "crashpad"
  version = "1.0"
  plex_requires = "zlib/1.2.11-5", "cmaketoolchain/1-14", "cmakecache/1-15"
  settings = "os", "compiler", "build_type", "arch"
  url = "http://plex.tv"
  description = "Crashpad"
  license = "proprietary"
  generators = "cmake", "CMakeCache", "CMakeToolchain"
  options = {"devel": [True, False], "variation": ["standard", "desktop"]}
  initial_cache_variables = {
    "CMAKE_TOOLCHAIN_FILE": "plex-toolchain.cmake"
  }