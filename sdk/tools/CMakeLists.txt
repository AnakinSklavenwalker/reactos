
function(add_host_tool _tool)
    add_executable(${_tool} ${ARGN})
    set_target_properties(${_tool} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TOOLS_FOLDER})
endfunction()

#add_executable(pefixup pefixup.c)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -DHAVE_IO_H=1)
    add_compile_flags_language("/EHsc" "CXX")

    # Disable warning "conversion from 'size_t' to 'int', possible loss of data"
    add_compile_flags("/wd4267")
endif()

add_host_tool(bin2c bin2c.c)
add_host_tool(gendib gendib/gendib.c)
add_host_tool(geninc geninc/geninc.c)
add_host_tool(mkshelllink mkshelllink/mkshelllink.c)
add_host_tool(obj2bin obj2bin/obj2bin.c)
target_link_libraries(obj2bin PRIVATE host_includes)

add_host_tool(spec2def spec2def/spec2def.c)
add_host_tool(utf16le utf16le/utf16le.cpp)

add_subdirectory(cabman)
add_subdirectory(fatten)
add_subdirectory(hhpcomp)
add_subdirectory(hpp)
add_subdirectory(isohybrid)
add_subdirectory(kbdtool)
add_subdirectory(mkhive)
add_subdirectory(mkisofs)
add_subdirectory(unicode)
add_subdirectory(widl)
add_subdirectory(wpp)
add_subdirectory(xml2sdb)

if(NOT MSVC)
    add_subdirectory(log2lines)
    add_subdirectory(rsym)
endif()
