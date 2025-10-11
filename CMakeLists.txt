cmake_minimum_required(VERSION 3.16)

set(LIB_NAME "Param")
string(TOUPPER "${LIB_NAME}" LIB_NAME_UPPER)

project(${LIB_NAME} VERSION 0.1.2 LANGUAGES C)

# ============================================================
# === Platform Detection and Build Options ===
# ============================================================
if(PROJECT_IS_TOP_LEVEL)
    message(STATUS "Running as top-level project: Enabling platform detection")
    set(ENABLE_PLATFORM_DETECTION ON)
else()
    message(STATUS "Running as a subproject: Skipping platform detection")
    set(ENABLE_PLATFORM_DETECTION OFF)
endif()

option(${LIB_NAME_UPPER}_BUILD_STATIC_LIB "Build static library" OFF)
option(${LIB_NAME_UPPER}_BUILD_SHARED_LIB "Build shared library" OFF)
option(${LIB_NAME_UPPER}_BUILD_EXAMPLES "Build examples" OFF)

if(ENABLE_PLATFORM_DETECTION AND NOT MSVC)
    option(TARGET_ARCH "Target architecture" "none")
    option(USE_CUSTOM_COMPILER "Enable custom cross-compiler" OFF)
    if (USE_CUSTOM_COMPILER)
        set(CMAKE_C_COMPILER "<path-to-cross-compiler>")
        set(CMAKE_SYSTEM_NAME "<target-system-name>")
        message(STATUS "Using custom cross-compiler: ${CMAKE_C_COMPILER}")
    endif()

    if (TARGET_ARCH STREQUAL "arm")
        set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
        set(ARM ON)
        message(STATUS "Target: ARM")
    elseif (TARGET_ARCH STREQUAL "riscv")
        set(CMAKE_C_COMPILER "riscv-none-embed-gcc")
        set(RISCV ON)
        message(STATUS "Target: RISC-V")
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(WINDOWS ON)
        message(STATUS "Target: Windows")
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(LINUX ON)
        message(STATUS "Target: Linux")
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Generic")
        set(BARE_METAL ON)
        message(STATUS "Target: Bare-metal")
    else()
        message(STATUS "Target: Unknown platform")
    endif()
endif()

# ============================================================
# === Source and Header Directories ===
# ============================================================
set(LIBRARY_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Src)
set(EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Examples)
set(EXAMPLES_OUTPUT_DIR ${CMAKE_BINARY_DIR}/Examples)

file(GLOB_RECURSE LIBRARY_SOURCES ${LIBRARY_SRC_DIR}/*.c)
file(GLOB_RECURSE LIBRARY_HEADERS ${LIBRARY_SRC_DIR}/*.h)

# ============================================================
# === Library Type Decision ===
# ============================================================
if(${LIB_NAME_UPPER}_BUILD_STATIC_LIB)
    set(BUILD_STATIC_LIB_NUM 1)
else()
    set(BUILD_STATIC_LIB_NUM 0)
endif()

if(${LIB_NAME_UPPER}_BUILD_SHARED_LIB)
    set(BUILD_SHARED_LIB_NUM 1)
else()
    set(BUILD_SHARED_LIB_NUM 0)
endif()

math(EXPR LIB_COUNT "${BUILD_STATIC_LIB_NUM} + ${BUILD_SHARED_LIB_NUM}")

if(LIB_COUNT EQUAL 0)
    message(STATUS "No build type selected — defaulting to STATIC library")
    set(${LIB_NAME_UPPER}_BUILD_STATIC_LIB ON)
    set(BUILD_STATIC_LIB_NUM 1)
    set(LIB_COUNT 1)
endif()

# ============================================================
# === Dependency: Str library (local or installed) ===
# ============================================================
set(STR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Str" CACHE PATH "Path to Str library")

if(NOT TARGET Str::Static AND NOT TARGET Str::Shared)
    if(EXISTS "${STR_DIR}/CMakeLists.txt")
        # Use unique binary directory per dependency layer
        get_filename_component(_abs_str_dir "${STR_DIR}" ABSOLUTE)
        string(MD5 STR_HASH "${_abs_str_dir}")
        set(_str_binary_dir "${CMAKE_BINARY_DIR}/Str_${STR_HASH}")
        message(STATUS "Including local Str source at: ${STR_DIR}")
        add_subdirectory(${STR_DIR} ${_str_binary_dir})
    else()
        message(STATUS "Using installed Str library (find_package)")
        find_package(Str CONFIG REQUIRED)
    endif()
else()
    message(STATUS "Str already added — skipping re-addition")
endif()

if(TARGET Str::Shared)
    set(STR_LIB Str::Shared)
elseif(TARGET Str::Static)
    set(STR_LIB Str::Static)
else()
    message(FATAL_ERROR "No Str library target found! Make sure Str is available.")
endif()

# ============================================================
# === Create Library Targets ===
# ============================================================
if(${LIB_NAME_UPPER}_BUILD_SHARED_LIB)
    if(LIB_COUNT EQUAL 1)
        add_library(${LIB_NAME} SHARED ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
        set(SHARED_TARGET ${LIB_NAME})
    else()
        add_library(${LIB_NAME}-Shared SHARED ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
        set(SHARED_TARGET ${LIB_NAME}-Shared)
    endif()
    add_library(${LIB_NAME}::Shared ALIAS ${SHARED_TARGET})

    target_include_directories(${SHARED_TARGET} PUBLIC
        $<BUILD_INTERFACE:${LIBRARY_SRC_DIR}>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(${SHARED_TARGET} PUBLIC c_std_99)
    target_compile_definitions(${SHARED_TARGET} PRIVATE ${LIB_NAME_UPPER}_EXPORTS)
    target_link_libraries(${SHARED_TARGET} PUBLIC ${STR_LIB})

    set_target_properties(${SHARED_TARGET} PROPERTIES
        PUBLIC_HEADER "${LIBRARY_HEADERS}"
    )

    install(TARGETS ${SHARED_TARGET}
        PUBLIC_HEADER DESTINATION include
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )

    if(WIN32)
        set_target_properties(${SHARED_TARGET} PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS TRUE
            C_VISIBILITY_PRESET hidden
        )
    endif()
endif()

if(${LIB_NAME_UPPER}_BUILD_STATIC_LIB)
    if(LIB_COUNT EQUAL 1)
        add_library(${LIB_NAME} STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
        set(STATIC_TARGET ${LIB_NAME})
    else()
        add_library(${LIB_NAME}-Static STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
        set(STATIC_TARGET ${LIB_NAME}-Static)
    endif()
    add_library(${LIB_NAME}::Static ALIAS ${STATIC_TARGET})

    target_include_directories(${STATIC_TARGET} PUBLIC
        $<BUILD_INTERFACE:${LIBRARY_SRC_DIR}>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(${STATIC_TARGET} PUBLIC c_std_99)
    target_link_libraries(${STATIC_TARGET} PUBLIC ${STR_LIB})

    set_target_properties(${STATIC_TARGET} PROPERTIES
        PUBLIC_HEADER "${LIBRARY_HEADERS}"
    )

    install(TARGETS ${STATIC_TARGET}
        PUBLIC_HEADER DESTINATION include
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

# ============================================================
# === Examples ===
# ============================================================
if(${LIB_NAME_UPPER}_BUILD_EXAMPLES)
    file(MAKE_DIRECTORY ${EXAMPLES_OUTPUT_DIR})
    set(EXAMPLE_NAMES
        ${LIB_NAME}-Test
    )

    foreach(EXAMPLE_NAME ${EXAMPLE_NAMES})
        set(EXAMPLE_DIR ${EXAMPLES_DIR}/${EXAMPLE_NAME})
        file(GLOB EXAMPLE_SOURCES ${EXAMPLE_DIR}/*.c)

        if(EXAMPLE_SOURCES)
            add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCES})
            target_include_directories(${EXAMPLE_NAME} PRIVATE ${LIBRARY_SRC_DIR})

            if(TARGET ${STATIC_TARGET})
                target_link_libraries(${EXAMPLE_NAME} PRIVATE ${STATIC_TARGET})
            elseif(TARGET ${SHARED_TARGET})
                target_link_libraries(${EXAMPLE_NAME} PRIVATE ${SHARED_TARGET})
            endif()

            target_link_libraries(${EXAMPLE_NAME} PRIVATE ${STR_LIB})

            set_target_properties(${EXAMPLE_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_OUTPUT_DIR})

            if(CMAKE_BUILD_TYPE STREQUAL "Release" AND CMAKE_STRIP)
                add_custom_command(TARGET ${EXAMPLE_NAME}
                    POST_BUILD
                    COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${EXAMPLE_NAME}>
                    COMMENT "Stripping symbols from ${EXAMPLE_NAME}")
            endif()

            message(STATUS "Added example: ${EXAMPLE_NAME}")
        else()
            message(WARNING "No source files found for example: ${EXAMPLE_NAME}")
        endif()
    endforeach()
endif()

# ============================================================
# === Installation and Export ===
# ============================================================
install(DIRECTORY ${LIBRARY_SRC_DIR}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h")

install(
    TARGETS ${STATIC_TARGET} ${SHARED_TARGET}
    EXPORT ${LIB_NAME}Targets
    PUBLIC_HEADER DESTINATION include
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Generate config files
include(CMakePackageConfigHelpers)
set(CONFIG_IN_CONTENT [=[
@PACKAGE_INIT@

include("${CMAKE_CURRENT_LIST_DIR}/${LIB_NAME}Targets.cmake")
check_required_components(${LIB_NAME})
]=])
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake.in" "${CONFIG_IN_CONTENT}")

configure_package_config_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake
    INSTALL_DESTINATION lib/cmake/${LIB_NAME}
)

install(
    EXPORT ${LIB_NAME}Targets
    FILE ${LIB_NAME}Targets.cmake
    NAMESPACE ${LIB_NAME}::
    DESTINATION lib/cmake/${LIB_NAME}
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake
    DESTINATION lib/cmake/${LIB_NAME}
)
