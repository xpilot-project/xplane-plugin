cmake_minimum_required(VERSION 3.16)
project(xPilot LANGUAGES CXX)

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/Lib")
include_directories("${CMAKE_SOURCE_DIR}/Lib/ImGui")
include_directories("${CMAKE_SOURCE_DIR}/Lib/ImGui/misc/cpp")
include_directories("${CMAKE_SOURCE_DIR}/Lib/ImgWindow")
include_directories("${CMAKE_SOURCE_DIR}/Lib/SDK/CHeaders/XPLM")
include_directories("${CMAKE_SOURCE_DIR}/Lib/XPMP2")
include_directories("${CMAKE_SOURCE_DIR}/Lib/Font")
include_directories("${CMAKE_SOURCE_DIR}/Lib/gmath")
include_directories("${CMAKE_SOURCE_DIR}/Lib/protobuf")

add_definitions(-DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM303=1 -DZMQ_STATIC=1)
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

set(Header_Files
    include/AircraftManager.h
    include/Config.h
    include/Constants.h
    include/DataRefAccess.h
    include/FrameRateMonitor.h
    include/InterpolatedState.h
    include/NearbyATCWindow.h
    include/NetworkAircraft.h
    include/NotificationPanel.h
    include/OwnedDataRef.h
    include/Plugin.h
    include/SettingsWindow.h
    include/sha512.hh
    include/StopWatch.h
    include/TerrainProbe.h
    include/TextMessageConsole.h
    include/Utilities.h
    include/XPilot.h
    include/XPilotAPI.h
    include/XplaneCommand.h
    ${CMAKE_SOURCE_DIR}/Lib/imgui/imstb_rectpack.h
    ${CMAKE_SOURCE_DIR}/Lib/imgui/imstb_textedit.h
    ${CMAKE_SOURCE_DIR}/Lib/imgui/imstb_truetype.h
    ${CMAKE_SOURCE_DIR}/Lib/imgui/imgui_internal.h
    ${CMAKE_SOURCE_DIR}/Lib/imgui/imgui.h
    ${CMAKE_SOURCE_DIR}/Lib/imgui/imconfig.h
    ${CMAKE_SOURCE_DIR}/Lib/imgui/misc/cpp/imgui_stdlib.h
    ${CMAKE_SOURCE_DIR}/Lib/ImgWindow/ImgFontAtlas.h
    ${CMAKE_SOURCE_DIR}/Lib/ImgWindow/ImgWindow.h
    ${CMAKE_SOURCE_DIR}/Lib/ImgWindow/SystemGL.h
    ${CMAKE_SOURCE_DIR}/Lib/ImgWindow/XPImgWindow.h
	${CMAKE_SOURCE_DIR}/Lib/ZMQ/zmq.h
	${CMAKE_SOURCE_DIR}/Lib/ZMQ/zmq.hpp
    ${CMAKE_SOURCE_DIR}/Lib/json.hpp
	${CMAKE_SOURCE_DIR}/Lib/Font/IconsFontAwesome5.h
	${CMAKE_SOURCE_DIR}/Lib/Font/fa-solid-900.inc
)
source_group("Header Files" FILES ${Header_Files})

AUX_SOURCE_DIRECTORY("${CMAKE_SOURCE_DIR}/protobuf" PROTOBUF_SRC)

set(Source_Files
    src/AircraftManager.cpp
    src/Config.cpp
    src/DataRefAccess.cpp
    src/FrameRateMonitor.cpp
    src/NearbyATCWindow.cpp
    src/NetworkAircraft.cpp
    src/NotificationPanel.cpp
    src/OwnedDataRef.cpp
    src/Plugin.cpp
    src/SettingsWindow.cpp
    src/Stopwatch.cpp
    src/TerrainProbe.cpp
    src/TextMessageConsole.cpp
    src/XPilot.cpp
    ${CMAKE_SOURCE_DIR}/Lib/ImgWindow/XPImgWindow.cpp
    ${CMAKE_SOURCE_DIR}/Lib/ImgWindow/ImgFontAtlas.cpp
    ${CMAKE_SOURCE_DIR}/Lib/ImgWindow/ImgWindow.cpp
    ${CMAKE_SOURCE_DIR}/Lib/imgui/misc/cpp/imgui_stdlib.cpp
    ${CMAKE_SOURCE_DIR}/Lib/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/Lib/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/Lib/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/Lib/imgui/imgui_widgets.cpp
	${PROTOBUF_SRC}
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES  ${Header_Files} ${Source_Files})

add_library(xPilot MODULE ${ALL_FILES})

if (APPLE)
    list(APPEND CMAKE_FRAMEWORK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Lib/XPMP2")
	list(APPEND CMAKE_FRAMEWORK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Lib/ZMQ")
    list(APPEND CMAKE_FRAMEWORK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Lib/sdk/Libraries/Mac")
elseif (WIN32)
    list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Lib/XPMP2")
	list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Lib/ZMQ")
    list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Lib/sdk/Libraries/Win")
	list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Lib/protobuf")
endif()

if (APPLE)
    find_library(XPMP2_LIBRARY NAMES XPMP2)
elseif (UNIX)
    find_library(XPMP2_LIBRARY NAMES libXPMP2.a)
elseif (WIN32)
    find_library(XPMP2_LIBRARY NAMES XPMP2.lib)
endif()

target_link_libraries(xPilot ${XPMP2_LIBRARY})

if (WIN32)
	find_library(ZLIB_LIBRARY NAMES zlib.lib)
	find_library(ZMQ_LIBRARY NAMES libzmq.lib)
	find_library(PROTOBUF_LIB NAMES libprotobuf.lib)
endif()

target_link_libraries(xPilot ${ZLIB_LIBRARY})
target_link_libraries(xPilot ${ZMQ_LIBRARY})
target_link_libraries(xPilot ${PROTOBUF_LIB})

if (WIN32 OR APPLE)
    find_library(XPLM_LIBRARY NAMES XPLM XPLM_64.lib)
    target_link_libraries(xPilot ${XPLM_LIBRARY} )
endif()

if (APPLE)
    find_library(APPLICATION_SERVICES ApplicationServices)
    find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
    find_library(SECURITY_LIBRARY Security)
    find_library(GSS_LIBRARY GSS)
    find_library(Kerberos5_LIBRARY libgssapi_krb5.tbd)
    find_library(Cocoa_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)

    target_link_libraries(xPilot
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
    target_link_libraries(xPilot ws2_32.lib wldap32.lib advapi32.lib crypt32.lib wsock32.lib opengl32.lib iphlpapi.lib)
elseif (APPLE)
    target_compile_options(xPilot PUBLIC -mmacosx-version-min=10.11)
    target_link_libraries(xPilot -mmacosx-version-min=10.11)
    target_link_libraries(xPilot "-exported_symbols_list ${CMAKE_SOURCE_DIR}/src/xPilot.sym_mac")
elseif (UNIX)
    target_link_libraries(xPilot -Wl,--version-script -Wl,${CMAKE_SOURCE_DIR}/src/xPilot.sym)
endif ()

set (OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)  # apt install freeglut3-dev
if (OpenGL_FOUND)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(xPilot ${OpenGL_LIBRARIES})
endif(OpenGL_FOUND)

if(MSVC)
	set_property(TARGET xPilot PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(CMAKE_CXX_STANDARD 17)
target_compile_features(xPilot PUBLIC cxx_std_17)
set_property(TARGET xPilot PROPERTY CXX_STANDARD_REQUIRED 17)
set_property(TARGET xPilot PROPERTY CXX_STANDARD 17)

if (MSVC)
   set_target_properties(xPilot PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/win_x64")
elseif (APPLE)
   set_target_properties(xPilot PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/mac_x64")
elseif (UNIX)
   set_target_properties(xPilot PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lin_x64")
endif()

if(MSVC)
    target_link_options(xPilot PUBLIC /DEBUG)
endif()

set_target_properties(xPilot
    PROPERTIES
    PREFIX ""
    OUTPUT_NAME "xPilot"
    SUFFIX ".xpl"
)