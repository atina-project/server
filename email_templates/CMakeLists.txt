project(atina-server-email-templates)

set(EMAIL_TEMPLATES_OUTPUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/data/email/template")

file(GLOB EMAIL_TEMPLATES_SRCS
    ${PROJECT_SOURCE_DIR}/*.html
)

set(EMAIL_TEMPLATES_COPY_CMD "")
foreach(EMAIL_TEMPLATE ${EMAIL_TEMPLATES_SRCS})
    get_filename_component(EMAIL_TEMPLATE_FILENAME ${EMAIL_TEMPLATE} NAME)

    set(EMAIL_TEMPLATE_OUTPUT_PATH "${EMAIL_TEMPLATES_OUTPUT_DIR}/${EMAIL_TEMPLATE_FILENAME}")
    list(APPEND EMAIL_TEMPLATES_COPY_CMD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${EMAIL_TEMPLATE} ${EMAIL_TEMPLATE_OUTPUT_PATH}
    )
endforeach()

add_custom_command(
    OUTPUT ${EMAIL_TEMPLATES_OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${EMAIL_TEMPLATES_OUTPUT_DIR}
    ${EMAIL_TEMPLATES_COPY_CMD}
    COMMENT "Checking and copying HTML email templates to build directory if necessary..."
)

add_custom_target(atina-server-email-templates-copy-html-files
    DEPENDS ${EMAIL_TEMPLATES_OUTPUT_DIR}
)

add_dependencies(atina-server-core
    atina-server-email-templates-copy-html-files
)
