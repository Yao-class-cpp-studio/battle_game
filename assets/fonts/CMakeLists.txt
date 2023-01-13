file(GLOB font_files *.ttf *.otf)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/fonts)
file(COPY ${font_files} DESTINATION ${CMAKE_BINARY_DIR}/fonts)
