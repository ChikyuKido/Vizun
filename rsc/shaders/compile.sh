mkdir -p temp

glslc -fshader-stage=vert image_vert.glsl -o temp/image_vert.spv
glslc -fshader-stage=frag image_frag.glsl -o temp/image_frag.spv
glslc -fshader-stage=frag line_frag.glsl -o temp/line_frag.spv
glslc -fshader-stage=vert line_vert.glsl -o temp/line_vert.spv
glslc -fshader-stage=frag font_frag.glsl -o temp/font_frag.spv
glslc -fshader-stage=vert font_vert.glsl -o temp/font_vert.spv


xxd -i temp/image_vert.spv | sed 's/temp_image_vert_spv/image_vert_data/g' > include/image_vert.h
xxd -i temp/image_frag.spv | sed 's/temp_image_frag_spv/image_frag_data/g' > include/image_frag.h
xxd -i temp/line_frag.spv | sed 's/temp_line_frag_spv/line_frag_data/g' > include/line_frag.h
xxd -i temp/line_vert.spv | sed 's/temp_line_vert_spv/line_vert_data/g' > include/line_vert.h
xxd -i temp/line_frag.spv | sed 's/temp_line_frag_spv/line_fill_frag_data/g' > include/line_fill_frag.h
xxd -i temp/line_vert.spv | sed 's/temp_line_vert_spv/line_fill_vert_data/g' > include/line_fill_vert.h
xxd -i temp/font_frag.spv | sed 's/temp_font_frag_spv/font_frag_data/g' > include/font_frag.h
xxd -i temp/font_vert.spv | sed 's/temp_font_vert_spv/font_vert_data/g' > include/font_vert.h


