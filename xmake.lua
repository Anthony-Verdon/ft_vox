set_targetdir("./")

add_requires("freetype")
add_requires("glfw")
add_requires("glm")
add_requires("stb")
add_requires("glad")

target("ft_vox")
    set_kind("binary")
    add_files("srcs/**.cpp")
    add_packages("freetype")
    add_packages("glfw")
    add_packages("glm")
    add_packages("stb")
    add_packages("glad")
    set_symbols("debug") -- add -g when compile