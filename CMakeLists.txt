cmake_minimum_required(VERSION 3.2.0)
project(big_money)

add_subdirectory(3rd)
aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC)

add_executable(big_money ${SRC})

if (MSVC)
    target_compile_options(big_money PRIVATE /utf-8)
endif()

if (WIN32 OR MSVC)
    message(STATUS "Build big_money on windows system")
    add_dependencies(big_money libcurl)
    target_include_directories(big_money PUBLIC ${CMAKE_SOUREC_DIR}/include/pdcurses)
    target_link_libraries(big_money pdcurses libcurl)
    add_definitions(-DNOMINMAX)
else()
    message(STATUS "Build big_money on *nix system")
    target_compile_options(big_money PRIVATE -std=c++11)
    find_package(curl, REQUIRED)
    if (CURL_FOUND) 
        message(STATUS "Looking for curl. Ok")
    else()
        message(FATAL_ERROR "Looking for curl. Fail")
    endif()

    set(CURSES_NEED_NCURSES TRUE)
    find_package(curses REQUIRED)
    if (CURSES_FOUND)
        message(STATUS "Looking for curses. Ok")
    else()
        message(FATAL_ERROR "Looking for curses. Fail")
    endif()
    target_link_libraries(big_money ncursesw curl)
endif()


target_include_directories(big_money PRIVATE 
                        ${CMAKE_SOURCE_DIR}/include
                        ${CMAKE_SOURCE_DIR}/3rd/rapidjson/include
                        ${CMAKE_SOURCE_DIR}/3rd/csylib/
)