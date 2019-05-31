// This file does not belong to OpenSWE1R.
//
// It's mostly a prototype for other libs which will be required on Xbox.
// Some of these will likely be stubbed / disabled in OpenSWE1R.
// However, it's currently not known which parts exactly; so this is a hack.




// Some hacks..
//#define DUMP_TEXTURE
#define SAME_TEXTURE // Without this, Xbox will run out of memory
#define USE_TEXTURES
#define MARK_VERTICES
//#define LOG





#include <stdio.h>

#include <stddef.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define _PTRDIFF_T_DEFINED
#include <GL/glew.h>

#include "xbox.h"

//typedef uint8_t Uint8;
//typedef uint32_t Uint32;

#include "SDL.h"

#include <hal/video.h>
static int SCREEN_WIDTH = 0;
static int SCREEN_HEIGHT = 0;
static int SCREEN_BPP = 0;

#include <hal/input.h>
static const int pad = 0;

static int mouse_x = 0;
static int mouse_y = 0;

#ifndef LOG
#define printf(fmt, ...)  {}
#endif














#include <string.h>
#include <strings.h>

#ifdef GPU
#include <hal/video.h>
#include <hal/xbox.h>
#include <math.h>
#include <pbkit/pbkit.h>
#include <xboxkrnl/xboxkrnl.h>
#include <xboxrt/debug.h>
#endif


#define MASK(mask, val) (((val) << (ffs(mask)-1)) & (mask))

// We need ou own, as pb_erase_depth_stencil_buffer doesn't support giving a value
static void erase_depth_stencil_buffer(int x, int y, int w, int h)
{
    DWORD       *p;

    int     x1,y1,x2,y2;

    x1=x;
    y1=y;
    x2=x+w;
    y2=y+h;
    printf("zclear{");
    p=pb_begin();
    pb_push(p++,NV20_TCL_PRIMITIVE_3D_CLEAR_VALUE_HORIZ,2);     //sets rectangle coordinates
    *(p++)=((x2-1)<<16)|x1;
    *(p++)=((y2-1)<<16)|y1;
    pb_push(p++,NV20_TCL_PRIMITIVE_3D_CLEAR_VALUE_DEPTH,3);     //sets data used to fill in rectangle
    if (SCREEN_BPP == 32) {
      *(p++)=0xffffff00;      //(depth<<8)|stencil
      *(p++)=0;           //color
      *(p++)=0x03;            //triggers the HW rectangle fill (only on D&S)
    } else {
      *(p++)=0xffff;      //depth
      *(p++)=0;           //color
      *(p++)=0x01;            //triggers the HW rectangle fill (only on D&S)
    }
    pb_end(p);
    printf("}");
}

/* Load the shader we will render with */
static void init_shader(void)
{
    uint32_t *p;
    int       i;

    /* Setup vertex shader */
    uint32_t vs_program[] = {
#include "xbox-hacks/vs.inl"
    };

    p = pb_begin();

    /* Set run address of shader */
    pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_START, 0);
    p += 2;

    /* Set execution mode */
    pb_push1(p, NV097_SET_TRANSFORM_EXECUTION_MODE,
        MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM)
        | MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV));
    p += 2;

    pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, 0);
    p += 2;

    /* Set cursor and begin copying program */
    pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_LOAD, 0);
    p += 2;

    for (i=0; i<sizeof(vs_program)/8; i++) {
        pb_push(p++, NV097_SET_TRANSFORM_PROGRAM, 4);
        memcpy(p, &vs_program[i*4], 4*4);
        p+=4;
    }

    pb_end(p);

    /* Setup fragment shader */
    p = pb_begin();
#include "xbox-hacks/ps.inl"
    pb_end(p);
}

/* Set an attribute pointer */
static void set_attrib_pointer(unsigned int index, unsigned int format, unsigned int size, unsigned int stride, const void* data)
{
    uint32_t *p = pb_begin();
    pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_FORMAT + index*4,
        MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE, format) | \
        MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE, size) | \
        MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE, stride));
    p += 2;
    pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_OFFSET + index*4, (uint32_t)data & 0x03ffffff);
    p += 2;
    pb_end(p);
}

/* Send draw commands for the triangles */
static void draw_arrays(unsigned int mode, int start, int count)
{
    uint32_t *p = pb_begin();
    pb_push1(p, NV097_SET_BEGIN_END, mode); p += 2;

    pb_push(p++,0x40000000|NV097_DRAW_ARRAYS,1); //bit 30 means all params go to same register 0x1810
    *(p++) = MASK(NV097_DRAW_ARRAYS_COUNT, (count-1)) | MASK(NV097_DRAW_ARRAYS_START_INDEX, start);

    pb_push1(p,NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END); p += 2;
    pb_end(p);
}








void initialize_screen(void) {
  static bool initialized = false;

  // Don't initialize twice
  if (initialized) {
    return;
  }

  // Get display resolution
  VIDEO_MODE vm = XVideoGetMode();
  SCREEN_WIDTH = vm.width;
  SCREEN_HEIGHT = vm.height;
  SCREEN_BPP = vm.bpp;

#ifdef GPU
  // Set up rendering
  pb_show_front_screen();
  int width = pb_back_buffer_width();
  int height = pb_back_buffer_height();
  assert(width == SCREEN_WIDTH);
  assert(height == SCREEN_HEIGHT);

  // Load shaders
  init_shader();

  // Prepare drawing
  pb_wait_for_vbl();
  pb_reset();
  pb_target_back_buffer();
#endif

  // Set up input
  XInput_Init();

  // Move mouse to center
  mouse_x = SCREEN_WIDTH / 2;
  mouse_y = SCREEN_HEIGHT / 2;

  // Mark as initialized
  initialized = true;
}


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
#ifdef USE_TEXTURES
static void** pixel_unpack_buffer = NULL;
#endif

extern float clipScale[];
extern float clipOffset[];


#ifndef GPU
uint8_t buffer[640*480] = {0};
#endif


#define GLAPI // __stdcall

#ifdef USE_TEXTURES
typedef struct {
  GLenum type;
  void** pbo;
  unsigned int width;
  unsigned int height;
} Texture;

static Texture* texture_2d = NULL;
#endif

GLAPI void GLAPIENTRY glBindTexture (GLenum target, GLuint texture) {
  printf("%s\n", __func__);

  assert(target == GL_TEXTURE_2D);

#ifdef USE_TEXTURES
  texture_2d = texture;

  printf("TEX Binding %p\n", texture);
#endif

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

#ifdef GPU

  /* Clear depth & stencil buffers */

  //FIXME: Check flag
  erase_depth_stencil_buffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  //FIXME: Check flag
  pb_fill(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xff202020); //FIXME: Use proper color

  //FIXME: Why?
  while(pb_busy()) {
      /* Wait for completion... */
  }

#else

  //FIXME: Clear color?!
  memset(buffer, 0x00, sizeof(buffer));
  debugClearScreen();

#endif

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


#ifndef GPU
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
#endif

#ifdef MARK_VERTICES
static void drawVertex(int i) {

  GLsizei stride = 4 * 4 + 4 + 4 + 2 * 4;

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

#ifdef GPU

  pb_fill(x, y, 1, 1, 0xFF00FFFF); // Cyan

#else

  // Draw to some debug output
  buffer[y * 640 + x] = 0x80 + z / 2;



  // Draw to actual Xbox framebuffer
  unsigned char *videoBuffer = XVideoGetFB();
  videoBuffer += (y * SCREEN_WIDTH + x) * (SCREEN_BPP >> 3);

  switch (SCREEN_BPP) {
    case 32:
      *((int*)videoBuffer) = 0xFFFFFFFF;
      break;
    case 16:
      *((short*)videoBuffer) = 0xFFFF;
      break;
  }

#endif

}
#endif

#ifdef GPU
static void prepare_vertices(void) {
  uint32_t *p;

#if 1
  // Be a bit paranoid..
  //FIXME: Need to handle ringbuffer in pbkit..
  while(pb_busy()) {
    /* Wait for completion... */
  }
  pb_reset();
#endif


#ifdef USE_TEXTURES

#if 0
  Texture t;
  t.width = 32;
  t.height = 32;
  t.type = GL_UNSIGNED_BYTE;
  static uint32_t* xxx = NULL;
  if (xxx == NULL) {
    xxx = MmAllocateContiguousMemoryEx(t.width * t.height * 4, 0, 0x3ffb000, 0, 0x404);
    for(int y = 0; y < t.height; y++) {
      for(int x = 0; x < t.width; x++) {
        xxx[y * t.width + x] = ((x + (y & 8)) & 8) ? 0xFFFFFFFF : 0xFF0000FF;
      }
    }
  }
  void* pbo = xxx;
  t.pbo = &pbo;
  texture_2d = &t;
#endif

#if 1
  /* Enable texture stage 0 */
  if (texture_2d != NULL) {

    void** pbo = texture_2d->pbo;
    uintptr_t pixels = *pbo;
    unsigned int width = texture_2d->width;
    unsigned int height = texture_2d->height;

    uint32_t fmt = 0;
    unsigned int pitch = 0;
    switch(texture_2d->type) {
    case GL_UNSIGNED_BYTE: {
      fmt = NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8;
      pitch = width * 4;
#ifdef DUMP_TEXTURE
      FILE* f = fopen("rgba8888.bin", "wb");
      fwrite(pixels, pitch * height, 1, f);
      fclose(f);
#endif
      break;
    }
    case GL_UNSIGNED_SHORT_1_5_5_5_REV: {
#define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1 0x3D
      fmt = NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1;
      pitch = width * 2;
#ifdef DUMP_TEXTURE
      FILE* f = fopen("rgba5551.bin", "wb");
      fwrite(pixels, pitch * height, 1, f);
      fclose(f);
#endif
      break;
    }
    case GL_UNSIGNED_SHORT_4_4_4_4_REV: {
      fmt = NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4;
      pitch = width * 2;
#ifdef DUMP_TEXTURE
      FILE* f = fopen("rgba4444.bin", "wb");
      fwrite(pixels, pitch * height, 1, f);
      fclose(f);
#endif
      break;
    }
    default:
      assert(false);
      break;
    }

    printf("%s done\n", __func__);

    p=pb_begin();
    pb_push2(p,NV20_TCL_PRIMITIVE_3D_TX_OFFSET(0),(DWORD)(MmGetPhysicalAddress(pixels) & 0x03ffffff), (0x0001002a | (fmt << 8))); p+=3; //set stage 0 texture address & format
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_PITCH(0),pitch<<16); p+=2; //set stage 0 texture pitch (pitch<<16)
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_SIZE(0),(width<<16)|height); p+=2; //set stage 0 texture width & height ((witdh<<16)|height)
    //pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(0),0x00010101); p+=2;//set stage 0 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(0),0x4003ffc0); p+=2; //set stage 0 texture enable flags
    //pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(0),0x04074000); p+=2; //set stage 0 texture filters (AA!)
    pb_end(p);

    printf("TEX Activated %p: %u x %u (pitch: %u), PBO: %p pixels: %p / %p\n", texture_2d, width, height, pitch, pbo, pixels, (void*)MmGetPhysicalAddress(pixels));

  } else {

    p=pb_begin();
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(0),0x0003ffc0); p+=2;//set stage 0 texture enable flags (bit30 disabled)
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(0),0x00030303); p+=2;//set stage 0 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(0),0x02022000); p+=2;//set stage 0 texture filters (no AA, stage not even used)
    pb_end(p);

    printf("TEX Deactivated\n");

  }
#endif

  // Disable other texture stages

  p=pb_begin();

  pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(1),0x0003ffc0); p+=2;//set stage 1 texture enable flags (bit30 disabled)
  pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(1),0x00030303); p+=2;//set stage 1 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
  pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(1),0x02022000); p+=2;//set stage 1 texture filters (no AA, stage not even used)

  pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(2),0x0003ffc0); p+=2;//set stage 2 texture enable flags (bit30 disabled)
  pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(2),0x00030303); p+=2;//set stage 2 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
  pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(2),0x02022000); p+=2;//set stage 2 texture filters (no AA, stage not even used)

  pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(3),0x0003ffc0); p+=2;//set stage 3 texture enable flags (bit30 disabled)
  pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(3),0x00030303); p+=2;//set stage 3 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
  pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(3),0x02022000); p+=2;//set stage 3 texture filters (no AA, stage not even used)

  pb_end(p);
#endif

 // Setup matrix
  {

//    float m_viewport[4][4];
//    matrix_viewport(m_viewport, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 65536.0f);

    /*
    * WARNING: Changing shader source code may impact constant locations!
    * Check the intermediate file (*.inl) for the expected locations after
    * changing the code.
    */
    p = pb_begin();

    /* Set shader constants cursor at C0 */
    pb_push1(p, NV097_SET_TRANSFORM_CONSTANT_LOAD, 96); p+=2;

    /* Send the transformation matrix */
#ifdef USE_TEXTURES
    float tex0Scale[4] = { texture_2d->width, texture_2d->height, 0, 0 };
#else
    float tex0Scale[4] = { 1, 1, 0, 0 };
#endif
    float c3[4] = {1, 320, -240, 0};
    float c4[4] = {320, 240, 65536, 1};
    pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, 5 * 4);
    memset(p, 0x00, (4 * 4) *4);
    memcpy(p, clipScale, 3*4); p+=4; // c0
    memcpy(p, clipOffset, 3*4); p+=4; // c1
    memcpy(p, tex0Scale, 4*4); p+=4; // c2
    memcpy(p, c3, 4*4); p+=4; // c3
    memcpy(p, c4, 4*4); p+=4; // c4

    pb_end(p);
  }


  p = pb_begin();

#if 1
  //FIXME: Debug hack to set up wireframe mode
  unsigned int fill_mode = (g_Pads[pad].CurrentButtons.ucAnalogButtons[XPAD_WHITE] > 0x20) ? 0x1B01 : 0x1B02;

  pb_push2(p,NV20_TCL_PRIMITIVE_3D_POLYGON_MODE_FRONT,fill_mode,fill_mode); p+=3; //FillMode="solid" BackFillMode="point"


  pb_push1(p,NV20_TCL_PRIMITIVE_3D_CULL_FACE_ENABLE,0); p+=2;//CullModeEnable=TRUE
  pb_push1(p,NV20_TCL_PRIMITIVE_3D_DEPTH_TEST_ENABLE,0); p+=2; //ZEnable=TRUE or FALSE (But don't use W)
#endif

  /* Clear all attributes */
  pb_push(p++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT,16);
  for(int i = 0; i < 16; i++) {
      *(p++) = NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F;
  }

  pb_end(p);


  size_t stride = 4 * 4 + 4 + 4 + 2 * 4;
  //FIXME: Is this right? our sample doesn't use phys address...
  uint8_t* buf = MmGetPhysicalAddress(array_buffer); //FIXME: & 0x03ffffff ?


  /* Set vertex position attribute */
  set_attrib_pointer(0, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F, 4, stride, &buf[0]);

  /* Set vertex diffuse color attribute */
  set_attrib_pointer(3, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_D3D, 4, stride, &buf[0+4*4]);

  /* Set vertex specular color attribute */
  set_attrib_pointer(4, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_D3D, 4, stride, &buf[0+4*4+4]);

  /* Set vertex uv0 attribute */
  set_attrib_pointer(8, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F, 2, stride, &buf[0+4*4+4+4]);

}
#endif

GLAPI void GLAPIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count) {
  printf("%s\n", __func__);

#ifdef GPU
  prepare_vertices();

debugPrint("array {\n");
  /* Begin drawing triangles */
  draw_arrays(NV097_SET_BEGIN_END_OP_TRIANGLES, first, count);
debugPrint("}\n");

#endif

debugPrint("heh.. fuck!\n");
pb_kill();
while(1);

#ifdef MARK_VERTICES
  for(unsigned int i = 0; i < count; i++) {
    drawVertex(first + i);
  }
#endif

  return;
}

GLAPI void GLAPIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const void *indices) {
  printf("%s\n", __func__);

//debugPrint("elements: %d\n", count);

  // Skip empty draws
  if (count == 0) {
    return;
  }

  assert(count <= 2000);


  assert(type == GL_UNSIGNED_SHORT);
  uint16_t* i16 = (uint16_t*)((uintptr_t)element_array_buffer + (uintptr_t)indices);

#ifdef GPU

  //FIXME: Why doesn't this work? still only draws some of them
#if 1
  //FIXME: Split into multiple of 60 [divisible by 1, 2, 3, 4, 5] element batches
  unsigned int batch = 60;
  if (count > batch) {
    printf("...batch: %d\n", count);

    //FIXME: Debug hack to only draw last vertices
    if (!(g_Pads[pad].CurrentButtons.ucAnalogButtons[XPAD_BLACK] > 0x20)) {
      glDrawElements(mode, batch, type, indices);
    }

    glDrawElements(mode, count - batch, type, ((uintptr_t)indices) + 2*batch);
    return;
  }
#endif


  prepare_vertices();

  //FIXME: Make sure this is always the right mode!
  int gpu_mode = 0;
  if (mode == GL_TRIANGLES) {
    gpu_mode = NV097_SET_BEGIN_END_OP_TRIANGLES;
  } else {
    printf("Oops! mode %d\n", mode);
    assert(false);
  }

  uint32_t *p = pb_begin();

  pb_push1(p, NV097_SET_BEGIN_END, gpu_mode); p += 2;

  unsigned int pair_count = count / 2;
  if (pair_count > 0) {

    pb_push(p++,0x40000000|NV097_ARRAY_ELEMENT16, pair_count); //bit 30 means all params go to same register

    for(unsigned int i = 0; i < pair_count; i++) {
      //FIXME: memcpy instead (endianess?)
      *(p++) = (i16[2 * i + 1] << 16) | i16[2 * i + 0];
    }
  }

  if (count & 1) {
    pb_push(p++, NV097_ARRAY_ELEMENT32, 1); *p++ = i16[count - 1];
  }

  pb_push(p++, NV097_SET_BEGIN_END, 1); *p++ = NV097_SET_BEGIN_END_OP_END;

  pb_end(p);

#if 0
pb_kill();
while(1);
#endif
#endif

#ifdef MARK_VERTICES
  for(unsigned int i = 0; i < count; i++) {
    drawVertex(i16[i]);
  }
#endif

  return;
}

GLAPI void GLAPIENTRY glEnable (GLenum cap) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY glGenTextures (GLsizei n, GLuint *textures) {
  printf("%s\n", __func__);

  assert(n == 1);

#ifdef USE_TEXTURES
  
  Texture* texture = malloc(sizeof(Texture));
  memset(texture, 0x00, sizeof(Texture));

  textures[0] = texture;

  printf("TEX Created %p\n", texture);
#endif

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

  assert(target == GL_TEXTURE_2D);

#ifdef USE_TEXTURES
  assert(texture_2d != NULL); // Texture should be set

  assert(pixel_unpack_buffer != NULL); // PBO should be set
  assert(*pixel_unpack_buffer != NULL); // PBO should contain data

  assert(pixels == NULL); // PBO data should always be at base

  // Point texture at PBO, so we can find the data
  texture_2d->pbo = pixel_unpack_buffer;
  texture_2d->type = type; // Keep track of type
  texture_2d->width = width;
  texture_2d->height = height;

  printf("TEX Image %p %d x %d, type: %d, PBO: %p\n", texture_2d, width, height, type, pixel_unpack_buffer);
#endif

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







GLAPI void GLAPIENTRY _glActiveTexture(GLenum texture) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glAttachShader(GLuint program, GLuint shader) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glBindBuffer(GLenum target, GLuint buffer) {
  printf("%s\n", __func__);

#ifdef USE_TEXTURES
  if (target == GL_PIXEL_UNPACK_BUFFER) {
    pixel_unpack_buffer = buffer;
  }
#endif

  return;
}

GLAPI void GLAPIENTRY _glBindVertexArray(GLuint array) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) {
  printf("%s\n", __func__);

  if (target == GL_ELEMENT_ARRAY_BUFFER) {
    // Xbox puts element arrays into FIFO, so only accessed by CPU
    element_array_buffer = realloc(element_array_buffer, size);
    memcpy(element_array_buffer, data, size);
  } else if (target == GL_ARRAY_BUFFER) {
    // Xbox accesses vertices with GPU, so must be non-paged

    //FIXME: Only wait if buffer is in use
    while(pb_busy()) {
        /* Wait for completion... */
    }

    if (array_buffer != NULL) { MmFreeContiguousMemory(array_buffer); }
    array_buffer = MmAllocateContiguousMemoryEx(size, 0, 0x3ffb000, 0, 0x404);
    memcpy(array_buffer, data, size);
  } else if (target == GL_PIXEL_UNPACK_BUFFER) {
#ifdef USE_TEXTURES
    assert(pixel_unpack_buffer != NULL); // PBO must be bound

#ifdef SAME_TEXTURE
    static void* large_pbo = NULL;
    {
      size_t fake_size = 640*480*4;
      assert(size <= fake_size);
      if (large_pbo == NULL) {
        large_pbo = MmAllocateContiguousMemoryEx(fake_size, 0, 0x3ffb000, 0, 0x404);
      }
    }
    *pixel_unpack_buffer = large_pbo;
#else
    *pixel_unpack_buffer = MmAllocateContiguousMemoryEx(size, 0, 0x3ffb000, 0, 0x404);
    if (*pixel_unpack_buffer != NULL) { MmFreeContiguousMemory(*pixel_unpack_buffer); }
#endif
    memcpy(*pixel_unpack_buffer, data, size);
#endif
  } else {
    assert(false);
  }
  return;
}

GLAPI void GLAPIENTRY _glClearDepthf(GLfloat depth) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glCompileShader(GLuint shader) {
  printf("%s\n", __func__);
  return;
}

GLAPI GLuint GLAPIENTRY _glCreateProgram(void) {
  printf("%s\n", __func__);
  return 1;
}

GLAPI GLuint GLAPIENTRY _glCreateShader(GLenum shaderType) {
  printf("%s\n", __func__);
  return 1;
}

GLAPI void GLAPIENTRY _glDebugMessageInsert(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *message) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glEnableVertexAttribArray(GLuint index) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glGenBuffers(GLsizei n, GLuint * buffers) {

  printf("%s\n", __func__);

  assert(n == 1);

  // Allocate a pointer to buffer
  void** ptr = malloc(sizeof(void*));
  *ptr = NULL;

  buffers[0] = ptr;

  return;
}

GLAPI void GLAPIENTRY _glGenVertexArrays(int a0, unsigned int* a1) {
  printf("%s\n", __func__);
  return;
}

GLAPI GLint GLAPIENTRY _glGetAttribLocation(GLuint program, const GLchar *name) {
  printf("%s\n", __func__);
  return (GLint)name;
}

GLAPI void * GLAPIENTRY _glMapBuffer(GLenum target, GLenum access) {
  printf("%s\n", __func__);

  assert(target == GL_PIXEL_UNPACK_BUFFER);

#ifdef USE_TEXTURES

  assert(pixel_unpack_buffer != NULL); // PBO should be set
  assert(*pixel_unpack_buffer != NULL); // PBO should contain data

  return *pixel_unpack_buffer;
#else
  return NULL;
#endif
}

GLAPI GLboolean GLAPIENTRY _glUnmapBuffer(GLenum target) {
  printf("%s\n", __func__);

  assert(target == GL_PIXEL_UNPACK_BUFFER);

  return 0;
}

GLAPI void GLAPIENTRY _glewGetErrorString(int a0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog) {
  printf("%s\n", __func__);
  strcpy(infoLog, "");
  return;
}

GLAPI void GLAPIENTRY _glGetProgramiv(GLuint program, GLenum pname, GLint* param) {
  printf("%s\n", __func__);
  *param = GL_TRUE;
  return;
}

GLAPI void GLAPIENTRY _glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog) {
  printf("%s\n", __func__);
  strcpy(infoLog, "");
  return;
}

GLAPI void GLAPIENTRY _glGetShaderiv(GLuint shader, GLenum pname, GLint *params) {
  printf("%s\n", __func__);
  return;
}

GLAPI GLint GLAPIENTRY _glGetUniformLocation(GLuint program, const GLchar *name) {
  printf("%s\n", __func__);
  return (GLint)name;
}

GLAPI void GLAPIENTRY _glLinkProgram(GLuint program) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glShaderSource(GLuint shader, GLsizei count, const GLchar * const*string, const GLint *length) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glUniform1f(GLint location, GLfloat v0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glUniform1i(GLint location, GLint v0) {
  printf("%s\n", __func__);
  return;
}

GLAPI void GLAPIENTRY _glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) {
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

GLAPI void GLAPIENTRY _glUseProgram(GLuint program) {
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
GLEW_FUN_EXPORT PFNGLMAPBUFFERPROC __glewMapBuffer = _glMapBuffer;
GLEW_FUN_EXPORT PFNGLUNMAPBUFFERPROC __glewUnmapBuffer = _glUnmapBuffer;

GLEWAPI GLboolean glewExperimental = GL_FALSE;

GLEWAPI const GLubyte * GLEWAPIENTRY glewGetErrorString (GLenum error) {
  printf("%s\n", __func__);
  return (GLubyte*)"<glew error string>";
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


SDL_Window* SDL_CreateWindow(const char* title,
                             int         x,
                             int         y,
                             int         w,
                             int         h,
                             Uint32      flags) {
  printf("%s\n", __func__);
  return (SDL_Window*)1;
}

void SDL_Delay(Uint32 ms) {
  printf("%s\n", __func__);
  return;
}

const Uint8* SDL_GetKeyboardState(int* numkeys) {
  printf("%s\n", __func__);
  static unsigned char keys[256];
  memset(keys, 0x00, sizeof(keys));

  int pad = 0;

  //FIXME: Poll input - This is a hack! because of shitty USB code
  XInput_GetEvents();

  keys[SDL_SCANCODE_ESCAPE] |= (g_Pads[pad].CurrentButtons.ucAnalogButtons[XPAD_B] > 0x20)? 1 : 0;
  keys[SDL_SCANCODE_RETURN] |= (g_Pads[pad].CurrentButtons.ucAnalogButtons[XPAD_A] > 0x20)? 1 : 0;
  keys[SDL_SCANCODE_SPACE] |= (g_Pads[pad].CurrentButtons.ucAnalogButtons[XPAD_X] > 0x20)? 1 : 0;
  keys[SDL_SCANCODE_UP] |= (g_Pads[pad].CurrentButtons.usDigitalButtons & XPAD_DPAD_UP) ? 1 : 0;
  keys[SDL_SCANCODE_DOWN] |= (g_Pads[pad].CurrentButtons.usDigitalButtons & XPAD_DPAD_DOWN) ? 1 : 0;
  keys[SDL_SCANCODE_LEFT] |= (g_Pads[pad].CurrentButtons.usDigitalButtons & XPAD_DPAD_LEFT) ? 1 : 0;
  keys[SDL_SCANCODE_RIGHT] |= (g_Pads[pad].CurrentButtons.usDigitalButtons & XPAD_DPAD_RIGHT) ? 1 : 0;
  keys[SDL_SCANCODE_Q] |= 0;
  keys[SDL_SCANCODE_W] |= 0;
  keys[SDL_SCANCODE_E] |= 0;
  keys[SDL_SCANCODE_R] |= 0;
  keys[SDL_SCANCODE_I] |= 0;
  keys[SDL_SCANCODE_A] |= 0;
  keys[SDL_SCANCODE_S] |= 0;
  keys[SDL_SCANCODE_D] |= 0;
  keys[SDL_SCANCODE_F] |= 0;
  keys[SDL_SCANCODE_J] |= 0;
  keys[SDL_SCANCODE_K] |= 0;
  keys[SDL_SCANCODE_L] |= 0;
  keys[SDL_SCANCODE_M] |= 0;
  keys[SDL_SCANCODE_F1] |= 0;
  keys[SDL_SCANCODE_F2] |= 0;
  keys[SDL_SCANCODE_F3] |= 0;
  keys[SDL_SCANCODE_F4] |= 0;
  keys[SDL_SCANCODE_F5] |= 0;
  keys[SDL_SCANCODE_F6] |= 0;
  keys[SDL_SCANCODE_F7] |= 0;
  keys[SDL_SCANCODE_F12] |= 0;
  keys[SDL_SCANCODE_GRAVE] |= 0;
  keys[SDL_SCANCODE_EQUALS] |= 0;
  keys[SDL_SCANCODE_MINUS] |= 0;
  keys[SDL_SCANCODE_TAB] |= 0;
  keys[SDL_SCANCODE_CAPSLOCK] |= 0;
  keys[SDL_SCANCODE_LSHIFT] |= 0;
  keys[SDL_SCANCODE_RSHIFT] |= 0;
  keys[SDL_SCANCODE_LCTRL] |= 0;
  keys[SDL_SCANCODE_RCTRL] |= 0;
  keys[SDL_SCANCODE_LALT] |= 0;
  keys[SDL_SCANCODE_RALT] |= 0;

  return keys;
}

Uint32 SDL_GetMouseState(int* x, int* y) {
  printf("%s\n", __func__);

  //FIXME: Consider time as aspect and use
  //mouse_x += dx * dt;
  //mouse_y += dy * dt;

  //FIXME: Poll input - This is a hack! because of shitty USB code
  XInput_GetEvents();

  //FIXME: This is just a dirty hack.. remove
  mouse_x = SCREEN_WIDTH / 2 + g_Pads[pad].sRThumbX / 64;
  mouse_y = -40 + SCREEN_HEIGHT / 2 - g_Pads[pad].sRThumbY / 64; // Hack to pre-select single player

  if (mouse_x < 0) { mouse_x = 0; }
  if (mouse_y < 0) { mouse_y = 0; }
  if (mouse_x > SCREEN_WIDTH) { mouse_x = SCREEN_WIDTH; }
  if (mouse_y > SCREEN_HEIGHT) { mouse_y = SCREEN_HEIGHT; }

  *x = mouse_x;
  *y = mouse_y;

  return 0;
}

Uint32 SDL_GetTicks(void) {
  printf("%s\n", __func__);
  static uint32_t t = 0;
  return t++;
}

Uint64 SDL_GetPerformanceCounter(void) {
  printf("%s\n", __func__);
  return SDL_GetTicks();
}

Uint64 SDL_GetPerformanceFrequency(void) {
  printf("%s\n", __func__);
  return 1;
}

SDL_GLContext SDL_GL_CreateContext(SDL_Window* window) {
  printf("%s\n", __func__);
  return (SDL_GLContext)1;
}

int SDL_GL_SetAttribute(SDL_GLattr attr, int        value) {
  printf("%s\n", __func__);
  return 0;
}

void SDL_GL_SwapWindow(SDL_Window* window) {
  printf("%s\n", __func__);

#ifdef GPU

  //FIXME: Poll input - This is a hack! because of shitty USB code
  XInput_GetEvents();

  // Draw some text on the screen
  pb_print("OpenSWE1R\n");
  extern uint32_t callId;
  pb_print("Call: %u\n", callId);
  pb_print("Mouse: %u %u\n", mouse_x, mouse_y);
  pb_print("Pad: %d %d\n", g_Pads[pad].sRThumbX, g_Pads[pad].sRThumbY);

  pb_draw_text_screen();
  pb_erase_text_screen();

  // Finish current frame
  
  while(pb_busy()) {
      /* Wait for completion... */
  }

  /* Swap buffers (if we can) */
  while (pb_finished()) {
      /* Not ready to swap yet */
  }





  // Prepare for next frame

  pb_wait_for_vbl();
  pb_reset();
  //pb_target_back_buffer();

#else

  // Already done during draw: single buffered

#endif

  //saveBuffer();

  return;
}

int SDL_Init(Uint32 flags) {
  printf("%s\n", __func__);
  return 0;
}

int SDL_PollEvent(SDL_Event* event) {
  printf("%s\n", __func__);
  return 0;
}

void SDL_ShowWindow(SDL_Window* window) {
  printf("%s\n", __func__);
  return;
}

#endif
