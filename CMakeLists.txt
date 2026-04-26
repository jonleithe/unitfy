cmake_minimum_required(VERSION 3.20)

project(delta VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

configure_file(
    ${CMAKE_SOURCE_DIR}/inc/delta_version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/delta_version.h
    @ONLY
)

add_executable(delta
    main.cpp
    src/delta.cpp
    src/format.cpp
    src/length.cpp
    src/pressure.cpp
    src/temperature.cpp
    src/volume.cpp
)

target_include_directories(delta
    PRIVATE
        inc
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

find_library(READLINE_LIBRARY readline)
if(READLINE_LIBRARY)
    message(STATUS "Found readline: ${READLINE_LIBRARY}")
    target_link_libraries(delta PRIVATE ${READLINE_LIBRARY})
    target_compile_definitions(delta PRIVATE HAVE_READLINE=1)
else()
    message(STATUS "readline not found; REPL will run without line history")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(delta PRIVATE -Wall -Wextra -Wpedantic -Wswitch-default)
endif()

set_target_properties(delta PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

include(CTest)
if(BUILD_TESTING)
    add_executable(delta_smoke_test
        tests/smoke_test.cpp
        src/delta.cpp
        src/format.cpp
        src/length.cpp
        src/pressure.cpp
        src/temperature.cpp
        src/volume.cpp
    )

    target_include_directories(delta_smoke_test
        PRIVATE
            inc
            ${CMAKE_CURRENT_BINARY_DIR}/generated
    )

    if(READLINE_LIBRARY)
        target_link_libraries(delta_smoke_test PRIVATE ${READLINE_LIBRARY})
        target_compile_definitions(delta_smoke_test PRIVATE HAVE_READLINE=1)
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(delta_smoke_test PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    set_target_properties(delta_smoke_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    )

    add_test(NAME delta_smoke COMMAND $<TARGET_FILE:delta_smoke_test>)
endif()
