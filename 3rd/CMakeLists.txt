if (WIN32 OR MSVC)
    add_subdirectory(pdcurses)
    # option for curl, static lib only
    option(BUILD_CURL_EXE "" OFF)
    option(BUILD_SHARED_LIBS ""  OFF )
    add_subdirectory(curl)
endif()