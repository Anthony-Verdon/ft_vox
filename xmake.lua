set_targetdir("./")
set_languages("cxx17")

add_ldflags("-stdlib=libc++")
add_cxxflags("-std=c++17")
add_cxxflags("-Wall")
add_cxxflags("-Werror")
add_cxxflags("-Wextra")
add_cxxflags("-O3")

add_requires("freetype")
add_requires("glfw")
add_requires("glm")
add_requires("stb")
add_requires("glad")

rule("mode.debug")
    before_build(function (target)
        target:add("defines", "DEBUG_MODE")
    end)
rule_end()

rule("mode.generateMap")
    before_build(function (target)
        target:add("defines", "GENERATE_MAP")
    end)
rule_end()

target("ft_vox")
    set_kind("binary")
    if is_mode("debug") then
        add_rules("mode.debug")
        set_symbols("debug") -- add -g when compile
    end
    
    if is_mode("gen") then
        add_rules("mode.generateMap")
    end
        
    add_files("srcs/**.cpp")
    add_headerfiles("srcs/**.hpp")
    add_packages("freetype")
    add_packages("glfw")
    add_packages("glm")
    add_packages("stb")
    add_packages("glad")