file(GLOB texture_files *.jpg *.bmp *.png *.jpeg *.hdr)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/textures)

file(COPY ${texture_files} DESTINATION ${CMAKE_BINARY_DIR}/textures)
