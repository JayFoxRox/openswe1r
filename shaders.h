// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#ifndef __OPENSWE1R_SHADERS_H__
#define __OPENSWE1R_SHADERS_H__

static const char* VertexShader1Texture =
"#version 100\n"
"\n"
"uniform mat4 projectionMatrix;\n"
"\n"
"uniform bool fogEnable;\n"
"uniform float fogStart;\n"
"uniform float fogEnd;\n"
"uniform vec3 fogColor;\n"
"uniform vec3 clipScale;\n"
"uniform vec3 clipOffset;\n"
"\n"
"attribute vec4 positionIn;\n"
"attribute vec4 diffuseIn;\n"
"attribute vec4 specularIn;\n"
"attribute vec2 uv0In;\n"
"\n"
"varying vec4 diffuse;\n"
"varying vec4 specular;\n"
"varying vec2 uv0;\n"
"\n"
"void main() {\n"
"  gl_Position = vec4(positionIn.xyz * clipScale + clipOffset, 1.0);\n"
"  gl_Position /= positionIn.w;\n"
"  gl_Position.y = -gl_Position.y;\n"
"  diffuse = diffuseIn.bgra;\n"
"  specular = specularIn.bgra;\n"
"  uv0 = uv0In;\n"
"}";

static const char* FragmentShader1Texture =
"#version 100\n"
"\n"
"uniform sampler2D tex0;\n"
"uniform bool alphaTest;\n"
"uniform bool textureSwizzle;\n"
"\n"
"varying lowp vec4 diffuse;\n"
"varying lowp vec4 specular;\n"
"varying highp vec2 uv0;\n"
"\n"
"#define color gl_FragColor\n"
#if 0
"\n"
"uniform sampler2D tex0;\n"
#endif
"\n"
"void main() {\n"
"  color = texture2D(tex0, uv0);\n"
"  if (textureSwizzle) {\n"
"    color = color.gbar;\n"
"  }\n"
"  color *= diffuse;\n"
"  if (alphaTest && !(int(round(color.a * 255.0)) != 0)) { discard; }\n"
"}\n";

#endif
