#ifndef __OPENSWE1R_SHADERS_H__
#define __OPENSWE1R_SHADERS_H__

static const char* VertexShader1Texture =
"#version 330\n"
"\n"
"in vec4 positionIn;\n"
"in vec4 diffuseIn;\n"
"in vec4 specularIn;\n"
"in vec2 uv0In;\n"
"\n"
"out vec4 diffuse;\n"
"out vec4 specular;\n"
"out vec2 uv0;\n"
"\n"
"void main() {\n"
"  vec3 p = positionIn.xyz /* positionIn.w */;\n"
"  gl_Position = vec4(p.xy / vec2(640.0, -480.0) * 2.0 + vec2(-1.0, 1.0), p.z, 1.0);\n"
"  diffuse = diffuseIn.bgra;\n"
"  specular = specularIn.bgra;\n"
"  uv0 = uv0In;\n"
"}";

static const char* FragmentShader1Texture =
"#version 330\n"
"\n"
"uniform sampler2D tex0;\n"
"\n"
"in vec4 diffuse;\n"
"in vec4 specular;\n"
"in vec2 uv0;\n"
"\n"
"out vec4 color;\n"
#if 0
"\n"
"uniform sampler2D tex0;\n"
#endif
"\n"
"void main() {\n"
"  color = diffuse * texture2D(tex0, vec2(uv0.x, 1.0 - uv0.y));\n"
"}\n";

#endif
