cmake_minimum_required(VERSION 3.22.1)
project(tqrcodegen VERSION 1.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
option(BUILD_SHARED_LIBS "Build a loadable Tcl extension" ON)
find_package(TclStub 8.6 REQUIRED)

add_library(tqrcodegen library.c qrcodegen.c)
set_target_properties(tqrcodegen PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(tqrcodegen PRIVATE ${TCL_INCLUDE_PATH})
target_compile_definitions(tqrcodegen PRIVATE TCL_THREADS PROJECT_VERSION=${PROJECT_VERSION})
if(BUILD_SHARED_LIBS)
    target_compile_definitions(tqrcodegen PRIVATE USE_TCL_STUBS)
    target_link_libraries(tqrcodegen PRIVATE ${TCL_STUB_LIBRARY})
    configure_file(pkgIndex.tcl.in pkgIndex.tcl @ONLY)
else()
    target_compile_definitions(tqrcodegen PRIVATE STATIC_BUILD)
endif()
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(tqrcodegen PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS tqrcodegen
    LIBRARY DESTINATION lib/tqrcodegen${PROJECT_VERSION}
    ARCHIVE DESTINATION lib/tqrcodegen${PROJECT_VERSION}
    RUNTIME DESTINATION lib/tqrcodegen${PROJECT_VERSION})
install(FILES library.h DESTINATION include/tqrcodegen)
if(BUILD_SHARED_LIBS)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pkgIndex.tcl
        DESTINATION lib/tqrcodegen${PROJECT_VERSION})
endif()

include(CTest)
if(BUILD_TESTING)
    if(BUILD_SHARED_LIBS)
        if(NOT TCL_TCLSH)
            message(FATAL_ERROR "Set TCL_TCLSH to run tests, or use BUILD_TESTING=OFF")
        endif()
        add_test(NAME TclTests COMMAND ${TCL_TCLSH}
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/all.tcl $<TARGET_FILE:tqrcodegen>)
    else()
        find_library(TCL_LIBRARY
            NAMES tcl${TCL_MAJOR_MINOR} tcl${TCL_COMPACT_VERSION} tcl
            HINTS ${TCL_LIBRARY_DIR} "${TCL_PREFIX}/lib" REQUIRED)
        add_executable(static-smoke tests/static.c)
        target_compile_definitions(static-smoke PRIVATE STATIC_BUILD)
        target_include_directories(static-smoke PRIVATE ${TCL_INCLUDE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
        target_link_libraries(static-smoke PRIVATE tqrcodegen ${TCL_LIBRARY})
        add_test(NAME StaticEmbedding COMMAND static-smoke)
    endif()
endif()
