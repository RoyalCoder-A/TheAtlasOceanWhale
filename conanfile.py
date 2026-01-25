from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout


class TAOWRecipe(ConanFile):
    name = "taow"
    version = "0.0.1"

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    exports_sources = ["src/*", "CMakeLists.txt"]

    def config_options(self):
        if self.settings.os == "Windows":
            del self.settings.fPIC

    def requirements(self):
        self.requires("nlohmann_json/3.12.0")
        self.requires("boost/1.90.0")
        self.requires("zlib/1.3.1")
        self.requires("openssl/3.6.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.components["taow_cli_builder"].libs = ["taow_cli_builder"]
        self.cpp_info.components["taow_cli_builder"].requires = [
            "boost::headers",
            "openssl::ssl",
            "openssl::crypto",
            "taow_utils",
            "zlib::zlib",
        ]
        self.cpp_info.components["taow_json"].set_property(
            "cmake_target_name", "TAOW::json"
        )
        self.cpp_info.components["taow_json"].requires = [
            "nlohmann_json::nlohmann_json"
        ]
        self.cpp_info.components["taow_logging"].libs = ["taow_logging"]
        self.cpp_info.components["taow_logging"].requires = ["taow_utils"]
        self.cpp_info.components["taow_utils"].libs = ["taow_utils"]
