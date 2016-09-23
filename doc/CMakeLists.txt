# add a target to generate API documentation with Doxygen
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/img DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/manual DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/vulfi.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/vulfi)
    configure_file(${doxyfile_in} ${doxyfile} @ONLY)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating Doxygen documentation for VULFI..")
    add_custom_target(doc2
        COMMAND pdflatex vulfi
        COMMAND bibtex vulfi
        COMMAND pdflatex vulfi
        COMMAND pdflatex vulfi
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/manual
        COMMENT "Generating VULFI Manual..")        
    #install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
endif()
