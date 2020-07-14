cmake_minimum_required(VERSION 3.2.0)
project(big_money)

add_subdirectory(3rd)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC)

add_executable(big_money ${SRC})
target_compile_options(big_money PRIVATE -std=c++11)

if (WIN32 OR MSVC)
    message(STATUS "Build big_money on windows system")
    add_dependencies(big_money libcurl)
    target_include_directories(big_money PUBLIC ${CMAKE_CURRENT_SOUREC_DIR}/include/pdcurses)
    target_link_libraries(big_money pdcurses libcurl)
else()
    message(STATUS "Build big_money on *nix system")
    set(CURSES_NEED_NCURSES TRUE)
    find_package(Curses REQUIRED)
    if (CURSES_FOUND)
        message(STATUS "Looking for curses. Ok")
        target_include_directories(big_money PRIVATE ${CURSES_INCLUDE_DIRS})
        target_link_libraries(big_money ${CURSES_LIBRARIES})
    else()
        message(FATAL_ERROR "Looking for curses. Fail")
    endif()
    target_link_libraries(big_money ncurses curl)
endif()


target_include_directories(big_money PRIVATE 
                        ${CMAKE_CURRENT_SOURCE_DIR}/include
                        ${CMAKE_CURRENT_SOURCE_DIR}/3rd/rapidjson/include
)
