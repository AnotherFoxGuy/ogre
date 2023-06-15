import os
from conan import ConanFile
from conan.tools.files import copy


class OGRE(ConanFile):
    name = "OGRE"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("cg-toolkit/3.1@anotherfoxguy/stable")
        self.requires("freeimage/3.18.0")
        self.requires("freetype/2.12.1")
        self.requires("libpng/1.6.39")
        self.requires("pugixml/1.13")
        self.requires("sdl/2.26.1")

        self.requires("zlib/1.2.13", force=True)

        
    # def generate(self):
    #     for dep in self.dependencies.values():
    #         for f in dep.cpp_info.bindirs:
    #             self.cp_data(f)
    #         for f in dep.cpp_info.libdirs:
    #             self.cp_data(f)

    # def cp_data(self, src):
    #     bindir = os.path.join(self.build_folder, "bin")
    #     copy(self, "*.dll", src, bindir, False)
    #     copy(self, "*.so*", src, bindir, False)
