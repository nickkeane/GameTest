from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout


class ConanApplication(ConanFile):
    name = "MyGameProject"
    version = "1.0.0"
    package_type = "application"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    tool_requires = "cmake/[>=4.0]"

    def requirements(self) -> None:
        if self.requires is not None:
            pass

    # TODO: Port over requirements
    # def requirements(self) -> None:
    #     self.requires("assimp/6.0.2")
    #     #self.requires("lz4/1.10.0")

    def build_requirements(self) -> None:
        pass

    def configure(self) -> None:
        if self.options is not None:
            pass

    def layout(self) -> None:
        cmake_layout(self)

    def generate(self) -> None:
        tc = CMakeToolchain(self)
        #tc.user_presets_path = False
        tc.generate()

    def build(self) -> None:
        cmake = CMake(self)
        cmake.configure()
        cmake.build()