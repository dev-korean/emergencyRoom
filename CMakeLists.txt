cmake_minimum_required(VERSION 3.20)

project(DataStructureProject C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------------------------------------------------------------------
# 빌드 타입 (기본 Debug)
# --------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# --------------------------------------------------------------------------
# raylib 경로 (로컬 설치)
# --------------------------------------------------------------------------
set(RAYLIB_ROOT "C:/Users/twent/scoop/apps/raylib-mingw/6.0")

# --------------------------------------------------------------------------
# 인클루드 경로
#   include/          → 프로젝트 헤더 (patient.h 등)
#   external/         → raygui.h 등 서드파티 헤더온리
#   src/              → ui/, data/ 내부 헤더 상호참조용
# --------------------------------------------------------------------------
include_directories(
    include
    external
    src
    ${RAYLIB_ROOT}/include
)

link_directories(${RAYLIB_ROOT}/lib)

# --------------------------------------------------------------------------
# 소스 수집
#   src/*.c           → main.c
#   src/ui/*.c        → topbar, sidebar, main_view, right_panel
#   src/data/*.c      → queue, stack, bst, linked_list ...
# --------------------------------------------------------------------------
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "src/*.c"
    "src/ui/*.c"
    "src/data/*.c"
)

# --------------------------------------------------------------------------
# 실행 파일
# --------------------------------------------------------------------------
add_executable(app ${SOURCES})

# --------------------------------------------------------------------------
# 링크
# --------------------------------------------------------------------------
target_link_libraries(app PRIVATE raylib)

if(MINGW)
    target_link_libraries(app PRIVATE
        -lopengl32
        -lgdi32
        -lwinmm
        -lws2_32
    )
endif()

# --------------------------------------------------------------------------
# 컴파일 옵션
#   -Wpedantic 제거  → raygui 내부에서 pedantic 경고 대량 발생
#   Debug 빌드에서만 -g -O0 / Release는 -O2
# --------------------------------------------------------------------------
target_compile_options(app PRIVATE
    -Wall
    -Wextra
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O2>
)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(app PRIVATE
        -finput-charset=UTF-8
        -fexec-charset=UTF-8
    )
endif()

# --------------------------------------------------------------------------
# raygui 헤더온리 구현 활성화
#   raygui.h를 딱 한 파일(main.c 권장)에서만
#   #define RAYGUI_IMPLEMENTATION 하면 되는데,
#   CMake 레벨에서 한 번만 정의되게 강제할 수도 있어
#   (직접 소스에서 define 하는 걸 선호하면 이 줄 제거)
# --------------------------------------------------------------------------
# target_compile_definitions(app PRIVATE RAYGUI_IMPLEMENTATION)

# --------------------------------------------------------------------------
# 빌드 결과물을 프로젝트 루트/bin 으로 출력
# --------------------------------------------------------------------------
set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_SOURCE_DIR}/bin/assets
)
