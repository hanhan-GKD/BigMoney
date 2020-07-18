if (WIN32 OR MSVC)
    message(STATUS "Fetch submodule code into 3rd...")
    execute_process(COMMAND git submodule update --init 3rd/curl
                    COMMAND git submodule update --init 3rd/rapidjson
                    COMMAND git submodule update --init 3rd/csylib
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    add_subdirectory(pdcurses)
    # option for curl, static lib only
    option(BUILD_CURL_EXE "" OFF)
    option(BUILD_SHARED_LIBS ""  OFF )
    add_subdirectory(curl)
else()
    message(STATUS "Fetch submodule code into 3rd...")
    execute_process(COMMAND git submodule update --init 3rd/rapidjson
                    COMMAND git submodule update --init 3rd/csylib
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()