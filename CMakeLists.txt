cmake_minimum_required (VERSION 3.16)
project (lpp C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 99)
set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})

# Set default config for Ninja Multi-Config. Possible values: Debug;Release;RelWithDebInfo
if (CMAKE_GENERATOR MATCHES "Ninja Multi-Config")
    if(NOT CMAKE_DEFAULT_BUILD_TYPE)
        set(CMAKE_DEFAULT_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    endif()
endif()

include(CTest)
include(FindLua)

if(LUA_FOUND)
    set(LUA_VERSION_SHORT "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Werror -march=native -fdiagnostics-color=always")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -Werror -march=native -fdiagnostics-color=always")

find_program(CCACHE_EXECUTABLE ccache)
if(CCACHE_EXECUTABLE)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_EXECUTABLE})
endif(CCACHE_EXECUTABLE)

file(GLOB LPP_SOURCES      src/*.cpp src/*.h)

add_executable(
    lpp
    ${LPP_SOURCES}
)
target_include_directories(lpp PUBLIC inc)

if(LUA_FOUND AND (${LUA_VERSION_SHORT} MATCHES "5.3"))
    # ${LUA_LIBRARIES} does not work on Ubuntu
    target_link_libraries(lpp lua5.3)
endif()

if(LUA_FOUND AND (${LUA_VERSION_SHORT} MATCHES "5.3"))
    target_include_directories(lpp PUBLIC ${LUA_INCLUDE_DIR})
endif()
