glslc -fshader-stage=vert default_vert.glsl -o default_vert.spv
glslc -fshader-stage=frag default_frag.glsl -o default_frag.spv
glslc -fshader-stage=frag line_frag.glsl -o line_frag.spv
glslc -fshader-stage=vert line_vert.glsl -o line_vert.spv