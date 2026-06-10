#==============================================================================
# drmon — Linux port, Phase 1: compile + link (SYSTEM=SNES).
# Build inside the throwaway toolchain image (linux/Dockerfile); nothing on host.
# See docs/plans/2026-06-10-port-drmon-linux.md.
#==============================================================================
cmake_minimum_required(VERSION 3.16)
project(drmon LANGUAGES CXX C)

# Object list mirrors drmon.mak's OBJS, minus the DOS x86 asm transport
# (snesio.asm) which is replaced by linux/slio_stub.cpp.
set(DRMON_CXX_SRC
    symbol.cpp board.cpp listrect.cpp menu.cpp object.cpp layer.cpp
    app.cpp info.cpp drmon.cpp display.cpp screen.cpp list.cpp memory.cpp
    reg.cpp mouse.cpp window.cpp gadget.cpp input.cpp macro.cpp manager.cpp
    menubar.cpp message.cpp memops.cpp break.cpp expr.cpp control.cpp shell.cpp
    textview.cpp console.cpp help.cpp command.cpp error.cpp cmdmacro.cpp
    charts.cpp source.cpp which.cpp watch.cpp mem.cpp sld.cpp coff.cpp
    zardoz.cpp monmenu.cpp about.cpp general.cpp config.cpp filereq.cpp
    profile.cpp filename.cpp
    # Linux port shims:
    linux/dos_stubs.cpp linux/slio_stub.cpp linux/ncurses_io.cpp
)
set(DRMON_C_SRC lexyy.c ytab.c)

add_executable(drmon ${DRMON_CXX_SRC} ${DRMON_C_SRC})

find_package(Curses REQUIRED)
target_link_libraries(drmon PRIVATE ${CURSES_LIBRARIES})
target_include_directories(drmon PRIVATE ${CURSES_INCLUDE_DIRS})

target_include_directories(drmon PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/linux/include    # DOS/Borland header shims (first)
    ${CMAKE_CURRENT_SOURCE_DIR}                   # drmon's own headers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs     # in-repo pclib (<pclib/*.hpp>)
)

target_compile_definitions(drmon PRIVATE SNES SYSTEMSNES DEBUGDR)

# Force-include the compat header into every TU (neutralizes far/near/cdecl,
# maps far-heap/mem ops). Lenient dialect for 1994 pre-ISO C++.
target_compile_options(drmon PRIVATE
    -include ${CMAKE_CURRENT_SOURCE_DIR}/linux/linux_compat.hpp
    -w
    -fcommon                                     # 1990s code relies on common-symbol merging
    $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++98>
    $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>
    $<$<COMPILE_LANGUAGE:C>:-std=gnu89>
)
