cmake_minimum_required(VERSION 3.16)
project(xpilot LANGUAGES C CXX VERSION 2.0.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
	if(NOT DEFINED CONAN_LINK_RUNTIME AND DEFINED CONAN_SETTINGS_COMPILER_RUNTIME)
		message(WARNING "Forcing MSVC runtime based on conan profile")
		# this means Conan is not running the build, so we need to fix our linker flags ourself.... although we can probably cheat...
		set(CONAN_LINK_RUNTIME "/${CONAN_SETTINGS_COMPILER_RUNTIME}")
		conan_set_vs_runtime()
	endif()
endif(MSVC)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif(MSVC)

set(Header_Files
    include/AircraftManager.h
    include/Config.h
    include/Constants.h
    include/DataRefAccess.h
    include/FrameRateMonitor.h
    include/InterpolatedState.h
    include/NearbyATCWindow.h
    include/NetworkAircraft.h
	include/NetworkAircraftConfig.h
    include/NotificationPanel.h
    include/OwnedDataRef.h
    include/Plugin.h
    include/SettingsWindow.h
    include/StopWatch.h
    include/TerrainProbe.h
    include/TextMessageConsole.h
    include/Utilities.h
    include/XPilot.h
    include/XPilotAPI.h
    include/XplaneCommand.h
    extern/imgui/imgui_stdlib.h
    extern/ImgWindow/ImgFontAtlas.h
    extern/ImgWindow/ImgWindow.h
    extern/ImgWindow/SystemGL.h
    extern/ImgWindow/XPImgWindow.h
	extern/Font/IconsFontAwesome5.h
	extern/Font/fa-solid-900.inc
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    src/AircraftManager.cpp
    src/Config.cpp
    src/DataRefAccess.cpp
    src/FrameRateMonitor.cpp
    src/NearbyATCWindow.cpp
    src/NetworkAircraft.cpp
	src/NetworkAircraftConfig.cpp
    src/NotificationPanel.cpp
    src/OwnedDataRef.cpp
    src/Plugin.cpp
    src/SettingsWindow.cpp
    src/Stopwatch.cpp
    src/TerrainProbe.cpp
    src/TextMessageConsole.cpp
    src/XPilot.cpp
    ${CMAKE_SOURCE_DIR}/extern/ImgWindow/XPImgWindow.cpp
    ${CMAKE_SOURCE_DIR}/extern/ImgWindow/ImgFontAtlas.cpp
    ${CMAKE_SOURCE_DIR}/extern/ImgWindow/ImgWindow.cpp
    ${CMAKE_SOURCE_DIR}/extern/ImGui/imgui_stdlib.cpp
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES  ${Header_Files} ${Source_Files})

add_library(xpilot MODULE ${ALL_FILES})

target_include_directories(xpilot
		PUBLIC
		${CMAKE_SOURCE_DIR}/include
		${CMAKE_SOURCE_DIR}/extern
		${CMAKE_SOURCE_DIR}/extern/gmath
		${CMAKE_SOURCE_DIR}/extern/ImGui
		${CMAKE_SOURCE_DIR}/extern/ImgWindow
		${CMAKE_SOURCE_DIR}/extern/SDK/CHeaders/XPLM
		${CMAKE_SOURCE_DIR}/extern/XPMP2)

if (APPLE)
    list(APPEND CMAKE_FRAMEWORK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/XPMP2")
    list(APPEND CMAKE_FRAMEWORK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/sdk/Libraries/Mac")
elseif (WIN32)
    list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/XPMP2")
    list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/sdk/Libraries/Win")
endif()

# Link the XPMP2 library
find_library(XPMP2_LIBRARY XPMP2 REQUIRED)
message ("   XPMP2_LIBRARY = ${XPMP2_LIBRARY}")
target_link_libraries(xpilot ${XPMP2_LIBRARY})

target_link_libraries(xpilot
	CONAN_PKG::nlohmann_json
	CONAN_PKG::zeromq
	CONAN_PKG::cppzmq
	CONAN_PKG::imgui
	${XPMP2_LIBRARY})

if (WIN32 OR APPLE)
    find_library(XPLM_LIBRARY NAMES XPLM XPLM_64.lib)
    target_link_libraries(xpilot ${XPLM_LIBRARY})
endif()

if (APPLE)
    find_library(APPLICATION_SERVICES ApplicationServices)
    find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
    find_library(SECURITY_LIBRARY Security)
    find_library(GSS_LIBRARY GSS)
    find_library(Kerberos5_LIBRARY libgssapi_krb5.tbd)
    find_library(Cocoa_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)

    target_link_libraries(xpilot
        ${APPLICATION_SERVICES}
        ${CORE_FOUNDATION_LIBRARY}
        ${SECURITY_LIBRARY}
        ${GSS_LIBRARY}
        ${Kerberos5_LIBRARY}
        ${OpenGL_LIBRARY}
        ${Cocoa_LIBRARY}
    )
endif()

if (WIN32)
    target_link_libraries(xpilot ws2_32.lib wldap32.lib advapi32.lib crypt32.lib wsock32.lib opengl32.lib iphlpapi.lib)
elseif (APPLE)
    target_compile_options(xpilot PUBLIC -mmacosx-version-min=10.15)
    target_link_libraries(xpilot -mmacosx-version-min=10.15)
    target_link_libraries(xpilot "-exported_symbols_list ${CMAKE_SOURCE_DIR}/src/xPilot.sym_mac")
elseif (UNIX)
    target_link_libraries(xpilot -Wl,--version-script -Wl,${CMAKE_SOURCE_DIR}/src/xPilot.sym)
endif ()

set (OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)  # apt install freeglut3-dev
if (OpenGL_FOUND)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(xpilot ${OpenGL_LIBRARIES})
endif(OpenGL_FOUND)

if(MSVC)
	set_property(TARGET xpilot PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_definitions(-DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM303=1)
add_definitions(-DAPL=$<BOOL:${APPLE}> -DIBM=$<BOOL:${WIN32}> -DLIN=$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>)

if (UNIX OR APPLE)
    add_compile_options(-fexceptions -fpermissive)
    add_compile_options(-fvisibility=hidden)
    add_compile_options(-Wall -Wshadow -Wfloat-equal -Wextra)
    add_compile_options(-Wno-unused)
    add_compile_options(-O3 -fPIC)
    add_compile_options(-pthread)
elseif(WIN32)
    add_compile_options(/wo4996)
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
endif()

if (MSVC)
   set_target_properties(xpilot PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/win_x64")
elseif (APPLE)
   set_target_properties(xpilot PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/mac_x64")
elseif (UNIX)
   set_target_properties(xpilot PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lin_x64")
endif()

if(MSVC)
    target_link_options(xpilot PUBLIC /DEBUG)
endif()

set_target_properties(xpilot
    PROPERTIES
    PREFIX ""
    OUTPUT_NAME "xPilot"
    SUFFIX ".xpl"
)