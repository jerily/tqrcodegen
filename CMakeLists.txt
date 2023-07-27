cmake_minimum_required(VERSION 3.22.1)
project(tqrcodegen LANGUAGES CXX C)

set(TARGET tqrcodegen)
set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_C_STANDARD_REQUIRED true)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#set(CMAKE_VERBOSE_MAKEFILE ON)

add_library(tqrcodegen SHARED library.cc qrcodegen.cpp)
set_target_properties(tqrcodegen PROPERTIES POSITION_INDEPENDENT_CODE ON)

include(${CMAKE_CURRENT_LIST_DIR}/FindTCL.cmake)

if (NAVISERVER)
    include_directories(${NAVISERVER}/include)
    target_link_directories(tqrcodegen PRIVATE ${NAVISERVER}/lib)
    target_link_libraries(tqrcodegen PRIVATE ${TCL_LIBRARY} nsd nsthread)
    target_compile_options(tqrcodegen PRIVATE -DUSE_NAVISERVER)
    install(TARGETS ${TARGET}
            LIBRARY DESTINATION ${NAVISERVER}/lib
    )
else ()
    include_directories(${TCL_INCLUDE_PATH})
    get_filename_component(TCL_LIBRARY_PATH "${TCL_LIBRARY}" PATH)
    if (TCL_LIBRARY_PATH)
        install(TARGETS ${TARGET}
            LIBRARY DESTINATION ${TCL_LIBRARY_PATH}
        )
    endif()
endif()

#add_compile_options(-g -O0 -DDEBUG -Wall -Wextra -Wconversion)
