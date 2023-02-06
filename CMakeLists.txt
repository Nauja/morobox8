cmake_minimum_required(VERSION 3.3)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
 
if (MSVC)
list(APPEND MOROBOX8_COMPILE_FLAGS /W4 /WX)
else()
list(APPEND MOROBOX8_COMPILE_FLAGS -Wall -Wextra -pedantic -Werror -Wno-unused-parameter)
endif()

# Specify search path for CMake modules to be loaded by include() 
# and find_package()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

include(ExternalProject)

set(MOROBOX8_VERSION_MAJOR 0)
set(MOROBOX8_VERSION_MINOR 0)
set(MOROBOX8_VERSION_PATCH 1)
set(MOROBOX8_VERSION "${MOROBOX8_VERSION_MAJOR}.${MOROBOX8_VERSION_MINOR}.${MOROBOX8_VERSION_PATCH}")
set(MOROBOX8_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(MOROBOX8_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(MOROBOX8_VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

#find_package(Git)
if(Git_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} status
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        ERROR_VARIABLE RESULT_STRING
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    string(LENGTH "${RESULT_STRING}" LENGTH_RESULT_STRING)

    if(${LENGTH_RESULT_STRING} EQUAL 0)

        execute_process(
            COMMAND ${GIT_EXECUTABLE} log -1 --format=%H
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        string(SUBSTRING ${GIT_COMMIT_HASH} 0 7 GIT_COMMIT_HASH)
        set(VERSION_HASH ${GIT_COMMIT_HASH} )

        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-list HEAD --count
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE VERSION_REVISION
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

    endif()
endif()

project(morobox8 VERSION ${MOROBOX8_VERSION} LANGUAGES C CXX)

include(ConfigureChecks)

if(EMSCRIPTEN OR RPI)
    set(MOROBOX8_FILESYSTEM_DEFAULT OFF)
    set(MOROBOX8_SDL2_DEFAULT OFF)
    set(MOROBOX8_PLAYER_DEFAULT OFF)
    set(MOROBOX8_HOOK_DEFAULT ON)
    set(MOROBOX8_SERVEREMU_DEFAULT OFF)
else()
    set(MOROBOX8_FILESYSTEM_DEFAULT ON)
    set(MOROBOX8_SDL2_DEFAULT ON)
    set(MOROBOX8_PLAYER_DEFAULT ON)
    set(MOROBOX8_HOOK_DEFAULT OFF)
    set(MOROBOX8_SERVEREMU_DEFAULT ON)
endif()

option(MOROBOX8_STATIC "Build a static library" ON)
option(MOROBOX8_UNIT_TESTING "Unit Tests Enabled" OFF)
option(MOROBOX8_DOXYGEN "Docs Enabled" OFF)
option(MOROBOX8_LUA_API "Build with LUA API" ON)
option(MOROBOX8_JS_API "Build with JS API" ON)
option(MOROBOX8_FILESYSTEM "Build with support for filesystem" ${MOROBOX8_FILESYSTEM_DEFAULT})
option(MOROBOX8_SDL2 "Build with SDL2" ${MOROBOX8_SDL2_DEFAULT})
option(MOROBOX8_PLAYER "Build the player" ${MOROBOX8_PLAYER_DEFAULT})
option(MOROBOX8_HOOK "Build with hooks" ${MOROBOX8_HOOK_DEFAULT})
option(MOROBOX8_SERVEREMU "Build the serveremu" ${MOROBOX8_SERVEREMU_DEFAULT})

set(MOROBOX8_WEBSOCKETS ON)

if (RPI OR EMSCRIPTEN)
set(MOROBOX8_UNIT_TESTING OFF)
set(MOROBOX8_DOXYGEN OFF)
set(MOROBOX8_FILESYSTEM OFF)
set(MOROBOX8_SDL2 OFF)
set(MOROBOX8_PLAYER OFF)
set(MOROBOX8_HOOK ON)
set(MOROBOX8_SERVEREMU OFF)
set(MOROBOX8_WEBSOCKETS OFF)
endif (RPI OR EMSCRIPTEN)

if(MOROBOX8_PLAYER)
set(MOROBOX8_STATIC ON)
endif(MOROBOX8_PLAYER)

# disallow in-source build
include(MacroEnsureOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build. Please create a separate build directory and run 'cmake /path/to/${PROJECT_NAME} [options]' there.")

message("Building for target : ${CMAKE_SYSTEM_NAME}")
message("PROJECT_VERSION: ${MOROBOX8_VERSION}")
message("VERSION_HASH: ${VERSION_HASH}")
configure_file(morobox8_config.h.in ${MOROBOX8_INCLUDE_DIR}/morobox8_config.h)

set(MOROBOX8_LIB_INCLUDE_DIRS ${MOROBOX8_INCLUDE_DIR})
set(MOROBOX8_PLAYER_INCLUDE_DIRS ${MOROBOX8_INCLUDE_DIR})
set(MOROBOX8_SERVEREMU_INCLUDE_DIRS ${MOROBOX8_INCLUDE_DIR})
set(MOROBOX8_LIB_COMPILE_FLAGS ${MOROBOX8_COMPILE_FLAGS})
list(APPEND MOROBOX8_PLAYER_COMPILE_FLAGS ${MOROBOX8_COMPILE_FLAGS} -g -O0)
set(MOROBOX8_SERVEREMU_COMPILE_FLAGS ${MOROBOX8_COMPILE_FLAGS})

if (EMSCRIPTEN)
set(MOROBOX8_LIB_COMPILE_FLAGS ${MOROBOX8_LIB_COMPILE_FLAGS} -DMOROBOX8_COLOR_FORMAT=2)
endif (EMSCRIPTEN)

# SDL2
if(MOROBOX8_SDL2)

    set(MOROBOX8_VENDOR_LIBS ${MOROBOX8_VENDOR_LIBS} )

    if(WIN32)
        set(HAVE_LIBC TRUE)
    endif()

    if(ANDROID)
        include_directories(${ANDROID_NDK}/sources/android/cpufeatures)
        set(SDL_STATIC_PIC ON CACHE BOOL "" FORCE)
    endif()

    set(SDL_SHARED OFF CACHE BOOL "" FORCE)

    add_subdirectory(${MOROBOX8_VENDOR_DIR}/sdl2)
    set(MOROBOX8_PLAYER_LINKS SDL2-static SDL2main)
    set(MOROBOX8_PLAYER_INCLUDE_DIRS ${MOROBOX8_PLAYER_INCLUDE_DIRS} ${MOROBOX8_VENDOR_DIR}/sdl2/include)
    set(MOROBOX8_PLAYER_SOURCES ${MOROBOX8_PLAYER_SOURCES} ${MOROBOX8_SOURCE_DIR}/player/sdl2.c)

endif(MOROBOX8_SDL2)

# LUA

if(MOROBOX8_LUA_API)

    set(MOROBOX8_LIB_LINKS ${MOROBOX8_LIB_LINKS} lua)
    set(LUA_DIR ${MOROBOX8_VENDOR_DIR}/lua)
    set(LUA_SOURCES
        ${LUA_DIR}/lapi.c
        ${LUA_DIR}/lcode.c
        ${LUA_DIR}/lctype.c
        ${LUA_DIR}/ldebug.c
        ${LUA_DIR}/ldo.c
        ${LUA_DIR}/ldump.c
        ${LUA_DIR}/lfunc.c
        ${LUA_DIR}/lgc.c
        ${LUA_DIR}/llex.c
        ${LUA_DIR}/lmem.c
        ${LUA_DIR}/lobject.c
        ${LUA_DIR}/lopcodes.c
        ${LUA_DIR}/lparser.c
        ${LUA_DIR}/lstate.c
        ${LUA_DIR}/lstring.c
        ${LUA_DIR}/ltable.c
        ${LUA_DIR}/ltm.c
        ${LUA_DIR}/lundump.c
        ${LUA_DIR}/lvm.c
        ${LUA_DIR}/lzio.c
        ${LUA_DIR}/lauxlib.c
        ${LUA_DIR}/lbaselib.c
        ${LUA_DIR}/lcorolib.c
        ${LUA_DIR}/ldblib.c
        ${LUA_DIR}/liolib.c
        ${LUA_DIR}/lmathlib.c
        ${LUA_DIR}/loslib.c
        ${LUA_DIR}/lstrlib.c
        ${LUA_DIR}/ltablib.c
        ${LUA_DIR}/lutf8lib.c
        ${LUA_DIR}/loadlib.c
        ${LUA_DIR}/linit.c
    )

    add_library(lua STATIC ${LUA_SOURCES})

    target_compile_definitions(lua PRIVATE LUA_COMPAT_5_2)
    target_include_directories(lua INTERFACE ${LUA_DIR})

endif(MOROBOX8_LUA_API)

# Duktape

if(MOROBOX8_JS_API)

    set(MOROBOX8_LIB_LINKS ${MOROBOX8_LIB_LINKS} duktape)
    add_library(duktape STATIC ${MOROBOX8_VENDOR_DIR}/duktape/src/duktape.c)
    target_include_directories(duktape INTERFACE ${MOROBOX8_VENDOR_DIR}/duktape/src)

endif(MOROBOX8_JS_API)

# ArgParse lib

if(MOROBOX8_PLAYER)

    add_library(argparse STATIC ${MOROBOX8_VENDOR_DIR}/argparse/argparse.c)
    target_include_directories(argparse INTERFACE ${MOROBOX8_VENDOR_DIR}/argparse)
    set(MOROBOX8_PLAYER_LINKS ${MOROBOX8_PLAYER_LINKS} argparse)
    set(MOROBOX8_SERVEREMU_LINKS ${MOROBOX8_SERVEREMU_LINKS} argparse)

endif(MOROBOX8_PLAYER)

# zlib

if (MOROBOX8_FILESYSTEM)

    set(ZLIB_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/zlib")
    set(ZLIB_INCLUDE_DIR "${ZLIB_SOURCE_DIR}")

    set(ZLIB_SOURCES
        ${ZLIB_SOURCE_DIR}/adler32.c
        ${ZLIB_SOURCE_DIR}/compress.c
        ${ZLIB_SOURCE_DIR}/crc32.c
        ${ZLIB_SOURCE_DIR}/deflate.c
        ${ZLIB_SOURCE_DIR}/inflate.c
        ${ZLIB_SOURCE_DIR}/infback.c
        ${ZLIB_SOURCE_DIR}/inftrees.c
        ${ZLIB_SOURCE_DIR}/inffast.c
        ${ZLIB_SOURCE_DIR}/trees.c
        ${ZLIB_SOURCE_DIR}/uncompr.c
        ${ZLIB_SOURCE_DIR}/zutil.c
    )

    add_library(zlib STATIC ${ZLIB_SOURCES})
    target_include_directories(zlib INTERFACE ${ZLIB_INCLUDE_DIR})
    target_compile_options(zlib
    PRIVATE
        -Wno-deprecated-non-prototype)

endif (MOROBOX8_FILESYSTEM)

# libpng

if (MOROBOX8_FILESYSTEM)

    set(LIBPNG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/libpng")
    set(LIBPNG_INCLUDE_DIR "${LIBPNG_SOURCE_DIR}")

    set(LIBPNG_SOURCES
        ${LIBPNG_SOURCE_DIR}/png.c
        ${LIBPNG_SOURCE_DIR}/pngerror.c
        ${LIBPNG_SOURCE_DIR}/pngget.c
        ${LIBPNG_SOURCE_DIR}/pngmem.c
        ${LIBPNG_SOURCE_DIR}/pngpread.c
        ${LIBPNG_SOURCE_DIR}/pngread.c
        ${LIBPNG_SOURCE_DIR}/pngrio.c
        ${LIBPNG_SOURCE_DIR}/pngrtran.c
        ${LIBPNG_SOURCE_DIR}/pngrutil.c
        ${LIBPNG_SOURCE_DIR}/pngset.c
        ${LIBPNG_SOURCE_DIR}/pngtrans.c
        ${LIBPNG_SOURCE_DIR}/pngwio.c
        ${LIBPNG_SOURCE_DIR}/pngwrite.c
        ${LIBPNG_SOURCE_DIR}/pngwtran.c
        ${LIBPNG_SOURCE_DIR}/pngwutil.c
    )

    configure_file(${LIBPNG_SOURCE_DIR}/scripts/pnglibconf.h.prebuilt ${LIBPNG_SOURCE_DIR}/pnglibconf.h)

    add_library(png STATIC ${LIBPNG_SOURCES})

    target_compile_definitions(png PRIVATE PNG_ARM_NEON_OPT=0)

    target_include_directories(png
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
        PRIVATE ${ZLIB_SOURCE_DIR}
        INTERFACE ${LIBPNG_SOURCE_DIR})

    set(LIBPNG_LIBRARY png)
    set(LIBPNG_LINKS "")
    set(MOROBOX8_LIB_LINKS ${MOROBOX8_LIB_LINKS} ${LIBPNG_LIBRARY} ${LIBPNG_LINKS})
    set(MOROBOX8_LIB_INCLUDE_DIRS ${MOROBOX8_LIB_INCLUDE_DIRS} ${LIBPNG_INCLUDE_DIR} ${LIBPNG_BINARY_DIR})
    #set(MOROBOX8_PLAYER_LINKS ${MOROBOX8_PLAYER_LINKS} ${LIBPNG_LIBRARY} ${LIBPNG_LINKS})
    #set(MOROBOX8_PLAYER_INCLUDE_DIRS ${MOROBOX8_PLAYER_INCLUDE_DIRS} ${LIBPNG_INCLUDE_DIR} ${LIBPNG_BINARY_DIR})

    set(ZLIB_LIBRARY zlib)
    set(ZLIB_LINKS "")
    set(MOROBOX8_LIB_LINKS ${MOROBOX8_LIB_LINKS} ${ZLIB_LIBRARY} ${ZLIB_LINKS})
    set(MOROBOX8_LIB_INCLUDE_DIRS ${MOROBOX8_LIB_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR} ${ZLIB_BINARY_DIR})
    #set(MOROBOX8_PLAYER_LINKS ${MOROBOX8_PLAYER_LINKS} ${ZLIB_LIBRARY} ${ZLIB_LINKS})
    #set(MOROBOX8_PLAYER_INCLUDE_DIRS ${MOROBOX8_PLAYER_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR} ${ZLIB_BINARY_DIR})

endif (MOROBOX8_FILESYSTEM)

# libwebsockets

if(MOROBOX8_WEBSOCKETS)

    set(LIBWEBSOCKETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/libwebsockets")
    set(LIBWEBSOCKETS_INCLUDE_DIR "${LIBWEBSOCKETS_SOURCE_DIR}/include")
    set(LIBWEBSOCKETS_BINARY_DIR "${LIBWEBSOCKETS_SOURCE_DIR}/build")

    ExternalProject_Add(libwebsockets
    PREFIX libwebsockets
    SOURCE_DIR ${LIBWEBSOCKETS_SOURCE_DIR}
    BINARY_DIR ${LIBWEBSOCKETS_BINARY_DIR}
    INSTALL_COMMAND ""
    CMAKE_ARGS
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DLWS_WITH_STATIC=ON
            -DLWS_WITH_SHARED=OFF
            -DLWS_WITHOUT_TESTAPPS=ON
            -DLWS_WITHOUT_TEST_SERVER=ON
            -DLWS_WITHOUT_TEST_SERVER_EXTPOLL=ON
            -DLWS_WITHOUT_TEST_PING=ON
            -DLWS_WITHOUT_TEST_CLIENT=ON
            -DLWS_WITH_MINIMAL_EXAMPLES=OFF
    )

    add_library(libwebsockets_vendor STATIC IMPORTED)

    if(MSVC)
    set(LIBWEBSOCKETS_LIBRARY "${LIBWEBSOCKETS_BINARY_DIR}/${CMAKE_CFG_INTDIR}/lib/libwebsockets.lib")
    else()
    set(LIBWEBSOCKETS_LIBRARY "${LIBWEBSOCKETS_BINARY_DIR}/lib/libwebsockets_static.a")
    endif()

    add_dependencies(libwebsockets_vendor libwebsockets)
    set(LIBWEBSOCKETS_LINKS libssl wsock32 ws2_32 ssl crypto crypt32)
    set(MOROBOX8_PLAYER_LINKS ${MOROBOX8_PLAYER_LINKS} ${LIBWEBSOCKETS_LIBRARY} ${LIBWEBSOCKETS_LINKS})
    set(MOROBOX8_SERVEREMU_LINKS ${MOROBOX8_SERVEREMU_LINKS} ${LIBWEBSOCKETS_LIBRARY} ${LIBWEBSOCKETS_LINKS})
    set(MOROBOX8_PLAYER_INCLUDE_DIRS ${MOROBOX8_PLAYER_INCLUDE_DIRS} ${LIBWEBSOCKETS_INCLUDE_DIR} ${LIBWEBSOCKETS_BINARY_DIR})
    set(MOROBOX8_SERVEREMU_INCLUDE_DIRS ${MOROBOX8_SERVEREMU_INCLUDE_DIRS} ${LIBWEBSOCKETS_INCLUDE_DIR} ${LIBWEBSOCKETS_BINARY_DIR})
    set(LIBWEBSOCKETS_ROOT_DIR "${LIBWEBSOCKETS_BINARY_DIR}")

endif(MOROBOX8_WEBSOCKETS)

# libws

if(MOROBOX8_WEBSOCKETS)

    set(LWS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/libws")
    set(LWS_INCLUDE_DIR "${LWS_SOURCE_DIR}")
    set(LWS_BINARY_DIR "${LWS_SOURCE_DIR}/build")

    ExternalProject_Add(libws
    PREFIX libws
    SOURCE_DIR ${LWS_SOURCE_DIR}
    BINARY_DIR ${LWS_BINARY_DIR}
    INSTALL_COMMAND ""
    CMAKE_ARGS
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DLIBWS_STATIC=ON
            -DLIBWS_SHARED=OFF
            -DLIBWS_UNIT_TESTING=OFF
            -DLIBWEBSOCKETS_ROOT_DIR=${LIBWEBSOCKETS_ROOT_DIR}
    )

    add_library(libws_vendor STATIC IMPORTED)

    if(MSVC)
    set(LWS_LIBRARY "${LWS_BINARY_DIR}/${CMAKE_CFG_INTDIR}/libws.lib")
    else()
    set(LWS_LIBRARY "${LWS_BINARY_DIR}/libws.a")
    endif()

    add_dependencies(libws_vendor libws)
    set(MOROBOX8_PLAYER_LINKS ${MOROBOX8_PLAYER_LINKS} ${LWS_LIBRARY})
    set(MOROBOX8_SERVEREMU_LINKS ${MOROBOX8_SERVEREMU_LINKS} ${LWS_LIBRARY})
    set(MOROBOX8_PLAYER_INCLUDE_DIRS ${MOROBOX8_PLAYER_INCLUDE_DIRS} ${LWS_INCLUDE_DIR} ${LWS_BINARY_DIR})
    set(MOROBOX8_SERVEREMU_INCLUDE_DIRS ${MOROBOX8_SERVEREMU_INCLUDE_DIRS} ${LWS_INCLUDE_DIR} ${LWS_BINARY_DIR})
    set(MOROBOX8_PLAYER_SOURCES ${MOROBOX8_PLAYER_SOURCES} ${MOROBOX8_SOURCE_DIR}/player/lws_session.c)

endif(MOROBOX8_WEBSOCKETS)

# libfs

if(MOROBOX8_FILESYSTEM)

    set(LIBFS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/libfs")
    set(LIBFS_INCLUDE_DIR "${LIBFS_SOURCE_DIR}")
    set(LIBFS_BINARY_DIR "${LIBFS_SOURCE_DIR}/build")

    ExternalProject_Add(libfs
    PREFIX libfs
    SOURCE_DIR ${LIBFS_SOURCE_DIR}
    BINARY_DIR ${LIBFS_BINARY_DIR}
    INSTALL_COMMAND ""
    CMAKE_ARGS
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DLIBFS_STATIC=ON
            -DLIBFS_SHARED=OFF
            -DLIBFS_UNIT_TESTING=OFF
            -DLIBFS_DOXYGEN=OFF
    )

    add_library(libfs_vendor STATIC IMPORTED)

    if(MSVC)
    set(LIBFS_LIBRARY "${LIBFS_BINARY_DIR}/${CMAKE_CFG_INTDIR}/libfs.lib")
    else()
    set(LIBFS_LIBRARY "${LIBFS_BINARY_DIR}/libfs.a")
    endif()

    add_dependencies(libfs_vendor libfs)

    message("LIBFS_BINARY_DIR=${LIBFS_BINARY_DIR}")
    message("LIBFS_LIBRARY=${LIBFS_LIBRARY}")
    set(MOROBOX8_LIB_LINKS ${MOROBOX8_LIB_LINKS} ${LIBFS_LIBRARY})
    set(MOROBOX8_PLAYER_LINKS ${MOROBOX8_PLAYER_LINKS} ${LIBFS_LIBRARY})
    set(MOROBOX8_LIB_INCLUDE_DIRS ${MOROBOX8_LIB_INCLUDE_DIRS} ${LIBFS_INCLUDE_DIR} ${LIBFS_BINARY_DIR})
    set(MOROBOX8_PLAYER_INCLUDE_DIRS ${MOROBOX8_PLAYER_INCLUDE_DIRS} ${LIBFS_INCLUDE_DIR} ${LIBFS_BINARY_DIR})
    set(MOROBOX8_PLAYER_SOURCES ${MOROBOX8_PLAYER_SOURCES} ${MOROBOX8_SOURCE_DIR}/player/fs_filesystem.c)
    set(MOROBOX8_LIB_COMPILE_FLAGS ${MOROBOX8_LIB_COMPILE_FLAGS} -DLIBFS_UNIT_TESTING=OFF -DLIBFS_STATIC=ON)
    list(APPEND MOROBOX8_PLAYER_COMPILE_FLAGS -DLIBFS_UNIT_TESTING=OFF -DLIBFS_STATIC=ON)

endif(MOROBOX8_FILESYSTEM)

# JS

# if (EMSCRIPTEN)

#    execute_process(
#        COMMAND cmd /C "npm i" 
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#        RESULT_VARIABLE CMD_ERROR
#        OUTPUT_FILE CMD_OUTPUT)

#    MESSAGE( STATUS "npm i:" ${CMD_ERROR})
#    MESSAGE( STATUS "npm i:" ${CMD_OUTPUT})

#    execute_process(
#        COMMAND cmd /C "npm run build" 
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
 #       RESULT_VARIABLE CMD_ERROR
  #      OUTPUT_FILE CMD_OUTPUT)

#    MESSAGE( STATUS "npm run build:" ${CMD_ERROR})
#    MESSAGE( STATUS "npm run build:" ${CMD_OUTPUT})

#endif (EMSCRIPTEN)

# morobox8
set(MOROBOX8_LIB morobox8)

file(GLOB MOROBOX8_LIB_HEADERS
    ${MOROBOX8_INCLUDE_DIR}/*.h
    ${MOROBOX8_SOURCE_DIR}/*.h)
message("MOROBOX8_LIB_HEADERS=${MOROBOX8_LIB_HEADERS}")
set(MOROBOX8_LIB_SOURCES
    ${MOROBOX8_SOURCE_DIR}/api/lua_api.c
    ${MOROBOX8_SOURCE_DIR}/api/js_api.c
    ${MOROBOX8_SOURCE_DIR}/cart/cart.c
    ${MOROBOX8_SOURCE_DIR}/filesystem/filesystem_hooks.c
    ${MOROBOX8_SOURCE_DIR}/filesystem/filesystem.c
    ${MOROBOX8_SOURCE_DIR}/network/packet.c
    ${MOROBOX8_SOURCE_DIR}/network/session_hooks.c
    ${MOROBOX8_SOURCE_DIR}/tool/pack.c
    ${MOROBOX8_SOURCE_DIR}/morobox8_hooks.c
    ${MOROBOX8_SOURCE_DIR}/morobox8.c
)

if (MOROBOX8_STATIC)
    
    set(MOROBOX8_STATIC_LIB "${MOROBOX8_LIB}-static")

    if (EMSCRIPTEN)
        add_executable(${MOROBOX8_STATIC_LIB} ${MOROBOX8_LIB_SOURCES})
    else()
        add_library(${MOROBOX8_STATIC_LIB} STATIC "${MOROBOX8_LIB_SOURCES}")
    endif(EMSCRIPTEN)
    
    set_target_properties(${MOROBOX8_STATIC_LIB} PROPERTIES
            OUTPUT_NAME "${MOROBOX8_LIB}"
            PREFIX "")

    if (EMSCRIPTEN)

        set_target_properties(${MOROBOX8_STATIC_LIB}
            PROPERTIES LINK_FLAGS
                "-s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=\"morobox8\" -s NO_FILESYSTEM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS=['_malloc','_free'] -s EXPORTED_RUNTIME_METHODS=ccall --post-js ../dist/morobox8.js")
 
    endif (EMSCRIPTEN)

    target_link_libraries(${MOROBOX8_STATIC_LIB} PRIVATE ${MOROBOX8_LIB_LINKS})

    target_compile_options(${MOROBOX8_STATIC_LIB}
                           PRIVATE
                               ${MOROBOX8_LIB_COMPILE_FLAGS}
                               -DHAVE_CONFIG_H
                               -DMOROBOX8_HIDE_SYMBOLS
                               -g)
                               
    target_include_directories(${MOROBOX8_STATIC_LIB}
                               PRIVATE
                                   ${CMAKE_CURRENT_BINARY_DIR}
                                   ${MOROBOX8_LIB_INCLUDE_DIRS}
                                   ${MOROBOX8_SOURCE_DIR}
                               PUBLIC
                                    ${CMAKE_CURRENT_SOURCE_DIR})

    set(MOROBOX8_PLAYER_LINKS ${MOROBOX8_PLAYER_LINKS} ${MOROBOX8_STATIC_LIB})


endif(MOROBOX8_STATIC)

# player

if(MOROBOX8_PLAYER)
    
    set(MOROBOX8_PLAYER_BIN "morobox8-player")

    add_executable(${MOROBOX8_PLAYER_BIN}
        ${MOROBOX8_PLAYER_SOURCES}
        ${MOROBOX8_SOURCE_DIR}/player/main.c)
            
    target_compile_options(${MOROBOX8_PLAYER_BIN}
                           PRIVATE
                               ${MOROBOX8_PLAYER_COMPILE_FLAGS}
                               -DHAVE_CONFIG_H
                               -g)

    target_link_libraries(${MOROBOX8_PLAYER_BIN} PRIVATE ${MOROBOX8_PLAYER_LINKS} libssl wsock32 ws2_32 ssl crypto crypt32)

    target_include_directories(${MOROBOX8_PLAYER_BIN}
        SYSTEM PRIVATE 
            ${MOROBOX8_PLAYER_INCLUDE_DIRS})

    add_dependencies(${MOROBOX8_PLAYER_BIN} ${MOROBOX8_STATIC_LIB})

    set_target_properties(${MOROBOX8_PLAYER_BIN} PROPERTIES
            RUNTIME_OUTPUT_NAME morobox8)

endif(MOROBOX8_PLAYER)

# serveremu

if(MOROBOX8_SERVEREMU)
    
    set(MOROBOX8_SERVEREMU_BIN "morobox8-serveremu")

    add_executable(${MOROBOX8_SERVEREMU_BIN}
        ${MOROBOX8_SOURCE_DIR}/network/packet.c
        ${MOROBOX8_SOURCE_DIR}/network/relayserver.c
        ${MOROBOX8_SOURCE_DIR}/network/masterserver.c
        ${MOROBOX8_SOURCE_DIR}/network/serveremu.c)
            
    target_compile_options(${MOROBOX8_SERVEREMU_BIN}
                           PRIVATE
                               ${MOROBOX8_SERVEREMU_COMPILE_FLAGS}
                               -DHAVE_CONFIG_H)

    target_link_libraries(${MOROBOX8_SERVEREMU_BIN} PRIVATE ${MOROBOX8_SERVEREMU_LINKS})

    target_include_directories(${MOROBOX8_SERVEREMU_BIN}
        PRIVATE 
            ${MOROBOX8_SERVEREMU_INCLUDE_DIRS})

    set_target_properties(${MOROBOX8_SERVEREMU_BIN} PROPERTIES
            RUNTIME_OUTPUT_NAME serveremu)

endif(MOROBOX8_SERVEREMU)

# include cmocka
if (MOROBOX8_UNIT_TESTING)
    if (NOT MOROBOX8_STATIC)
        message("Skip unit testing because MOROBOX8_STATIC option is off")

    else()
        include(AddCMockaTest)
        add_subdirectory(tests)

    endif(NOT MOROBOX8_STATIC)
endif (MOROBOX8_UNIT_TESTING)

if (MOROBOX8_DOXYGEN)
    add_subdirectory (docs)
endif (MOROBOX8_DOXYGEN)