from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake

class MyGameProjectRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def requirements(self):
        self.requires("sdl/3.2.20")
        #self.requires("glfw/3.4")
        #self.requires("sfml/3.0.2")
        #self.requires("glew/2.2.0")
        #self.requires("glm/1.0.1")
        #self.requires("miniaudio/0.11.22")
        #self.requires("entt/3.16.0")
        #self.requires("imgui/1.92.5")
        #self.requires("boost/1.90.0")
        #self.requires("fmt/12.1.0")
        #self.requires("spdlog/1.17.0")
        #self.requires("magic_enum/0.9.7")
        #self.requires("nlohmann_json/3.12.0")
        self.requires("gtest/1.17.0")
        #self.requires("nameof/0.10.4")
        # self.requires("lz4/1.10.0")
        # self.requires("box2d/3.1.1")

    def layout(self):
        cmake_layout(self)