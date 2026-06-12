#==============================================================================
# drmon — Linux port, Phase 2: two targets (snesmon + genmon) with MAME bridge.
# Phase 1: compile + link (SYSTEM=SNES) — see docs/plans/2026-06-10-port-drmon-linux.md.
# Phase 2: MAME TCP bridge backend — see docs/plans/2026-06-11-drmon-mame-backend.md.
# Build inside the throwaway toolchain image (linux/Dockerfile); nothing on host.
#==============================================================================
cmake_minimum_required(VERSION 3.16)
project(drmon LANGUAGES CXX C)

# AddressSanitizer (ON by default; turn off with -DDRMON_ASAN=OFF once stable).
option(DRMON_ASAN "Build with AddressSanitizer" ON)

# Phase 2 MAME bridge backend (ON by default; OFF falls back to slio_stub.cpp).
option(DRMON_MAME_BACKEND "Build with MAME TCP bridge backend" ON)

# Shared source list (mirrors drmon.mak OBJS, minus the DOS asm transport).
set(DRMON_CXX_SRC
    symbol.cpp board.cpp listrect.cpp menu.cpp object.cpp layer.cpp
    app.cpp info.cpp drmon.cpp display.cpp screen.cpp list.cpp memory.cpp
    reg.cpp spc_reg.cpp mouse.cpp window.cpp gadget.cpp input.cpp macro.cpp manager.cpp
    menubar.cpp message.cpp memops.cpp break.cpp expr.cpp control.cpp shell.cpp
    textview.cpp console.cpp help.cpp command.cpp error.cpp cmdmacro.cpp
    charts.cpp source.cpp which.cpp watch.cpp mem.cpp sld.cpp coff.cpp
    zardoz.cpp monmenu.cpp about.cpp general.cpp config.cpp filereq.cpp
    profile.cpp filename.cpp search.cpp
    # Linux port shims:
    linux/dos_stubs.cpp linux/ncurses_io.cpp
)
set(DRMON_C_SRC lexyy.c ytab.c)

# MAME backend replaces slio_stub.cpp; slio_stub.cpp stays for non-MAME builds.
if(DRMON_MAME_BACKEND)
    # sliomame.cpp is included via which.cpp (#include "linux/sliomame.cpp")
    # slio_stub.cpp is NOT added — sliomame.cpp provides all the same residual symbols.
else()
    list(APPEND DRMON_CXX_SRC linux/slio_stub.cpp)
endif()

set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)

# Shared compile options for all targets.
set(DRMON_COMPILE_OPTS
    -include ${CMAKE_CURRENT_SOURCE_DIR}/linux/linux_compat.hpp
    -w
    -fcommon
    $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++98>
    $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>
    $<$<COMPILE_LANGUAGE:C>:-std=gnu89>
)
if(DRMON_ASAN)
    list(APPEND DRMON_COMPILE_OPTS -fsanitize=address -fno-omit-frame-pointer -g)
endif()

# Helper macro to create a drmon target.
function(drmon_target TARGET SYSTEM_FLAG EXTRA_DEFS)
    add_executable(${TARGET} ${DRMON_CXX_SRC} ${DRMON_C_SRC})
    target_link_libraries(${TARGET} PRIVATE ${CURSES_LIBRARIES})
    target_include_directories(${TARGET} PRIVATE
        ${CURSES_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/linux/include
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs
    )
    set(DEFS ${SYSTEM_FLAG} DEBUGDR)
    if(DRMON_MAME_BACKEND)
        list(APPEND DEFS MAMEBACKEND)
    endif()
    list(APPEND DEFS ${EXTRA_DEFS})
    target_compile_definitions(${TARGET} PRIVATE ${DEFS})
    target_compile_options(${TARGET} PRIVATE ${DRMON_COMPILE_OPTS})
    if(DRMON_ASAN)
        target_link_options(${TARGET} PRIVATE -fsanitize=address)
    endif()
endfunction()

# snesmon: SNES / 65816 (was the only target in Phase 1, previously named drmon)
drmon_target(snesmon "SNES;SYSTEMSNES" "")

# genmon: Genesis / 68000
drmon_target(genmon "GENESIS;SYSTEMGEN" "")

#==============================================================================
# Phase 3: drmon-dap — DAP front end (cppdap, C++17, no TUI dependency)
#==============================================================================

include(FetchContent)
FetchContent_Declare(cppdap
    GIT_REPOSITORY https://github.com/google/cppdap.git
    GIT_TAG        dap-1.58.0-a
)
# cppdap's own CMakeLists may try to build tests/examples; suppress them.
set(CPPDAP_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
set(CPPDAP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# Use the system JsonCpp instead of cppdap's bundled submodule. FetchContent
# does not fetch cppdap's third_party/json submodule, so the bundled JSON
# backend is empty and the Json::Value symbols are undefined at link time.
# CPPDAP_USE_EXTERNAL_JSONCPP_PACKAGE makes cppdap find_package(jsoncpp);
# build hosts need libjsoncpp-dev (Ubuntu) / jsoncpp (vcpkg/brew) installed.
set(CPPDAP_USE_EXTERNAL_JSONCPP_PACKAGE ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(cppdap)
find_package(jsoncpp REQUIRED)

find_package(Threads REQUIRED)

# Common DAP sources (shared by both SNES and GEN targets)
set(DRMON_DAP_SRC_COMMON
    linux/dap/main.cpp
    linux/dap/session.cpp
    linux/dap/backend.cpp
    linux/dap/breakpoints.cpp
    linux/dap/disasm_stubs.cpp
    linux/dap/symbols.cpp
)

function(drmon_dap_target TARGET SYSTEM_DEFINE DISASM_WRAPPER)
    add_executable(${TARGET} ${DRMON_DAP_SRC_COMMON} ${DISASM_WRAPPER})
    # Legacy disassembler source: suppress modern-warning noise, allow C-style casts.
    set_source_files_properties(${DISASM_WRAPPER}
        PROPERTIES COMPILE_OPTIONS "-w;-fpermissive")
    target_link_libraries(${TARGET} PRIVATE cppdap jsoncpp_lib Threads::Threads)
    target_compile_definitions(${TARGET} PRIVATE ${SYSTEM_DEFINE})
    target_compile_options(${TARGET} PRIVATE
        -include ${CMAKE_CURRENT_SOURCE_DIR}/linux/linux_compat.hpp
        -std=c++17 -Wall -Wextra)
    target_include_directories(${TARGET} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/linux/dap
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/linux/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs
    )
endfunction()

drmon_dap_target(drmon-dap-snes DRMON_DAP_SNES linux/dap/dis816_dap.cpp)
drmon_dap_target(drmon-dap-gen  DRMON_DAP_GEN  linux/dap/dis68000_dap.cpp)
