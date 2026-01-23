from conan import ConanFile
from conan.tools.cmake import cmake_layout

class ExampleRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("sdl3/3.2.20")
        self.requires("sdl_ttf/3.2.2")
        self.requires("glfw/3.4")
        self.requires("glew/2.2.0")
        self.requires("glm/1.0.1")
        self.requires("lz4/1.10.0")
        self.requires("fmt/12.1.0")
        self.requires("spdlog/1.17.0")
        self.requires("entt/3.16.0")
        self.requires("box2d/3.1.1")
        self.requires("yaml_cpp/0.8.0")
        self.requires("magic_enum/0.9.7")
        self.requires("catch2/3.12.0")
        self.requires("imgui/1.92.5")
        self.requires("gtest/1.17.0")
        self.requires("boost/1.90.0")

    def layout(self):
        cmake_layout(self)