workspace "Snake"
   configurations { "Debug", "Release" }
   platforms "x64"

project "Client"
   kind "ConsoleApp"

   language "C++"
   cppdialect "C++17"

   debugdir "bin"
   targetdir "bin"

   libdirs "thirdparty/SFML/lib"
   sysincludedirs "thirdparty/SFML/include"

   files { "**.hpp", "**.cpp" }
   removefiles "sv_*.*"

   links "ws2_32"

   filter "configurations:Debug"
      defines { "DEBUG" }
      links { "sfml-system-d", "sfml-window-d", "sfml-graphics-d" }
      symbols "On"
      targetsuffix "-d"

   filter "configurations:Release"
      defines { "NDEBUG" }
      links { "sfml-system", "sfml-window", "sfml-graphics" }
      optimize "On"

project "Server"
   kind "ConsoleApp"

   language "C++"
   cppdialect "C++17"

   debugdir "bin"
   targetdir "bin"

   libdirs "thirdparty/SFML/lib"
   sysincludedirs "thirdparty/SFML/include"

   files { "**.hpp", "**.cpp" }
   removefiles "cl_*.*"

   links "ws2_32"

   filter "configurations:Debug"
      defines { "DEBUG" }
      links "sfml-system-d"
      symbols "On"
      targetsuffix "-d"

   filter "configurations:Release"
      defines { "NDEBUG" }
      links "sfml-system"
      optimize "On"
