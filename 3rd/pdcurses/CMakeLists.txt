aux_source_directory(src PD_SOURCE)
add_library(pdcurses ${PD_SOURCE})
target_include_directories(pdcurses PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(pdcurses PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include/pdcurses)