file(GLOB textures *.png *.jpg *.hdr *.bmp *.jpeg)

file(MAKE_DIRECTORY ${GRASSLAND_BINARY_DIR}/textures)

foreach(texture ${textures})
    file(COPY ${texture} DESTINATION ${GRASSLAND_BINARY_DIR}/textures)
endforeach()
