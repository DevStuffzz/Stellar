workspace "Stellar"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "Vendor/glfw"
    include "Vendor/glad"
group ""

-- The vendor scripts default to the static CRT; Stellar is a DLL, so every module
-- must share the dynamic CRT to safely pass heap objects across the DLL boundary.
-- They also build into their own folders by default; redirect them to the shared bin/bin-int.
project "GLFW"
    staticruntime "Off"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

project "glad"
    staticruntime "Off"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

project "Stellar"
    location "Stellar"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Stellar/src/**.h",
        "Stellar/src/**.cpp"
    }

    includedirs {
        "Stellar/src",
        "Vendor/glfw/include",
        "Vendor/glad/include",
        "Vendor/glm"
    }

    links { "GLFW", "glad" }

    defines { "ST_BUILD_DLL", "GLFW_INCLUDE_NONE" }

    -- Exported classes hold STL members; safe because all modules share the same CRT.
    disablewarnings { "4251" }

    -- Copy Stellar.dll next to the Sandbox executable after every build.
    postbuildcommands {
        "{MKDIR} %[bin/" .. outputdir .. "/Sandbox]",
        "{COPYFILE} %[%{!cfg.buildtarget.abspath}] %[bin/" .. outputdir .. "/Sandbox]"
    }

    filter "system:windows"
        systemversion "latest"
        defines { "ST_PLATFORM_WINDOWS" }
        links { "opengl32" }

    filter "configurations:Debug"
        defines { "ST_DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "ST_RELEASE" }
        runtime "Release"
        optimize "On"

project "NativeScriptTests"
    location "Tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    targetdir ("bin/" .. outputdir .. "/Stellar")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files { "Tests/NativeScriptTests.cpp" }
    includedirs { "Stellar/src", "Vendor/glm" }
    links { "Stellar" }
    disablewarnings { "4251" }

    filter "system:windows"
        systemversion "latest"
        defines { "ST_PLATFORM_WINDOWS" }

    filter "configurations:Debug"
        defines { "ST_DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "ST_RELEASE" }
        runtime "Release"
        optimize "On"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Sandbox/src/**.h",
        "Sandbox/src/**.cpp"
    }

    includedirs {
        "Stellar/src",
        "Vendor/glm"
    }

    links { "Stellar" }

    disablewarnings { "4251" }

    -- Run from the output directory so the copied assets are in the working directory.
    debugdir ("bin/" .. outputdir .. "/%{prj.name}")

    -- Mirror Sandbox/assets into the output directory after every build.
    postbuildcommands {
        "{COPYDIR} %[Sandbox/assets] %[bin/" .. outputdir .. "/%{prj.name}/assets]"
    }

    filter "system:windows"
        systemversion "latest"
        defines { "ST_PLATFORM_WINDOWS" }

    filter "configurations:Debug"
        defines { "ST_DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "ST_RELEASE" }
        runtime "Release"
        optimize "On"
