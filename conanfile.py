from conan import ConanFile
from conan.tools.cmake import cmake_layout


class TAOWRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("catch2/3.12.0")
        self.requires("nlohmann_json/3.12.0")
        self.requires("boost/1.90.0")
        self.requires("zlib/1.3.1")
        self.requires("openssl/3.6.0")

    def layout(self):
        cmake_layout(self)
