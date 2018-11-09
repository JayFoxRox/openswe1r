// This file does not belong to OpenSWE1R.
//
// It's mostly a prototype for other libs which will be required on Xbox.
// Some of these will likely be stubbed / disabled in OpenSWE1R.
// However, it's currently not known which parts exactly; so this is a hack.

#include <stdio.h>

#include <GL/glew.h>

#include <assert.h>
#include <stdbool.h>

#include <stdint.h>
#include <stdlib.h>


#include "xbox.h"




// unistd.. thanks SDL
void _exit(int status) {
  exit(status);
}




int alcCreateContext(int a0) {
  printf("%s\n", __func__);
  return 0;
}

int alcMakeContextCurrent(int a0) {
  printf("%s\n", __func__);
  return 1;
}

int alcOpenDevice(int a0) {
  printf("%s\n", __func__);
  return 1;
}






int alBufferData(int a0) {
  printf("%s\n", __func__);
  return 0;
}

int alGenBuffers(int a0) {
  printf("%s\n", __func__);
  return 0;
}

int alGenSources(int a0) {
  printf("%s\n", __func__);
  return 0;
}

int alSource3f(int a0) {
  printf("%s\n", __func__);
  return 0;
}

int alSourcef(int a0) {
  printf("%s\n", __func__);
  return 0;
}

int alSourcei(int a0) {
  printf("%s\n", __func__);
  return 0;
}

int alSourcePlay(int a0) {
  printf("%s\n", __func__);
  return 0;
}

int alSourceStop(int a0) {
  printf("%s\n", __func__);
  return 0;
}









static void* element_array_buffer = NULL;
static void* array_buffer = NULL;

extern float clipScale[];
extern float clipOffset[];

uint8_t buffer[640*480] = {0};

#define GLAPI // __stdcall


GLAPI void GLAPIENTRY glBindTexture (GLenum target, GLuint texture) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glClearDepth (GLclampd depth) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glClear (GLbitfield mask) {
  printf("%s\n", __func__);
  //FIXME: Clear color?!
  memset(buffer, 0x00, sizeof(buffer));
  return;
}

GLAPI void GLAPIENTRY glClearStencil (GLint s) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glCullFace (GLenum mode) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glDepthFunc (GLenum func) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glDepthMask (GLboolean flag) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glDisable (GLenum cap) {
  printf("%s\n", __func__);
  return;
}

static void saveBuffer() {

  static int t = 0;
  if (t++ < 60) {
    return;
  }
  t = 0;

  FILE* f = fopen("fb.ppm", "wb");
  fprintf(f, "P3\n"
             "640 480\n"
             "255\n");
  for(int y = 0; y < 480; y++) {
    for(int x = 0; x < 640; x++) {
      uint8_t r = buffer[y * 640 + x];
      uint8_t g = buffer[y * 640 + x];
      uint8_t b = buffer[y * 640 + x];
      fprintf(f, "%d %d %d  ", r,g,b);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

static void drawVertex(int i) {

  GLsizei stride = 4 * 4 + 4 + 4 + 1 * 8;

  struct Vertex {
    float x;
    float y;
    float z;
    float w;
    float unk0;    
    float unk1;
    float u;
    float v;
  };
  assert(sizeof(struct Vertex) == stride);

  struct Vertex* vertices = array_buffer;
  struct Vertex* v = &vertices[i];
/*  
"  gl_Position = vec4(positionIn.xyz * clipScale + clipOffset, 1.0);\n"
*/
  float fx = v->x * clipScale[0] + clipOffset[0];
  float fy = v->y * clipScale[1] + clipOffset[1];
  float fz = v->z * clipScale[2] + clipOffset[2];
  float fw = 1.0f;


  // "  gl_Position /= positionIn.w;\n"
  fx /= fw;
  fy /= fw;
  fz /= fw;
  fw /= fw;

  //"  gl_Position.y = -gl_Position.y;\n"
  fy = -fy;


  int x = fx * 320 + 320;
  int y = -fy * 240 + 240;
  int z = fz * 0xFF;

  // Clip
  if ((x < 0) || (x >= 640)) { return; }
  if ((y < 0) || (y >= 480)) { return; }
  
  //FIXME: fz is typically -1.0?!
  //if ((z < 0) || (z > 0xFF)) { return; }
  z = 0xFF;  

  buffer[y * 640 + x] = 0x80 + z / 2;

}

GLAPI void GLAPIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count) {
  printf("%s\n", __func__);

  for(unsigned int i = 0; i < count; i++) {
    drawVertex(first + i);
  }

  return;
}

GLAPI void GLAPIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const void *indices) {
  printf("%s\n", __func__);

  assert(type == GL_UNSIGNED_SHORT);
  uint16_t* i16 = (uint16_t*)((uintptr_t)element_array_buffer + (uintptr_t)indices);
  for(unsigned int i = 0; i < count; i++) {
    drawVertex(i16[i]);
  }

  return;
}

GLAPI void GLAPIENTRY glEnable (GLenum cap) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glGenTextures (GLsizei n, GLuint *textures) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glGetIntegerv (GLenum pname, GLint *params) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) {
  printf("%s\n", __func__);

  switch(type) {
  case GL_UNSIGNED_BYTE: {
    FILE* f = fopen("rgba8888.bin", "wb");
    fwrite(pixels, width * height * 4, 1, f);
    fclose(f);
    break;
  }
  case GL_UNSIGNED_SHORT_1_5_5_5_REV: {
    FILE* f = fopen("rgba5551.bin", "wb");
    fwrite(pixels, width * height * 2, 1, f);
    fclose(f);
    break;
  }
  case GL_UNSIGNED_SHORT_4_4_4_4_REV: {
    FILE* f = fopen("rgba4444.bin", "wb");
    fwrite(pixels, width * height * 2, 1, f);
    fclose(f);
    break;
  }
  default:
    assert(false);
    break;
  }

  printf("%s done\n", __func__);

  return;
}

GLAPI void GLAPIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height) {
  printf("%s\n", __func__);
  return;
}







GLAPI void GLAPIENTRY _glActiveTexture(unsigned int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glAttachShader(int a0) {
  printf("%s\n", __func__);
  return;
}

void GLAPIENTRY _glBindBuffer(GLenum target, GLuint buffer) {
  printf("%s\n", __func__);
  return;
}

void GLAPIENTRY _glBindVertexArray(int a0) {
  printf("%s\n", __func__);
  return;
}

void GLAPIENTRY _glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) {
  printf("%s\n", __func__);
  if (target == GL_ELEMENT_ARRAY_BUFFER) {
    element_array_buffer = realloc(element_array_buffer, size);
    memcpy(element_array_buffer, data, size);
  } else if (target == GL_ARRAY_BUFFER) {
    array_buffer = realloc(array_buffer, size);
    memcpy(array_buffer, data, size);
  } else {
    assert(false);
  }
  return;
}

GLAPI void GLAPIENTRY _glClearDepthf(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glCompileShader(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glCreateProgram(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glCreateShader(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glDebugMessageInsert(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glEnableVertexAttribArray(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glGenBuffers(int a0, unsigned int* a1) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glGenVertexArrays(int a0, unsigned int* a1) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glGetAttribLocation(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glewGetErrorString(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glGetProgramInfoLog(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glGetProgramiv(GLuint program, GLenum pname, GLint* param) {
  printf("%s\n", __func__);
  *param = GL_TRUE;
  return;
}

GLAPI void GLAPIENTRY _glGetShaderInfoLog(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glGetShaderiv(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI GLint GLAPIENTRY _glGetUniformLocation(GLuint program, const GLchar *name) {
  printf("%s\n", __func__);
  return name;
}

GLAPI void GLAPIENTRY _glLinkProgram(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glShaderSource(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glUniform1f(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glUniform1i(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glUniform3f(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glUniform3fv(GLint location, GLsizei count, const GLfloat *value) {
  printf("%s\n", __func__);
#if 0
  char* name = (char*)location;
  if (!strcmp(name, "clipScale")) {
    //FIXME!
  } else {
    printf("Unknown uniform: '%s'\n", name);
  }
#endif
  return;
}

GLAPI void GLAPIENTRY _glUniformMatrix4fv(int a0,  int a1,  unsigned char a2,  const float * a3) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glUseProgram(unsigned int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glVertexAttribPointer(unsigned int a0,  int a1,  unsigned int a2,  unsigned char a3,  int a4,  const void * a5) {
  printf("%s\n", __func__);
  return;
}






#undef GLEWAPI
#define GLEWAPI //extern
#define GLEW_FUN_EXPORT GLEWAPI

GLEW_FUN_EXPORT PFNGLACTIVETEXTUREPROC __glewActiveTexture = _glActiveTexture;
GLEW_FUN_EXPORT PFNGLGENVERTEXARRAYSPROC __glewGenVertexArrays = _glGenVertexArrays;
GLEW_FUN_EXPORT PFNGLUSEPROGRAMPROC __glewUseProgram = _glUseProgram;
GLEW_FUN_EXPORT PFNGLVERTEXATTRIBPOINTERPROC __glewVertexAttribPointer = _glVertexAttribPointer;
GLEW_FUN_EXPORT PFNGLUNIFORMMATRIX4FVPROC __glewUniformMatrix4fv = _glUniformMatrix4fv;
GLEW_FUN_EXPORT PFNGLGENBUFFERSPROC __glewGenBuffers = _glGenBuffers;
GLEW_FUN_EXPORT PFNGLGETATTRIBLOCATIONPROC __glewGetAttribLocation = _glGetAttribLocation;
GLEW_FUN_EXPORT PFNGLGETPROGRAMINFOLOGPROC __glewGetProgramInfoLog = _glGetProgramInfoLog;
GLEW_FUN_EXPORT PFNGLGETPROGRAMIVPROC __glewGetProgramiv = _glGetProgramiv;
GLEW_FUN_EXPORT PFNGLGETSHADERINFOLOGPROC __glewGetShaderInfoLog = _glGetShaderInfoLog;
GLEW_FUN_EXPORT PFNGLBINDVERTEXARRAYPROC __glewBindVertexArray = _glBindVertexArray;
GLEW_FUN_EXPORT PFNGLGETUNIFORMLOCATIONPROC __glewGetUniformLocation = _glGetUniformLocation;
GLEW_FUN_EXPORT PFNGLUNIFORM1IPROC __glewUniform1i = _glUniform1i;
GLEW_FUN_EXPORT PFNGLUNIFORM3FPROC __glewUniform3f = _glUniform3f;
GLEW_FUN_EXPORT PFNGLUNIFORM3FVPROC __glewUniform3fv = _glUniform3fv;
GLEW_FUN_EXPORT PFNGLUNIFORM1FPROC __glewUniform1f = _glUniform1f;
GLEW_FUN_EXPORT PFNGLENABLEVERTEXATTRIBARRAYPROC __glewEnableVertexAttribArray = _glEnableVertexAttribArray;
GLEW_FUN_EXPORT PFNGLDEBUGMESSAGEINSERTPROC __glewDebugMessageInsert = _glDebugMessageInsert;
GLEW_FUN_EXPORT PFNGLATTACHSHADERPROC __glewAttachShader = _glAttachShader;
GLEW_FUN_EXPORT PFNGLBUFFERDATAPROC __glewBufferData = _glBufferData;
GLEW_FUN_EXPORT PFNGLBINDBUFFERPROC __glewBindBuffer = _glBindBuffer;
GLEW_FUN_EXPORT PFNGLLINKPROGRAMPROC __glewLinkProgram = _glLinkProgram;
GLEW_FUN_EXPORT PFNGLCOMPILESHADERPROC __glewCompileShader = _glCompileShader;
GLEW_FUN_EXPORT PFNGLCLEARDEPTHFPROC __glewClearDepthf = _glClearDepthf;
GLEW_FUN_EXPORT PFNGLSHADERSOURCEPROC __glewShaderSource = _glShaderSource;
GLEW_FUN_EXPORT PFNGLCREATEPROGRAMPROC __glewCreateProgram = _glCreateProgram;
GLEW_FUN_EXPORT PFNGLCREATESHADERPROC __glewCreateShader = _glCreateShader;
GLEW_FUN_EXPORT PFNGLGETSHADERIVPROC __glewGetShaderiv = _glGetShaderiv;

GLEWAPI GLboolean glewExperimental = GL_FALSE;

GLEWAPI const GLubyte * GLEWAPIENTRY glewGetErrorString (GLenum error) {
  printf("%s\n", __func__);
  return "<glew error string>";
}

GLEWAPI GLenum GLEWAPIENTRY glewInit (void) {
  printf("%s\n", __func__);
  return 0;
}




int pthread_kill(int a0) {
  printf("%s\n", __func__);
  assert(false);
  return 0;
}

int pthread_sigmask(int a0) {
  printf("%s\n", __func__);
  return 0;
}












#if 1 //#ifndef XBOX


int SDL_CreateWindow(int a0) {
  printf("%s\n", __func__);
  return 1;
}

int SDL_Delay(int a0) {
  printf("%s\n", __func__);
  return 0;
}

int SDL_GetKeyboardState(int a0) {
  printf("%s\n", __func__);
  static unsigned char keys[256];
  return keys;
}

int SDL_GetMouseState(int a0) {
  printf("%s\n", __func__);
  return 0;
}

int SDL_GetPerformanceCounter(int a0) {
  printf("%s\n", __func__);
  return SDL_GetTicks();
}

int SDL_GetPerformanceFrequency(int a0) {
  printf("%s\n", __func__);
  return 1;
}

int SDL_GetTicks(int a0) {
  printf("%s\n", __func__);
  static int t = 0;
  return t++;
}

int SDL_GL_CreateContext(int a0) {
  printf("%s\n", __func__);
  return 1;
}

int SDL_GL_SetAttribute(int a0, int a1) {
  printf("%s\n", __func__);
  return 0;
}

int SDL_GL_SwapWindow(int a0) {
  printf("%s\n", __func__);

  saveBuffer();

  return 0;
}

int SDL_Init(int a0) {
  printf("%s\n", __func__);
  return 0;
}

int SDL_PollEvent(int a0) {
  printf("%s\n", __func__);
  return 0;
}

int SDL_ShowWindow(int a0) {
  printf("%s\n", __func__);
  return 0;
}

#endif
