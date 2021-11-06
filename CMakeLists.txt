cmake_minimum_required(VERSION 3.16)
project(xpilot LANGUAGES C CXX VERSION 2.0.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

if(APPLE)
    add_compile_options(-mmacosx-version-min=10.15)
    add_link_options(-mmacosx-version-min=10.15)
endif()

set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED 17)
set_property(GLOBAL PROPERTY CXX_STANDARD 17)

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

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/extern/gmath")
include_directories("${CMAKE_SOURCE_DIR}/extern/ImGui")
include_directories("${CMAKE_SOURCE_DIR}/extern/ImgWindow")
include_directories("${CMAKE_SOURCE_DIR}/extern/XPMP2/XPMP2.framework/Versions/Current/Headers")
include_directories("${CMAKE_SOURCE_DIR}/extern/SDK/CHeaders/XPLM")

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
    extern/XPMP2/XPMP2.framework/Versions/Current/Headers/XPCAircraft.h
    extern/XPMP2/XPMP2.framework/Versions/Current/Headers/XPMPMultiplayer.h
    extern/XPMP2/XPMP2.framework/Versions/Current/Headers/XPMPPlaneRenderer.h
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

if (APPLE)
    list(APPEND CMAKE_FRAMEWORK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/XPMP2")
    list(APPEND CMAKE_FRAMEWORK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/sdk/Libraries/Mac")
elseif (WIN32)
    list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/XPMP2/win")
    list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/sdk/Libraries/Win")
endif()

# Link the XPMP2 library
find_library(XPMP2_LIBRARY XPMP2 REQUIRED)
message ("XPMP2_LIBRARY = ${XPMP2_LIBRARY}")
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

if (WIN32)
    set_property(TARGET xpilot PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_link_libraries(xpilot ws2_32.lib wldap32.lib advapi32.lib crypt32.lib wsock32.lib opengl32.lib iphlpapi.lib)
    target_link_options(xpilot PUBLIC /DEBUG)
elseif (APPLE)
    # Link OS X core system libraries.
    find_library(CORE_FOUNDATION_LIBRARY CoreFoundation REQUIRED)
    find_library(Cocoa_LIBRARY Cocoa REQUIRED)
    find_library(Security_LIBRARY Security REQUIRED)
    find_library(GSS_LIBRARY GSS REQUIRED)
    find_library(OpenGL_LIBRARY OpenGL REQUIRED)

    target_link_libraries(xpilot
        ${CORE_FOUNDATION_LIBRARY}
        ${Cocoa_LIBRARY}
        ${Security_LIBRARY}
        ${GSS_LIBRARY}
        ${OpenGL_LIBRARY})

    # Restrict set of symbols exported from the plugin to the ones required by XPLM:
    target_link_libraries(xpilot "-exported_symbols_list ${CMAKE_SOURCE_DIR}/src/Xpilot.sym_mac")
elseif (UNIX)
    # Link library for dynamic loading of shared objects on UNIX systems.
    find_library(DL_LIBRARY dl REQUIRED)
    # Threads
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads REQUIRED)
    target_link_libraries(LiveTraffic ${DL_LIBRARY} Threads::Threads)
    # Specify additional runtime search paths for dynamically-linked libraries.
    # Restrict set of symbols exported from the plugin to the ones required by XPLM:
    target_link_libraries(LiveTraffic -Wl,--version-script -Wl,${CMAKE_SOURCE_DIR}/src/Xpilot.sym)    
endif ()

# Link OpenGL
set (OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)  # apt install freeglut3-dev
include_directories( ${OpenGL_INCLUDE_DIRS} )
target_link_libraries( xpilot ${OpenGL_LIBRARIES} )

# Enable all X-Plane SDK APIs up to the newest version.
add_definitions(-DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM303=1)

# Define platform macros.
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

set_target_properties(xpilot
    PROPERTIES
    PREFIX ""
    OUTPUT_NAME "xPilot"
    SUFFIX ".xpl"
)