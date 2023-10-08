find_program(MARP_EXECUTABLE "marp" REQUIRED)

function(add_marp_slides name source)
    find_file(source_full ${source} PATHS ${CMAKE_CURRENT_LIST_DIR} NO_CACHE)
    set(source ${source_full})

    if(CPP_COURSE_BUILD_SLIDES_PDF)
        set(output_pdf "${CMAKE_BINARY_DIR}/${name}.pdf")
        add_custom_command(
            OUTPUT ${output_pdf}
            COMMAND
                ${MARP_EXECUTABLE}
                --allow-local-files
                --html
                --theme ${CMAKE_SOURCE_DIR}/slides-support/themes/cscs.css
                --pdf
                -o ${output_pdf}
                ${source}
            MAIN_DEPENDENCY ${source}
        )
        add_custom_target(${name}_pdf ALL DEPENDS ${output_pdf})
    endif()

    set(output_html "${CMAKE_BINARY_DIR}/${name}.html")
    add_custom_command(
        OUTPUT ${output_html}
        COMMAND 
            ${MARP_EXECUTABLE} 
            --allow-local-files
            --html
            --theme ${CMAKE_SOURCE_DIR}/slides-support/themes/cscs.css
            -o ${output_html}
            ${source}
        MAIN_DEPENDENCY ${source}
    )
    add_custom_target(${name}_html ALL DEPENDS ${output_html})
endfunction()
