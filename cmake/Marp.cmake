find_program(MARP_EXECUTABLE "marp" REQUIRED)

function(add_marp_slides name source)
    find_file(source_full ${source} PATHS ${CMAKE_CURRENT_LIST_DIR} NO_CACHE)

    set(source ${source_full})
    set(output "${CMAKE_BINARY_DIR}/${name}.pdf")

    add_custom_command(
        OUTPUT ${output}
        COMMAND 
            ${MARP_EXECUTABLE} 
            --allow-local-files
            --html
            --theme ${CMAKE_SOURCE_DIR}/slides-support/themes/cscs.css
            --pdf
            -o ${output}
            ${source}
        MAIN_DEPENDENCY ${source}
    )
    
    add_custom_target(${name} ALL DEPENDS ${output})
endfunction()