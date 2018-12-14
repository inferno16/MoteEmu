-- Global variables
outputdir = "%{cfg.buildcfg}_%{cfg.system}_%{cfg.architecture}"
inputdir = "%{prj.name}/src"

-- Workspace definition (applies to all projects)
workspace "MoteEmu"
    configurations { "Debug", "Release" }
    platforms { "x86", "x64" }
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        inputdir .. "/*.h",
        inputdir .. "/*.cpp"
    }

    vpaths 
    {
        ["Header Files"] = { "**.hpp", "**.h" },
        ["Source Files"] = { "**.cpp", "**.cc", "**.c" }
    }

    filter "platforms:x86"
        architecture "x86"

    filter "platforms:x64"
        architecture "x64"
        
    filter "configurations:Debug"
        kind "ConsoleApp"
        defines { "DEBUG" }
        optimize "Debug"

    filter "configurations:Release"
        kind "WindowedApp"
        defines { "NDEBUG" }
        optimize "Full"

    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"


-- Project definitions
project "MoteEmu"
    location "MoteEmu"

    includedirs { "vendor/SDL2/include" }
    links { "SDL2", "SDL2main" }

    filter "platforms:x86"
        libdirs { "vendor/SDL2/lib/x86" }

    filter "platforms:x64"
        libdirs { "vendor/SDL2/lib/x64" }
        