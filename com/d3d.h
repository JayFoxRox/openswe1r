#ifndef __OPENSWE1R_COM_D3D_H__
#define __OPENSWE1R_COM_D3D_H__

#define MS__DIRECT3D_VERSION 0x0600

#include "../emulation.h"
#include "../windows.h"

typedef struct {
  void* vtable;
  Address surface;
  GLuint handle;
} MS(Direct3DTexture2);

typedef struct {
  union {
    uint32_t x1;
    uint32_t lX1;
  };
  union {
    uint32_t y1;
    uint32_t lY1;
  };
  union {
    uint32_t x2;
    uint32_t lX2;
  };
  union {
    uint32_t y2;
    uint32_t lY2;
  };
} MS(D3DRECT);

enum {
  MS(D3DCLEAR_TARGET) = 0x00000001,
  MS(D3DCLEAR_ZBUFFER) = 0x00000002,
  MS(D3DCLEAR_STENCIL) = 0x00000004
};

typedef float MS(D3DVALUE); // Assumes 32 bit float

typedef struct {
    uint32_t       dwSize;
    uint32_t       dwX;
    uint32_t       dwY;        /* Viewport Top left */
    uint32_t       dwWidth;
    uint32_t       dwHeight;   /* Viewport Dimensions */
    MS(D3DVALUE)    dvClipX;        /* Top left of clip volume */
    MS(D3DVALUE)    dvClipY;
    MS(D3DVALUE)    dvClipWidth;    /* Clip Volume Dimensions */
    MS(D3DVALUE)    dvClipHeight;
    MS(D3DVALUE)    dvMinZ;         /* Min/max of clip Volume */
    MS(D3DVALUE)    dvMaxZ;
} MS(D3DVIEWPORT2);


// From DX6 SDK

typedef enum {
    MS(D3DRENDERSTATE_ANTIALIAS)         = 2,    /* D3DANTIALIASMODE */
    MS(D3DRENDERSTATE_TEXTUREPERSPECTIVE)= 4,    /* TRUE for perspective correction */
    MS(D3DRENDERSTATE_ZENABLE)           = 7,    /* D3DZBUFFERTYPE (or TRUE/FALSE for legacy) */
    MS(D3DRENDERSTATE_FILLMODE)          = 8,    /* D3DFILL_MODE        */
    MS(D3DRENDERSTATE_SHADEMODE)         = 9,    /* D3DSHADEMODE */
    MS(D3DRENDERSTATE_LINEPATTERN)       = 10,   /* D3DLINEPATTERN */
    MS(D3DRENDERSTATE_ZWRITEENABLE)      = 14,   /* TRUE to enable z writes */
    MS(D3DRENDERSTATE_ALPHATESTENABLE)   = 15,   /* TRUE to enable alpha tests */
    MS(D3DRENDERSTATE_LASTPIXEL)         = 16,   /* TRUE for last-pixel on lines */
    MS(D3DRENDERSTATE_SRCBLEND)          = 19,   /* D3DBLEND */
    MS(D3DRENDERSTATE_DESTBLEND)         = 20,   /* D3DBLEND */
    MS(D3DRENDERSTATE_CULLMODE)          = 22,   /* D3DCULL */
    MS(D3DRENDERSTATE_ZFUNC)             = 23,   /* D3DCMPFUNC */
    MS(D3DRENDERSTATE_ALPHAREF)          = 24,   /* D3DFIXED */
    MS(D3DRENDERSTATE_ALPHAFUNC)         = 25,   /* D3DCMPFUNC */
    MS(D3DRENDERSTATE_DITHERENABLE)      = 26,   /* TRUE to enable dithering */
#if(MS__DIRECT3D_VERSION >= 0x0500)
    MS(D3DRENDERSTATE_ALPHABLENDENABLE)  = 27,   /* TRUE to enable alpha blending */
#endif /* MS__DIRECT3D_VERSION >= 0x0500 */
    MS(D3DRENDERSTATE_FOGENABLE)         = 28,   /* TRUE to enable fog blending */
    MS(D3DRENDERSTATE_SPECULARENABLE)    = 29,   /* TRUE to enable specular */
    MS(D3DRENDERSTATE_ZVISIBLE)          = 30,   /* TRUE to enable z checking */
    MS(D3DRENDERSTATE_STIPPLEDALPHA)     = 33,   /* TRUE to enable stippled alpha (RGB device only) */
    MS(D3DRENDERSTATE_FOGCOLOR)          = 34,   /* D3DCOLOR */
    MS(D3DRENDERSTATE_FOGTABLEMODE)      = 35,   /* D3DFOGMODE */
#if(MS__DIRECT3D_VERSION >= 0x0700)
    MS(D3DRENDERSTATE_FOGSTART)          = 36,   /* Fog start (for both vertex and pixel fog) */
    MS(D3DRENDERSTATE_FOGEND)            = 37,   /* Fog end      */
    MS(D3DRENDERSTATE_FOGDENSITY)        = 38,   /* Fog density  */
#endif /* MS__DIRECT3D_VERSION >= 0x0700 */
#if(MS__DIRECT3D_VERSION >= 0x0500)
    MS(D3DRENDERSTATE_EDGEANTIALIAS)     = 40,   /* TRUE to enable edge antialiasing */
    MS(D3DRENDERSTATE_COLORKEYENABLE)    = 41,   /* TRUE to enable source colorkeyed textures */
    MS(D3DRENDERSTATE_ZBIAS)             = 47,   /* LONG Z bias */
    MS(D3DRENDERSTATE_RANGEFOGENABLE)    = 48,   /* Enables range-based fog */
#endif /* MS__DIRECT3D_VERSION >= 0x0500 */

#if(MS__DIRECT3D_VERSION >= 0x0600)
    MS(D3DRENDERSTATE_STENCILENABLE)     = 52,   /* BOOL enable/disable stenciling */
    MS(D3DRENDERSTATE_STENCILFAIL)       = 53,   /* D3DSTENCILOP to do if stencil test fails */
    MS(D3DRENDERSTATE_STENCILZFAIL)      = 54,   /* D3DSTENCILOP to do if stencil test passes and Z test fails */
    MS(D3DRENDERSTATE_STENCILPASS)       = 55,   /* D3DSTENCILOP to do if both stencil and Z tests pass */
    MS(D3DRENDERSTATE_STENCILFUNC)       = 56,   /* D3DCMPFUNC fn.  Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true */
    MS(D3DRENDERSTATE_STENCILREF)        = 57,   /* Reference value used in stencil test */
    MS(D3DRENDERSTATE_STENCILMASK)       = 58,   /* Mask value used in stencil test */
    MS(D3DRENDERSTATE_STENCILWRITEMASK)  = 59,   /* Write mask applied to values written to stencil buffer */
    MS(D3DRENDERSTATE_TEXTUREFACTOR)     = 60,   /* D3DCOLOR used for multi-texture blend */
#endif /* MS__DIRECT3D_VERSION >= 0x0600 */

#if(MS__DIRECT3D_VERSION >= 0x0600)

    /*
     * 128 values [128, 255] are reserved for texture coordinate wrap flags.
     * These are constructed with the D3DWRAP_U and D3DWRAP_V macros. Using
     * a flags uint16_t preserves forward compatibility with texture coordinates
     * that are >2D.
     */
    MS(D3DRENDERSTATE_WRAP0)             = 128,  /* wrap for 1st texture coord. set */
    MS(D3DRENDERSTATE_WRAP1)             = 129,  /* wrap for 2nd texture coord. set */
    MS(D3DRENDERSTATE_WRAP2)             = 130,  /* wrap for 3rd texture coord. set */
    MS(D3DRENDERSTATE_WRAP3)             = 131,  /* wrap for 4th texture coord. set */
    MS(D3DRENDERSTATE_WRAP4)             = 132,  /* wrap for 5th texture coord. set */
    MS(D3DRENDERSTATE_WRAP5)             = 133,  /* wrap for 6th texture coord. set */
    MS(D3DRENDERSTATE_WRAP6)             = 134,  /* wrap for 7th texture coord. set */
    MS(D3DRENDERSTATE_WRAP7)             = 135,  /* wrap for 8th texture coord. set */
#endif /* MS__DIRECT3D_VERSION >= 0x0600 */
#if(MS__DIRECT3D_VERSION >= 0x0700)
    MS(D3DRENDERSTATE_CLIPPING)           = 136,
    MS(D3DRENDERSTATE_LIGHTING)           = 137,
    MS(D3DRENDERSTATE_EXTENTS)            = 138,
    MS(D3DRENDERSTATE_AMBIENT)            = 139,
    MS(D3DRENDERSTATE_FOGVERTEXMODE)      = 140,
    MS(D3DRENDERSTATE_COLORVERTEX)        = 141,
    MS(D3DRENDERSTATE_LOCALVIEWER)        = 142,
    MS(D3DRENDERSTATE_NORMALIZENORMALS)   = 143,
    MS(D3DRENDERSTATE_COLORKEYBLENDENABLE)     = 144,
    MS(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE)   = 145,
    MS(D3DRENDERSTATE_SPECULARMATERIALSOURCE)  = 146,
    MS(D3DRENDERSTATE_AMBIENTMATERIALSOURCE)   = 147,
    MS(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE)  = 148,
    MS(D3DRENDERSTATE_VERTEXBLEND)             = 151,
    MS(D3DRENDERSTATE_CLIPPLANEENABLE)         = 152,

#endif /* MS__DIRECT3D_VERSION >= 0x0700 */

//
// retired renderstates - not supported for DX7 interfaces
//
    MS(D3DRENDERSTATE_TEXTUREHANDLE)     = 1,    /* Texture handle for legacy interfaces (Texture,Texture2) */
    MS(D3DRENDERSTATE_TEXTUREADDRESS)    = 3,    /* D3DTEXTUREADDRESS  */
    MS(D3DRENDERSTATE_WRAPU)             = 5,    /* TRUE for wrapping in u */
    MS(D3DRENDERSTATE_WRAPV)             = 6,    /* TRUE for wrapping in v */
    MS(D3DRENDERSTATE_MONOENABLE)        = 11,   /* TRUE to enable mono rasterization */
    MS(D3DRENDERSTATE_ROP2)              = 12,   /* ROP2 */
    MS(D3DRENDERSTATE_PLANEMASK)         = 13,   /* uint32_t physical plane mask */
    MS(D3DRENDERSTATE_TEXTUREMAG)        = 17,   /* D3DTEXTUREFILTER */
    MS(D3DRENDERSTATE_TEXTUREMIN)        = 18,   /* D3DTEXTUREFILTER */
    MS(D3DRENDERSTATE_TEXTUREMAPBLEND)   = 21,   /* D3DTEXTUREBLEND */
    MS(D3DRENDERSTATE_SUBPIXEL)          = 31,   /* TRUE to enable subpixel correction */
    MS(D3DRENDERSTATE_SUBPIXELX)         = 32,   /* TRUE to enable correction in X only */
    MS(D3DRENDERSTATE_STIPPLEENABLE)     = 39,   /* TRUE to enable stippling */
#if(MS__DIRECT3D_VERSION >= 0x0500)
    MS(D3DRENDERSTATE_BORDERCOLOR)       = 43,   /* Border color for texturing w/border */
    MS(D3DRENDERSTATE_TEXTUREADDRESSU)   = 44,   /* Texture addressing mode for U coordinate */
    MS(D3DRENDERSTATE_TEXTUREADDRESSV)   = 45,   /* Texture addressing mode for V coordinate */
    MS(D3DRENDERSTATE_MIPMAPLODBIAS)     = 46,   /* D3DVALUE Mipmap LOD bias */
    MS(D3DRENDERSTATE_ANISOTROPY)        = 49,   /* Max. anisotropy. 1 = no anisotropy */
#endif /* MS__DIRECT3D_VERSION >= 0x0500 */
    MS(D3DRENDERSTATE_FLUSHBATCH)        = 50,   /* Explicit flush for DP batching (DX5 Only) */
#if(MS__DIRECT3D_VERSION >= 0x0600)
    MS(D3DRENDERSTATE_TRANSLUCENTSORTINDEPENDENT)=51, /* BOOL enable sort-independent transparency */
#endif /* MS__DIRECT3D_VERSION >= 0x0600 */
    MS(D3DRENDERSTATE_STIPPLEPATTERN00)  = 64,   /* Stipple pattern 01...  */
    MS(D3DRENDERSTATE_STIPPLEPATTERN01)  = 65,
    MS(D3DRENDERSTATE_STIPPLEPATTERN02)  = 66,
    MS(D3DRENDERSTATE_STIPPLEPATTERN03)  = 67,
    MS(D3DRENDERSTATE_STIPPLEPATTERN04)  = 68,
    MS(D3DRENDERSTATE_STIPPLEPATTERN05)  = 69,
    MS(D3DRENDERSTATE_STIPPLEPATTERN06)  = 70,
    MS(D3DRENDERSTATE_STIPPLEPATTERN07)  = 71,
    MS(D3DRENDERSTATE_STIPPLEPATTERN08)  = 72,
    MS(D3DRENDERSTATE_STIPPLEPATTERN09)  = 73,
    MS(D3DRENDERSTATE_STIPPLEPATTERN10)  = 74,
    MS(D3DRENDERSTATE_STIPPLEPATTERN11)  = 75,
    MS(D3DRENDERSTATE_STIPPLEPATTERN12)  = 76,
    MS(D3DRENDERSTATE_STIPPLEPATTERN13)  = 77,
    MS(D3DRENDERSTATE_STIPPLEPATTERN14)  = 78,
    MS(D3DRENDERSTATE_STIPPLEPATTERN15)  = 79,
    MS(D3DRENDERSTATE_STIPPLEPATTERN16)  = 80,
    MS(D3DRENDERSTATE_STIPPLEPATTERN17)  = 81,
    MS(D3DRENDERSTATE_STIPPLEPATTERN18)  = 82,
    MS(D3DRENDERSTATE_STIPPLEPATTERN19)  = 83,
    MS(D3DRENDERSTATE_STIPPLEPATTERN20)  = 84,
    MS(D3DRENDERSTATE_STIPPLEPATTERN21)  = 85,
    MS(D3DRENDERSTATE_STIPPLEPATTERN22)  = 86,
    MS(D3DRENDERSTATE_STIPPLEPATTERN23)  = 87,
    MS(D3DRENDERSTATE_STIPPLEPATTERN24)  = 88,
    MS(D3DRENDERSTATE_STIPPLEPATTERN25)  = 89,
    MS(D3DRENDERSTATE_STIPPLEPATTERN26)  = 90,
    MS(D3DRENDERSTATE_STIPPLEPATTERN27)  = 91,
    MS(D3DRENDERSTATE_STIPPLEPATTERN28)  = 92,
    MS(D3DRENDERSTATE_STIPPLEPATTERN29)  = 93,
    MS(D3DRENDERSTATE_STIPPLEPATTERN30)  = 94,
    MS(D3DRENDERSTATE_STIPPLEPATTERN31)  = 95,

//
// retired renderstate names - the values are still used under new naming conventions
//
    MS(D3DRENDERSTATE_FOGTABLESTART)     = 36,   /* Fog table start    */
    MS(D3DRENDERSTATE_FOGTABLEEND)       = 37,   /* Fog table end      */
    MS(D3DRENDERSTATE_FOGTABLEDENSITY)   = 38,   /* Fog table density  */

#if(MS__DIRECT3D_VERSION >= 0x0500)
    MS(D3DRENDERSTATE_FORCE_DWORD)       = 0x7fffffff, /* force 32-bit size enum */
#endif /* MS__DIRECT3D_VERSION >= 0x0500 */
} MS(D3DRENDERSTATETYPE);







typedef uint32_t MS(D3DCOLORMODEL);


typedef struct {
    uint32_t dwSize;
    uint32_t dwMiscCaps;                 /* Capability flags */
    uint32_t dwRasterCaps;
    uint32_t dwZCmpCaps;
    uint32_t dwSrcBlendCaps;
    uint32_t dwDestBlendCaps;
    uint32_t dwAlphaCmpCaps;
    uint32_t dwShadeCaps;
    uint32_t dwTextureCaps;
    uint32_t dwTextureFilterCaps;
    uint32_t dwTextureBlendCaps;
    uint32_t dwTextureAddressCaps;
    uint32_t dwStippleWidth;             /* maximum width and height of */
    uint32_t dwStippleHeight;            /* of supported stipple (up to 32x32) */
} MS(D3DPRIMCAPS);

typedef struct {
    uint32_t dwSize;
    uint32_t dwCaps;                   /* Lighting caps */
    uint32_t dwLightingModel;          /* Lighting model - RGB or mono */
    uint32_t dwNumLights;              /* Number of lights that can be handled */
} MS(D3DLIGHTINGCAPS);

typedef struct {
    uint32_t dwSize;
    uint32_t dwCaps;
} MS(D3DTRANSFORMCAPS);

typedef struct {
    uint32_t             dwSize;                 /* Size of D3DDEVICEDESC structure */
    uint32_t             dwFlags;                /* Indicates which fields have valid data */
    MS(D3DCOLORMODEL)    dcmColorModel;          /* Color model of device */
    uint32_t             dwDevCaps;              /* Capabilities of device */
    MS(D3DTRANSFORMCAPS) dtcTransformCaps;       /* Capabilities of transform */
    uint8_t              bClipping;              /* Device can do 3D clipping */
    MS(D3DLIGHTINGCAPS)  dlcLightingCaps;        /* Capabilities of lighting */
    MS(D3DPRIMCAPS)      dpcLineCaps;
    MS(D3DPRIMCAPS)      dpcTriCaps;
    uint32_t             dwDeviceRenderBitDepth; /* One of DDBB_8, 16, etc.. */
    uint32_t             dwDeviceZBufferBitDepth;/* One of DDBD_16, 32, etc.. */
    uint32_t             dwMaxBufferSize;        /* Maximum execute buffer size */
    uint32_t             dwMaxVertexCount;       /* Maximum vertex count */
#if(MS__DIRECT3D_VERSION >= 0x0500)
    // *** New fields for DX5 *** //

    // Width and height caps are 0 for legacy HALs.
    uint32_t        dwMinTextureWidth, dwMinTextureHeight;
    uint32_t        dwMaxTextureWidth, dwMaxTextureHeight;
    uint32_t        dwMinStippleWidth, dwMaxStippleWidth;
    uint32_t        dwMinStippleHeight, dwMaxStippleHeight;
#endif /* MS__DIRECT3D_VERSION >= 0x0500 */

#if(MS__DIRECT3D_VERSION >= 0x0600)
    // New fields for DX6
    uint32_t        dwMaxTextureRepeat;
    uint32_t        dwMaxTextureAspectRatio;
    uint32_t        dwMaxAnisotropy;

    // Guard band that the rasterizer can accommodate
    // Screen-space vertices inside this space but outside the viewport
    // will get clipped properly.
    MS(D3DVALUE)    dvGuardBandLeft;
    MS(D3DVALUE)    dvGuardBandTop;
    MS(D3DVALUE)    dvGuardBandRight;
    MS(D3DVALUE)    dvGuardBandBottom;

    MS(D3DVALUE)    dvExtentsAdjust;
    uint32_t        dwStencilCaps;

    uint32_t        dwFVFCaps;
    uint32_t        dwTextureOpCaps;
    uint16_t        wMaxTextureBlendStages;
    uint16_t        wMaxSimultaneousTextures;
#endif /* MS__DIRECT3D_VERSION >= 0x0600 */
} MS(D3DDEVICEDESC);

typedef enum { 
  MS(D3DBLEND_ZERO)             = 1,
  MS(D3DBLEND_ONE)              = 2,
  MS(D3DBLEND_SRCCOLOR)         = 3,
  MS(D3DBLEND_INVSRCCOLOR)      = 4,
  MS(D3DBLEND_SRCALPHA)         = 5,
  MS(D3DBLEND_INVSRCALPHA)      = 6,
  MS(D3DBLEND_DESTALPHA)        = 7,
  MS(D3DBLEND_INVDESTALPHA)     = 8,
  MS(D3DBLEND_DESTCOLOR)        = 9,
  MS(D3DBLEND_INVDESTCOLOR)     = 10,
  MS(D3DBLEND_SRCALPHASAT)      = 11,
  MS(D3DBLEND_BOTHSRCALPHA)     = 12,
  MS(D3DBLEND_BOTHINVSRCALPHA)  = 13,
  MS(D3DBLEND_BLENDFACTOR)      = 14,
  MS(D3DBLEND_INVBLENDFACTOR)   = 15,
  MS(D3DBLEND_SRCCOLOR2)        = 16,
  MS(D3DBLEND_INVSRCCOLOR2)     = 17
} MS(D3DBLEND);

#endif
