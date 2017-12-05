#ifndef __OPENSWE1R_COM_DINPUT_H__
#define __OPENSWE1R_COM_DINPUT_H__

#define MS__DIRECTINPUT_VERSION 0x0600

#include "../windows.h"

#include "../export.h"
#include "../emulation.h"

typedef struct {
  const MS(GUID) *pguid;
  MS(DWORD)   dwOfs;
  MS(DWORD)   dwType;
  MS(DWORD)   dwFlags;
} MS(DIOBJECTDATAFORMAT);
typedef Address MS(LPDIOBJECTDATAFORMAT);

typedef struct {
  MS(DWORD)   dwSize;
  MS(DWORD)   dwObjSize;
  MS(DWORD)   dwFlags;
  MS(DWORD)   dwDataSize;
  MS(DWORD)   dwNumObjs;
  MS(LPDIOBJECTDATAFORMAT) rgodf;
} MS(DIDATAFORMAT);

typedef struct {
    MS(DWORD)   dwSize;
    MS(GUID)    guidInstance;
    MS(GUID)    guidProduct;
    MS(DWORD)   dwDevType;
    char    tszInstanceName[MS(MAX_PATH)];
    char    tszProductName[MS(MAX_PATH)];
#if (MS__DIRECTINPUT_VERSION >= 0x0500)
    MS(GUID)    guidFFDriver;
    uint16_t    wUsagePage;
    uint16_t    wUsage;
#endif
} MS(DIDEVICEINSTANCEA);

typedef struct {
    MS(DWORD)       dwOfs;
    MS(DWORD)       dwData;
    MS(DWORD)       dwTimeStamp;
    MS(DWORD)       dwSequence;
#if (MS__DIRECTINPUT_VERSION >= 0x0800)
    MS(UINT_PTR)    uAppData;
#endif
} MS(DIDEVICEOBJECTDATA);


// From Microsoft DX6 SDK headers

enum {
  MS(DIK_ESCAPE) =          0x01,
  MS(DIK_1) =               0x02,
  MS(DIK_2) =               0x03,
  MS(DIK_3) =               0x04,
  MS(DIK_4) =               0x05,
  MS(DIK_5) =               0x06,
  MS(DIK_6) =               0x07,
  MS(DIK_7) =               0x08,
  MS(DIK_8) =               0x09,
  MS(DIK_9) =               0x0A,
  MS(DIK_0) =               0x0B,
  MS(DIK_MINUS) =           0x0C,    /* - on main keyboard */
  MS(DIK_EQUALS) =          0x0D,
  MS(DIK_BACK) =            0x0E,    /* backspace */
  MS(DIK_TAB) =             0x0F,
  MS(DIK_Q) =               0x10,
  MS(DIK_W) =               0x11,
  MS(DIK_E) =               0x12,
  MS(DIK_R) =               0x13,
  MS(DIK_T) =               0x14,
  MS(DIK_Y) =               0x15,
  MS(DIK_U) =               0x16,
  MS(DIK_I) =               0x17,
  MS(DIK_O) =               0x18,
  MS(DIK_P) =               0x19,
  MS(DIK_LBRACKET) =        0x1A,
  MS(DIK_RBRACKET) =        0x1B,
  MS(DIK_RETURN) =          0x1C,    /* Enter on main keyboard */
  MS(DIK_LCONTROL) =        0x1D,
  MS(DIK_A) =               0x1E,
  MS(DIK_S) =               0x1F,
  MS(DIK_D) =               0x20,
  MS(DIK_F) =               0x21,
  MS(DIK_G) =               0x22,
  MS(DIK_H) =               0x23,
  MS(DIK_J) =               0x24,
  MS(DIK_K) =               0x25,
  MS(DIK_L) =               0x26,
  MS(DIK_SEMICOLON) =       0x27,
  MS(DIK_APOSTROPHE) =      0x28,
  MS(DIK_GRAVE) =           0x29,    /* accent grave */
  MS(DIK_LSHIFT) =          0x2A,
  MS(DIK_BACKSLASH) =       0x2B,
  MS(DIK_Z) =               0x2C,
  MS(DIK_X) =               0x2D,
  MS(DIK_C) =               0x2E,
  MS(DIK_V) =               0x2F,
  MS(DIK_B) =               0x30,
  MS(DIK_N) =               0x31,
  MS(DIK_M) =               0x32,
  MS(DIK_COMMA) =           0x33,
  MS(DIK_PERIOD) =          0x34,    /* . on main keyboard */
  MS(DIK_SLASH) =           0x35,    /* / on main keyboard */
  MS(DIK_RSHIFT) =          0x36,
  MS(DIK_MULTIPLY) =        0x37,    /* * on numeric keypad */
  MS(DIK_LMENU) =           0x38,    /* left Alt */
  MS(DIK_SPACE) =           0x39,
  MS(DIK_CAPITAL) =         0x3A,
  MS(DIK_F1) =              0x3B,
  MS(DIK_F2) =              0x3C,
  MS(DIK_F3) =              0x3D,
  MS(DIK_F4) =              0x3E,
  MS(DIK_F5) =              0x3F,
  MS(DIK_F6) =              0x40,
  MS(DIK_F7) =              0x41,
  MS(DIK_F8) =              0x42,
  MS(DIK_F9) =              0x43,
  MS(DIK_F10) =             0x44,
  MS(DIK_NUMLOCK) =         0x45,
  MS(DIK_SCROLL) =          0x46,    /* Scroll Lock */
  MS(DIK_NUMPAD7) =         0x47,
  MS(DIK_NUMPAD8) =         0x48,
  MS(DIK_NUMPAD9) =         0x49,
  MS(DIK_SUBTRACT) =        0x4A,    /* - on numeric keypad */
  MS(DIK_NUMPAD4) =         0x4B,
  MS(DIK_NUMPAD5) =         0x4C,
  MS(DIK_NUMPAD6) =         0x4D,
  MS(DIK_ADD) =             0x4E,    /* + on numeric keypad */
  MS(DIK_NUMPAD1) =         0x4F,
  MS(DIK_NUMPAD2) =         0x50,
  MS(DIK_NUMPAD3) =         0x51,
  MS(DIK_NUMPAD0) =         0x52,
  MS(DIK_DECIMAL) =         0x53,    /* . on numeric keypad */
  MS(DIK_OEM_102) =         0x56,    /* < > | on UK/Germany keyboards */
  MS(DIK_F11) =             0x57,
  MS(DIK_F12) =             0x58,

  MS(DIK_F13) =             0x64,    /*                     (NEC PC98) */
  MS(DIK_F14) =             0x65,    /*                     (NEC PC98) */
  MS(DIK_F15) =             0x66,    /*                     (NEC PC98) */

  MS(DIK_KANA) =            0x70,    /* (Japanese keyboard)            */
  MS(DIK_ABNT_C1) =         0x73,    /* / ? on Portugese (Brazilian) keyboards */
  MS(DIK_CONVERT) =         0x79,    /* (Japanese keyboard)            */
  MS(DIK_NOCONVERT) =       0x7B,    /* (Japanese keyboard)            */
  MS(DIK_YEN) =             0x7D,    /* (Japanese keyboard)            */
  MS(DIK_ABNT_C2) =         0x7E,    /* Numpad . on Portugese (Brazilian) keyboards */
  MS(DIK_NUMPADEQUALS) =    0x8D,    /* = on numeric keypad (NEC PC98) */
  MS(DIK_PREVTRACK) =       0x90,    /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
  MS(DIK_AT) =              0x91,    /*                     (NEC PC98) */
  MS(DIK_COLON) =           0x92,    /*                     (NEC PC98) */
  MS(DIK_UNDERLINE) =       0x93,    /*                     (NEC PC98) */
  MS(DIK_KANJI) =           0x94,    /* (Japanese keyboard)            */
  MS(DIK_STOP) =            0x95,    /*                     (NEC PC98) */
  MS(DIK_AX) =              0x96,    /*                     (Japan AX) */
  MS(DIK_UNLABELED) =       0x97,    /*                        (J3100) */
  MS(DIK_NEXTTRACK) =       0x99,    /* Next Track */
  MS(DIK_NUMPADENTER) =     0x9C,    /* Enter on numeric keypad */
  MS(DIK_RCONTROL) =        0x9D,
  MS(DIK_MUTE) =            0xA0,    /* Mute */
  MS(DIK_CALCULATOR) =      0xA1,    /* Calculator */
  MS(DIK_PLAYPAUSE) =       0xA2,    /* Play / Pause */
  MS(DIK_MEDIASTOP) =       0xA4,    /* Media Stop */
  MS(DIK_VOLUMEDOWN) =      0xAE,    /* Volume - */
  MS(DIK_VOLUMEUP) =        0xB0,    /* Volume + */
  MS(DIK_WEBHOME) =         0xB2,    /* Web home */
  MS(DIK_NUMPADCOMMA) =     0xB3,    /* , on numeric keypad (NEC PC98) */
  MS(DIK_DIVIDE) =          0xB5,    /* / on numeric keypad */
  MS(DIK_SYSRQ) =           0xB7,
  MS(DIK_RMENU) =           0xB8,    /* right Alt */
  MS(DIK_PAUSE) =           0xC5,    /* Pause */
  MS(DIK_HOME) =            0xC7,    /* Home on arrow keypad */
  MS(DIK_UP) =              0xC8,    /* UpArrow on arrow keypad */
  MS(DIK_PRIOR) =           0xC9,    /* PgUp on arrow keypad */
  MS(DIK_LEFT) =            0xCB,    /* LeftArrow on arrow keypad */
  MS(DIK_RIGHT) =           0xCD,    /* RightArrow on arrow keypad */
  MS(DIK_END) =             0xCF,    /* End on arrow keypad */
  MS(DIK_DOWN) =            0xD0,    /* DownArrow on arrow keypad */
  MS(DIK_NEXT) =            0xD1,    /* PgDn on arrow keypad */
  MS(DIK_INSERT) =          0xD2,    /* Insert on arrow keypad */
  MS(DIK_DELETE) =          0xD3,    /* Delete on arrow keypad */
  MS(DIK_LWIN) =            0xDB,    /* Left Windows key */
  MS(DIK_RWIN) =            0xDC,    /* Right Windows key */
  MS(DIK_APPS) =            0xDD,    /* AppMenu key */
  MS(DIK_POWER) =           0xDE,    /* System Power */
  MS(DIK_SLEEP) =           0xDF,    /* System Sleep */
  MS(DIK_WAKE) =            0xE3,    /* System Wake */
  MS(DIK_WEBSEARCH) =       0xE5,    /* Web Search */
  MS(DIK_WEBFAVORITES) =    0xE6,    /* Web Favorites */
  MS(DIK_WEBREFRESH) =      0xE7,    /* Web Refresh */
  MS(DIK_WEBSTOP) =         0xE8,    /* Web Stop */
  MS(DIK_WEBFORWARD) =      0xE9,    /* Web Forward */
  MS(DIK_WEBBACK) =         0xEA,    /* Web Back */
  MS(DIK_MYCOMPUTER) =      0xEB,    /* My Computer */
  MS(DIK_MAIL) =            0xEC,    /* Mail */
  MS(DIK_MEDIASELECT) =     0xED,    /* Media Select */

/*
 *  Alternate names for keys, to facilitate transition from DOS.
 */
  MS(DIK_BACKSPACE) =       MS(DIK_BACK),            /* backspace */
  MS(DIK_NUMPADSTAR) =      MS(DIK_MULTIPLY),        /* * on numeric keypad */
  MS(DIK_LALT) =            MS(DIK_LMENU),           /* left Alt */
  MS(DIK_CAPSLOCK) =        MS(DIK_CAPITAL),         /* CapsLock */
  MS(DIK_NUMPADMINUS) =     MS(DIK_SUBTRACT),        /* - on numeric keypad */
  MS(DIK_NUMPADPLUS) =      MS(DIK_ADD),             /* + on numeric keypad */
  MS(DIK_NUMPADPERIOD) =    MS(DIK_DECIMAL),         /* . on numeric keypad */
  MS(DIK_NUMPADSLASH) =     MS(DIK_DIVIDE),          /* / on numeric keypad */
  MS(DIK_RALT) =            MS(DIK_RMENU),           /* right Alt */
  MS(DIK_UPARROW) =         MS(DIK_UP),              /* UpArrow on arrow keypad */
  MS(DIK_PGUP) =            MS(DIK_PRIOR),           /* PgUp on arrow keypad */
  MS(DIK_LEFTARROW) =       MS(DIK_LEFT),            /* LeftArrow on arrow keypad */
  MS(DIK_RIGHTARROW) =      MS(DIK_RIGHT),           /* RightArrow on arrow keypad */
  MS(DIK_DOWNARROW) =       MS(DIK_DOWN),            /* DownArrow on arrow keypad */
  MS(DIK_PGDN) =            MS(DIK_NEXT),            /* PgDn on arrow keypad */

/*
 *  Alternate names for keys originally not used on US keyboards.
 */
  MS(DIK_CIRCUMFLEX) =      MS(DIK_PREVTRACK)        /* Japanese keyboard */
};

#endif
