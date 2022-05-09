set(component_srcs "main.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS ".")

#target_compile_options(${COMPONENT_TARGET} PRIVATE -DDISABLE_CJSON)
#target_compile_options(${COMPONENT_TARGET} PRIVATE -DDISABLE_XML)
#target_compile_options(${COMPONENT_TARGET} PRIVATE -DDISABLE_FREQ)
#target_compile_options(${COMPONENT_TARGET} PRIVATE -DDISABLE_MALLOC)


