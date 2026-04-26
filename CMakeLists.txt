cmake_minimum_required(VERSION 3.20)

project(unitfy VERSION 1.0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(UNITFY_PKGREL "" CACHE STRING "Optional package release for runtime version output")
set(PKGREL_SUFFIX "")
if(UNITFY_PKGREL)
    string(REGEX REPLACE "^-+" "" UNITFY_PKGREL_CLEAN "${UNITFY_PKGREL}")
    if(NOT UNITFY_PKGREL_CLEAN STREQUAL "")
        set(PKGREL_SUFFIX "-${UNITFY_PKGREL_CLEAN}")
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(VERSION_SUFFIX "${PKGREL_SUFFIX}-debug")
else()
    set(VERSION_SUFFIX "${PKGREL_SUFFIX}")
endif()

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
    OUTPUT_NAME "unitfy"
)

include(GNUInstallDirs)

install(TARGETS delta
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


install(FILES LICENSE
    DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/unitfy
)

install(FILES README.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/unitfy
)

install(FILES docs/unitfy.1
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
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
