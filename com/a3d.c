// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include "../main.h"
#include "../windows.h"

#include <inttypes.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "al.h"
#include "alc.h"

static void AL_APIENTRY alc_DeferUpdatesSOFT(void) {
  alcSuspendContext(alcGetCurrentContext());
}

static void AL_APIENTRY alc_ProcessUpdatesSOFT(void) {
  alcProcessContext(alcGetCurrentContext());
}

static void(AL_APIENTRY *_DeferUpdatesSOFT)(void) = NULL;
static void(AL_APIENTRY *_ProcessUpdatesSOFT)(void) = NULL;

typedef struct {
  void* vtable;
  ALCdevice *device;
  ALCcontext *context;
} A3d4;


typedef struct {
  uint16_t wFormatTag;
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
  uint16_t cbSize;
} API(WAVEFORMATEX);

typedef struct {
  void* vtable;
  ALuint al_source;
  ALuint al_buffer;
  uint32_t type;
  API(WAVEFORMATEX) fmt;
  Address data;
  uint32_t event_offset;
  uint32_t event_handle;
} A3DSOURCE;

typedef struct {
   API(DWORD) dwSize;        // Use for internal version control
   API(DWORD) dwFlags;
   API(DWORD) dwReserved;
   API(DWORD) dwReserved2;
   API(DWORD) dwOutputChannels;
   API(DWORD) dwMinSampleRate;
   API(DWORD) dwMaxSampleRate;
   API(DWORD) dwMax2DBuffers;
   API(DWORD) dwMax3DBuffers;
} API(A3DCAPS_HARDWARE);

#if 0
DECLARE_INTERFACE_(IA3d4, IUnknown)
{
	// IUnknown Methods.
	STDMETHOD(QueryInterface)				(THIS_ REFIID, LPVOID FAR *) PURE; // 0
	STDMETHOD_(ULONG,AddRef)				(THIS) PURE; // 1
	STDMETHOD_(ULONG,Release)				(THIS) PURE; // 2

	// IA3d Methods.
	STDMETHOD(SetOutputMode)				(THIS_ DWORD, DWORD, DWORD) PURE; // 3
	STDMETHOD(GetOutputMode)				(THIS_ LPDWORD, LPDWORD, LPDWORD) PURE; // 4
	STDMETHOD(SetResourceManagerMode)		(THIS_ DWORD) PURE; // 5
	STDMETHOD(GetResourceManagerMode)		(THIS_ LPDWORD) PURE; // 6
	STDMETHOD(SetHFAbsorbFactor)			(THIS_ FLOAT) PURE; // 7
	STDMETHOD(GetHFAbsorbFactor)			(THIS_ FLOAT *) PURE; // 8

	// IA3d2 Methods.
	STDMETHOD(RegisterVersion)				(THIS_ DWORD) PURE; // 9
	STDMETHOD(GetSoftwareCaps)				(THIS_ LPA3DCAPS_SOFTWARE) PURE; // 10
	STDMETHOD(GetHardwareCaps)				(THIS_ LPA3DCAPS_HARDWARE) PURE; // 11

	// IA3d3 Methods.
	STDMETHOD(Clear)						(THIS) PURE; // 12
	STDMETHOD(Flush)						(THIS) PURE; // 13
	STDMETHOD(Compat)						(THIS_ DWORD, DWORD) PURE; // 14
	STDMETHOD(Init)							(THIS_ LPGUID, DWORD, DWORD) PURE; // 15
	STDMETHOD(IsFeatureAvailable)			(THIS_ DWORD) PURE; // 16
	STDMETHOD(NewSource)					(THIS_ DWORD, LPA3DSOURCE *) PURE; // 17
	STDMETHOD(DuplicateSource)				(THIS_ LPA3DSOURCE, LPA3DSOURCE *) PURE; // 18
	STDMETHOD(SetCooperativeLevel)			(THIS_ HWND, DWORD) PURE; // 19
	STDMETHOD(GetCooperativeLevel)			(THIS_ LPDWORD) PURE; // 20
	STDMETHOD(SetMaxReflectionDelayTime)	(THIS_ A3DVAL) PURE; // 21
	STDMETHOD(GetMaxReflectionDelayTime)	(THIS_ LPA3DVAL) PURE; // 22
	STDMETHOD(SetCoordinateSystem)			(THIS_ DWORD) PURE; // 23
	STDMETHOD(GetCoordinateSystem)			(THIS_ LPDWORD) PURE; // 24
	STDMETHOD(SetOutputGain)				(THIS_ A3DVAL) PURE; // 25
	STDMETHOD(GetOutputGain)				(THIS_ LPA3DVAL) PURE; // 26

	// IA3d4 Methods
	STDMETHOD(SetNumFallbackSources)		(THIS_ DWORD) PURE; // 27
	STDMETHOD(GetNumFallbackSources)		(THIS_ LPDWORD) PURE;	// 28
	STDMETHOD(SetRMPriorityBias)			(THIS_ A3DVAL) PURE; // 29
	STDMETHOD(GetRMPriorityBias)			(THIS_ LPA3DVAL) PURE; // 30
	STDMETHOD(DisableViewer)				(THIS) PURE; // 31
	STDMETHOD(SetUnitsPerMeter) 			(THIS_ A3DVAL) PURE; // 32
	STDMETHOD(GetUnitsPerMeter)				(THIS_ LPA3DVAL) PURE; // 33
	STDMETHOD(SetDopplerScale)				(THIS_ A3DVAL) PURE; // 34
	STDMETHOD(GetDopplerScale)				(THIS_ LPA3DVAL) PURE; // 35
	STDMETHOD(SetDistanceModelScale)		(THIS_ A3DVAL) PURE; // 36
	STDMETHOD(GetDistanceModelScale)		(THIS_ LPA3DVAL) PURE; // 37
	STDMETHOD(SetEq)						(THIS_ A3DVAL) PURE; // 38
	STDMETHOD(GetEq)						(THIS_ LPA3DVAL) PURE; // 39
	STDMETHOD(Shutdown)						(THIS) PURE; // 40
	STDMETHOD(RegisterApp)					(THIS_ REFIID) PURE; // 41
};

DECLARE_INTERFACE_(IA3dListener, IUnknown)
{
	// IUnknown Methods.
	STDMETHOD(QueryInterface)		(THIS_ REFIID, LPVOID FAR *) PURE; // 0
	STDMETHOD_(ULONG,AddRef)		(THIS) PURE; // 1
	STDMETHOD_(ULONG,Release)		(THIS) PURE; // 2

	// IA3dListener Methods.
	STDMETHOD(SetPosition3f)		(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 3
	STDMETHOD(GetPosition3f)		(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 4
	STDMETHOD(SetPosition3fv)		(THIS_ LPA3DVAL) PURE; // 5
	STDMETHOD(GetPosition3fv)		(THIS_ LPA3DVAL) PURE; // 6
	STDMETHOD(SetOrientationAngles3f)	(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 7
	STDMETHOD(GetOrientationAngles3f)	(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 8
	STDMETHOD(SetOrientationAngles3fv)	(THIS_ LPA3DVAL) PURE; // 9
	STDMETHOD(GetOrientationAngles3fv)	(THIS_ LPA3DVAL) PURE; // 10
	STDMETHOD(SetOrientation6f)		(THIS_ A3DVAL, A3DVAL, A3DVAL, A3DVAL, A3DVAL, A3DVAL) PURE; // 11
	STDMETHOD(GetOrientation6f)		(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL, LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 12
	STDMETHOD(SetOrientation6fv)	(THIS_ LPA3DVAL) PURE; // 13
	STDMETHOD(GetOrientation6fv)	(THIS_ LPA3DVAL) PURE; // 14
	STDMETHOD(SetVelocity3f)		(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 15
	STDMETHOD(GetVelocity3f)		(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 16
	STDMETHOD(SetVelocity3fv)		(THIS_ LPA3DVAL) PURE; // 17
	STDMETHOD(GetVelocity3fv)		(THIS_ LPA3DVAL) PURE; // 18
};

DECLARE_INTERFACE_(IA3dSource, IUnknown)
{
	// IUnknown Methods.
	STDMETHOD(QueryInterface)			(THIS_ REFIID, LPVOID FAR *) PURE; // 0
	STDMETHOD_(ULONG,AddRef)			(THIS) PURE; // 1
	STDMETHOD_(ULONG,Release)			(THIS) PURE; // 2

	// IA3dSource Methods.
	STDMETHOD(LoadWaveFile)				(THIS_ LPSTR) PURE; // 3
	STDMETHOD(LoadWaveData)				(THIS_ LPVOID, DWORD) PURE; // 4
	STDMETHOD(AllocateWaveData)			(THIS_ INT) PURE; // 5
	STDMETHOD(FreeWaveData)				(THIS) PURE; // 6
	STDMETHOD(SetWaveFormat)			(THIS_ LPVOID) PURE; // 7
	STDMETHOD(GetWaveFormat)			(THIS_ LPVOID) PURE; // 8
	STDMETHOD(GetWaveSize)				(THIS) PURE; // 9
	STDMETHOD(GetType)					(THIS_ LPDWORD) PURE; // 10
	STDMETHOD(Lock)						(THIS_ DWORD, DWORD, LPVOID *, LPDWORD, LPVOID *, LPDWORD, DWORD) PURE; // 11
	STDMETHOD(Unlock)					(THIS_ LPVOID, DWORD, LPVOID, DWORD) PURE; // 12
	STDMETHOD(Play)						(THIS_ INT) PURE; // 13
	STDMETHOD(Stop)						(THIS) PURE; // 14
	STDMETHOD(Rewind)					(THIS) PURE; // 15
	STDMETHOD(SetWaveTime)				(THIS_ A3DVAL) PURE; // 16
	STDMETHOD(GetWaveTime)				(THIS_ LPA3DVAL) PURE; // 17
	STDMETHOD(SetWavePosition)			(THIS_ DWORD) PURE; // 18
	STDMETHOD(GetWavePosition)			(THIS_ LPDWORD) PURE; // 19
	STDMETHOD(SetPosition3f)			(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 20
	STDMETHOD(GetPosition3f)			(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 21
	STDMETHOD(SetPosition3fv)			(THIS_ LPA3DVAL) PURE; // 22
	STDMETHOD(GetPosition3fv)			(THIS_ LPA3DVAL) PURE; // 23
	STDMETHOD(SetOrientationAngles3f)	(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 24
	STDMETHOD(GetOrientationAngles3f)	(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 25
	STDMETHOD(SetOrientationAngles3fv)	(THIS_ LPA3DVAL) PURE; // 26
	STDMETHOD(GetOrientationAngles3fv)	(THIS_ LPA3DVAL) PURE; // 27
 	STDMETHOD(SetOrientation6f)			(THIS_ A3DVAL, A3DVAL, A3DVAL, A3DVAL, A3DVAL, A3DVAL) PURE; // 28
	STDMETHOD(GetOrientation6f)			(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL, LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 29
	STDMETHOD(SetOrientation6fv)		(THIS_ LPA3DVAL) PURE; // 30
	STDMETHOD(GetOrientation6fv)		(THIS_ LPA3DVAL) PURE; // 31
	STDMETHOD(SetVelocity3f)			(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 32
	STDMETHOD(GetVelocity3f)			(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 33
	STDMETHOD(SetVelocity3fv)			(THIS_ LPA3DVAL) PURE; // 34
	STDMETHOD(GetVelocity3fv)			(THIS_ LPA3DVAL) PURE; // 35
	STDMETHOD(SetCone)					(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 36
	STDMETHOD(GetCone)					(THIS_ LPA3DVAL, LPA3DVAL, LPA3DVAL) PURE; // 37
	STDMETHOD(SetMinMaxDistance)		(THIS_ A3DVAL, A3DVAL, DWORD) PURE; // 38
	STDMETHOD(GetMinMaxDistance)		(THIS_ LPA3DVAL, LPA3DVAL, LPDWORD) PURE; // 39
	STDMETHOD(SetGain)					(THIS_ A3DVAL) PURE; // 40
	STDMETHOD(GetGain)					(THIS_ LPA3DVAL) PURE; // 41
	STDMETHOD(SetPitch)					(THIS_ A3DVAL) PURE; // 42
	STDMETHOD(GetPitch)					(THIS_ LPA3DVAL) PURE; // 43
	STDMETHOD(SetDopplerScale)			(THIS_ A3DVAL) PURE; // 44
	STDMETHOD(GetDopplerScale)			(THIS_ LPA3DVAL) PURE; // 45
	STDMETHOD(SetDistanceModelScale)	(THIS_ A3DVAL) PURE; // 46
	STDMETHOD(GetDistanceModelScale)	(THIS_ LPA3DVAL) PURE; // 47
	STDMETHOD(SetEq)					(THIS_ A3DVAL) PURE; // 48
	STDMETHOD(GetEq)					(THIS_ LPA3DVAL) PURE; // 49
	STDMETHOD(SetPriority)				(THIS_ A3DVAL) PURE; // 50
	STDMETHOD(GetPriority)				(THIS_ LPA3DVAL) PURE; // 51
	STDMETHOD(SetRenderMode)			(THIS_ DWORD) PURE; // 52
	STDMETHOD(GetRenderMode)			(THIS_ LPDWORD) PURE; // 53
	STDMETHOD(GetAudibility)			(THIS_ LPA3DVAL) PURE; // 54
	STDMETHOD(GetOcclusionFactor)		(THIS_ LPA3DVAL) PURE; // 55
    STDMETHOD(GetStatus)				(THIS_ LPDWORD) PURE; // 56
    STDMETHOD(SetPanValues)				(THIS_ DWORD, LPA3DVAL) PURE; // 57
    STDMETHOD(GetPanValues)				(THIS_ DWORD, LPA3DVAL) PURE; // 58
	STDMETHOD(SetWaveEvent)				(THIS_ DWORD, HANDLE) PURE; // 59
	STDMETHOD(ClearWaveEvents)			(THIS) PURE; // 60
	STDMETHOD(SetTransformMode)			(THIS_ DWORD) PURE; // 61
	STDMETHOD(GetTransformMode)			(THIS_ LPDWORD) PURE; // 62
	STDMETHOD(SetReflectionDelayScale)	(THIS_ A3DVAL) PURE; // 63
	STDMETHOD(GetReflectionDelayScale)	(THIS_ LPA3DVAL) PURE; // 64
	STDMETHOD(SetReflectionGainScale)	(THIS_ A3DVAL) PURE; // 65
	STDMETHOD(GetReflectionGainScale)	(THIS_ LPA3DVAL) PURE; // 66
};


#endif


// Implementation

// IA3d4 -> STDMETHOD(QueryInterface)				(THIS_ REFIID, LPVOID FAR *) PURE; // 0
HACKY_COM_BEGIN(IA3d4, 0)
  hacky_printf("QueryInterface\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);

  const API(IID)* iid = (const API(IID)*)Memory(stack[2]);

  char iidString[1024];
  sprintf(iidString, "%08" PRIX32 "-%04" PRIX16 "-%04" PRIX16 "-%02" PRIX8 "%02" PRIX8 "-%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8,
          iid->Data1, iid->Data2, iid->Data3,
          iid->Data4[0], iid->Data4[1], iid->Data4[2], iid->Data4[3],
          iid->Data4[4], iid->Data4[5], iid->Data4[6], iid->Data4[7]);
  printf("  (read iid: {%s})\n", iidString);

  char name[32];
  //FIXME: Add more classed / interfaces

  if (!strcmp(iidString, "C398E563-D90B-11D1-90FB-006008A1F441")) {
    strcpy(name, "IA3dListener");
  } else {
    assert(false);
  }
  
  *(Address*)Memory(stack[3]) = CreateInterface(name, 200);

  eax = 0;
  esp += 3 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD_(ULONG,Release)			(THIS) PURE; // 2
HACKY_COM_BEGIN(IA3d4, 2)
  hacky_printf("Release\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  //FIXME: Implement
  eax = 0;
  esp += 1 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(GetHardwareCaps)				(THIS_ LPA3DCAPS_HARDWARE) PURE; // 11
HACKY_COM_BEGIN(IA3d4, 11)
  hacky_printf("GetHardwareCaps\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  //FIXME: Implement
  API(A3DCAPS_HARDWARE)* caps = (API(A3DCAPS_HARDWARE)*)Memory(stack[2]);
  assert(caps->dwSize == sizeof(API(A3DCAPS_HARDWARE)));

 
// Feature flags (FIXME: Check which of those are checked by the game)
enum {
  API(A3D_1ST_REFLECTIONS) =      0x00000002,
  API(A3D_DIRECT_PATH_A3D) =      0x00000008,
  API(A3D_DIRECT_PATH_GENERIC) =  0x00000020,
  API(A3D_OCCLUSIONS) =           0x00000040,
  API(A3D_DISABLE_SPLASHSCREEN) = 0x00000080,
  API(A3D_REVERB) =               0x00000100,
  API(A3D_GEOMETRIC_REVERB) =     0x00000200,
  API(A3D_DISABLE_FOCUS_MUTE) =   0x00000400
};

  caps->dwFlags = API(A3D_1ST_REFLECTIONS); // | API(A3D_DIRECT_PATH_A3D);
  caps->dwOutputChannels = 2;
  caps->dwMinSampleRate = 0;
  caps->dwMaxSampleRate = 96000;
  caps->dwMax2DBuffers = 128;
  caps->dwMax3DBuffers = 128;

  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(Flush)						(THIS) PURE; // 13
HACKY_COM_BEGIN(IA3d4, 13)
  hacky_printf("Flush\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);

  _ProcessUpdatesSOFT();
  _DeferUpdatesSOFT();

  eax = 0;
  esp += 1 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(Init)							(THIS_ LPGUID, DWORD, DWORD) PURE; // 15
HACKY_COM_BEGIN(IA3d4, 15)
  hacky_printf("Init\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);

  A3d4* this = Memory(stack[1]);

  this->device = alcOpenDevice(NULL);
  assert(this->device != NULL);

  this->context = alcCreateContext(this->device, NULL);

  //FIXME: Do this on every call which uses this context instead
  if (!alcMakeContextCurrent(this->context)) {
    assert(false);
  }

  if (alcIsExtensionPresent(this->device, "AL_SOFT_deferred_updates") == ALC_TRUE) {
    _DeferUpdatesSOFT = alcGetProcAddress(this->device, "DeferUpdatesSOFT");
    _ProcessUpdatesSOFT = alcGetProcAddress(this->device, "ProcessUpdatesSOFT");
  } else {
    _DeferUpdatesSOFT = alc_DeferUpdatesSOFT;
    _ProcessUpdatesSOFT = alc_ProcessUpdatesSOFT;
    printf("AL_SOFT_deferred_updates not present, using fallback\n");
  }

  // Stop updating, so we must force IA3d4::Flush
  _DeferUpdatesSOFT();

  eax = 0;
  esp += 4 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(NewSource)					(THIS_ DWORD, LPA3DSOURCE *) PURE; // 17
HACKY_COM_BEGIN(IA3d4, 17)
  hacky_printf("NewSource\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);

  Address addr = CreateInterface("IA3dSource", 200);
  
  A3DSOURCE* source = Memory(addr);
  alGenSources(1, &source->al_source);
  alGenBuffers(1, &source->al_buffer);
  source->type = stack[2];

  source->event_offset = 0;
  source->event_handle = 0;

//FIXME: Move these to proper functions, unless we need defaults

alSourcef(source->al_source, AL_PITCH, 1);
// check for errors
alSourcef(source->al_source, AL_GAIN, 1);
// check for errors
alSource3f(source->al_source, AL_POSITION, 0, 0, 0);
// check for errors
alSource3f(source->al_source, AL_VELOCITY, 0, 0, 0);
// check for errors
alSourcei(source->al_source, AL_LOOPING, AL_FALSE);

  *(Address*)Memory(stack[3]) = addr;  

  eax = 0;
  esp += 3 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(DuplicateSource)				(THIS_ LPA3DSOURCE, LPA3DSOURCE *) PURE; // 18
HACKY_COM_BEGIN(IA3d4, 18)
  hacky_printf("DuplicateSource\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);

  Address addr = CreateInterface("IA3dSource", 200);
  A3DSOURCE* src = Memory(stack[2]);
  
  //FIXME: don't share vtable?
  A3DSOURCE* source = Memory(addr);
  Address vtable = source->vtable;
  memcpy(source, src, sizeof(A3DSOURCE));
  alGenSources(1, &source->al_source);
  source->vtable = vtable;

  *(Address*)Memory(stack[3]) = addr;  

  eax = 0;
  esp += 3 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetCooperativeLevel)			(THIS_ HWND, DWORD) PURE; // 19
HACKY_COM_BEGIN(IA3d4, 19)
  hacky_printf("SetCooperativeLevel\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  //FIXME: Implement
  eax = 0;
  esp += 3 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetCoordinateSystem)			(THIS_ DWORD) PURE; // 23
HACKY_COM_BEGIN(IA3d4, 23)
  hacky_printf("SetCoordinateSystem\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  //FIXME: Implement
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetOutputGain)				(THIS_ A3DVAL) PURE; // 25
HACKY_COM_BEGIN(IA3d4, 25)
  hacky_printf("SetOutputGain\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  float a = *(float*)&stack[2];
  hacky_printf("a %f\n", a);
  alListenerf(AL_GAIN, a);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(GetOutputGain)				(THIS_ LPA3DVAL) PURE; // 26
HACKY_COM_BEGIN(IA3d4, 26)
  hacky_printf("GetOutputGain\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  float f;
  alGetListenerf(AL_GAIN, &f);
  *(float*)Memory(stack[2]) = f;
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetNumFallbackSources)		(THIS_ DWORD) PURE; // 27
HACKY_COM_BEGIN(IA3d4, 27)
  hacky_printf("SetNumFallbackSources\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  //FIXME: Implement
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetUnitsPerMeter) 			(THIS_ A3DVAL) PURE; // 32
HACKY_COM_BEGIN(IA3d4, 32)
  hacky_printf("SetUnitsPerMeter\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  //FIXME: Implement
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetDopplerScale)				(THIS_ A3DVAL) PURE; // 34
HACKY_COM_BEGIN(IA3d4, 34)
  hacky_printf("SetDopplerScale\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  //FIXME: Implement
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetDistanceModelScale)		(THIS_ A3DVAL) PURE; // 36
HACKY_COM_BEGIN(IA3d4, 36)
  hacky_printf("SetDistanceModelScale\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  //FIXME: Implement
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3d4 -> STDMETHOD(SetEq)						(THIS_ A3DVAL) PURE; // 38
HACKY_COM_BEGIN(IA3d4, 38)
  hacky_printf("SetEq\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  //FIXME: Implement
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD_(ULONG,Release)			(THIS) PURE; // 2
HACKY_COM_BEGIN(IA3dSource, 2)
  hacky_printf("Release\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  //FIXME: Implement
  eax = 0;
  esp += 1 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(AllocateWaveData)			(THIS_ INT) PURE; // 5
HACKY_COM_BEGIN(IA3dSource, 5)
  hacky_printf("AllocateWaveData\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);

  // a = size?
  A3DSOURCE* this = Memory(stack[1]);
  this->data = Allocate(stack[2]);

  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetWaveFormat)			(THIS_ LPVOID) PURE; // 7
HACKY_COM_BEGIN(IA3dSource, 7)
  hacky_printf("SetWaveFormat\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);

  A3DSOURCE* this = Memory(stack[1]);
  memcpy(&this->fmt, Memory(stack[2]), sizeof(API(WAVEFORMATEX)));

  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(GetType)					(THIS_ LPDWORD) PURE; // 10
HACKY_COM_BEGIN(IA3dSource, 10)
  hacky_printf("GetType\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  A3DSOURCE* this = Memory(stack[1]);
  *(uint32_t*)Memory(stack[2]) = this->type;
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(Lock)						(THIS_ DWORD, DWORD, LPVOID *, LPDWORD, LPVOID *, LPDWORD, DWORD) PURE; // 11
HACKY_COM_BEGIN(IA3dSource, 11)
  hacky_printf("Lock\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("dwWriteCursor 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("dwNumBytes 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("pvAudioPtr1 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("dwAudioBytes1 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("pvAudioPtr2 0x%" PRIX32 "\n", stack[6]);
  hacky_printf("dwAudioBytes2 0x%" PRIX32 "\n", stack[7]);
  hacky_printf("dwFlags 0x%" PRIX32 "\n", stack[8]);

  assert(stack[8] == 0);

  A3DSOURCE* this = Memory(stack[1]);

  //FIXME: assert that the requested length is shorter than the buffer len etc.
  *(Address*)Memory(stack[4]) = this->data;
  *(uint32_t*)Memory(stack[5]) = stack[3];

  // Check if we can write a second buffer
  if (stack[6] && stack[7]) {
    *(Address*)Memory(stack[6]) = 0;
    *(uint32_t*)Memory(stack[7]) = 0;
  }

  eax = 0;
  esp += 8 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(Unlock)					(THIS_ LPVOID, DWORD, LPVOID, DWORD) PURE; // 12
HACKY_COM_BEGIN(IA3dSource, 12)
  hacky_printf("Unlock\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("d 0x%" PRIX32 "\n", stack[5]);

  A3DSOURCE* this = Memory(stack[1]);
  ALenum al_format; 
  if (this->fmt.nChannels == 1) {
    if (this->fmt.wBitsPerSample == 8) {
      al_format = AL_FORMAT_MONO8;
    } else if (this->fmt.wBitsPerSample == 16) {
      al_format = AL_FORMAT_MONO16;
    } else {
      assert(false);
    }
  } else if (this->fmt.nChannels == 2) {
    if (this->fmt.wBitsPerSample == 8) {
      al_format = AL_FORMAT_STEREO8;
    } else if (this->fmt.wBitsPerSample == 16) {
      al_format = AL_FORMAT_STEREO16;
    } else {
      assert(false);
    }
  } else {
    assert(false);
  }

  assert(this->fmt.wFormatTag == 0x0001);
  assert(this->fmt.nBlockAlign == (this->fmt.nChannels * this->fmt.wBitsPerSample / 8));

  alBufferData(this->al_buffer, al_format, Memory(stack[2]), stack[3], this->fmt.nSamplesPerSec);

  //FIXME: assert that this source isn't already playing etc.
  alSourcei(this->al_source, AL_BUFFER, this->al_buffer);

  assert(stack[4] == 0);
  assert(stack[5] == 0);

  eax = 0;
  esp += 5 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(Play)						(THIS_ INT) PURE; // 13
HACKY_COM_BEGIN(IA3dSource, 13)
  hacky_printf("Play\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  A3DSOURCE* this = Memory(stack[1]);
  //FIXME: Set looping
  ALint state;
  alGetSourcei(this->al_source, AL_SOURCE_STATE, &state);
  alSourcei(this->al_source, AL_LOOPING, stack[2] ? AL_TRUE : AL_FALSE);
  if (state != AL_PLAYING) {
    alSourcePlay(this->al_source);
  }
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(Stop)						(THIS) PURE; // 14
HACKY_COM_BEGIN(IA3dSource, 14)
  hacky_printf("Stop\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  A3DSOURCE* this = Memory(stack[1]);
  alSourceStop(this->al_source);
  eax = 0;
  esp += 1 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(Rewind)					(THIS) PURE; // 15
HACKY_COM_BEGIN(IA3dSource, 15)
  hacky_printf("Rewind\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  A3DSOURCE* this = Memory(stack[1]);
  alSourcei(this->al_source, AL_BYTE_OFFSET, 0);
  eax = 0;
  esp += 1 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetPosition3f)			(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 20
HACKY_COM_BEGIN(IA3dSource, 20)
  hacky_printf("SetPosition3f\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  float a = *(float*)&stack[2];
  float b = *(float*)&stack[3];
  float c = *(float*)&stack[4];
  hacky_printf("a %f\n", a);
  hacky_printf("b %f\n", b);
  hacky_printf("c %f\n", c);
  A3DSOURCE* this = Memory(stack[1]);
  alSource3f(this->al_source, AL_POSITION, a, b, c);
  eax = 0;
  esp += 4 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetVelocity3f)			(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 32
HACKY_COM_BEGIN(IA3dSource, 32)
  hacky_printf("SetVelocity3f\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  float a = *(float*)&stack[2];
  float b = *(float*)&stack[3];
  float c = *(float*)&stack[4];
  hacky_printf("a %f\n", a);
  hacky_printf("b %f\n", b);
  hacky_printf("c %f\n", c);
  A3DSOURCE* this = Memory(stack[1]);
  alSource3f(this->al_source, AL_VELOCITY, a, b, c);
  eax = 0;
  esp += 4 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetMinMaxDistance)		(THIS_ A3DVAL, A3DVAL, DWORD) PURE; // 38
HACKY_COM_BEGIN(IA3dSource, 38)
  hacky_printf("SetMinMaxDistance\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  float a = *(float*)&stack[2];
  float b = *(float*)&stack[3];
  hacky_printf("a %f\n", a);
  hacky_printf("b %f\n", b);
  hacky_printf("c 0x%" PRIX32 "\n", stack[4]);

  A3DSOURCE* this = Memory(stack[1]);

enum { // MaxMinDistance flags
  API(A3D_AUDIBLE) = 0x00000000,
  API(A3D_MUTE) =    0x00000001
};

  alSourcef(this->al_source, AL_REFERENCE_DISTANCE, a);
  alSourcef(this->al_source, AL_MAX_DISTANCE, b);

  if (stack[4] & API(A3D_MUTE)) {
    alSourcef(this->al_source, AL_MAX_GAIN, 0.0f);
  } else {
    alSourcef(this->al_source, AL_MAX_GAIN, 1.0f);
  }

  eax = 0;
  esp += 4 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetGain)					(THIS_ A3DVAL) PURE; // 40
HACKY_COM_BEGIN(IA3dSource, 40)
  hacky_printf("SetGain\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  float a = *(float*)&stack[2];
  hacky_printf("a %f\n", a);
  A3DSOURCE* this = Memory(stack[1]);

  //FIXME: This is a hack to disable streaming audio
  if (this->event_handle != 0) {
    

    //FIXME: the game only seems to set this to very silent values currently. probably a bug elsewhere..
  //  alSourcef(this->al_source, AL_GAIN, a);

  }

  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetPitch)					(THIS_ A3DVAL) PURE; // 42
HACKY_COM_BEGIN(IA3dSource, 42)
  hacky_printf("SetPitch\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  float a = *(float*)&stack[2];
  hacky_printf("a %f\n", a);
  A3DSOURCE* this = Memory(stack[1]);
  alSourcef(this->al_source, AL_PITCH, a);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetDopplerScale)			(THIS_ A3DVAL) PURE; // 44
HACKY_COM_BEGIN(IA3dSource, 44)
  hacky_printf("SetDopplerScale\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  float a = *(float*)&stack[2];
  hacky_printf("a %f\n", a);
  A3DSOURCE* this = Memory(stack[1]);
  //FIXME: Implement
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetRenderMode)			(THIS_ DWORD) PURE; // 52
HACKY_COM_BEGIN(IA3dSource, 52)
  hacky_printf("SetRenderMode\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(GetRenderMode)			(THIS_ LPDWORD) PURE; // 53
HACKY_COM_BEGIN(IA3dSource, 53)
  hacky_printf("GetRenderMode\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);

  //FIXME: Some functions check if this is a native source (0x20)
  *(uint32_t*)Memory(stack[2]) = 0;

  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(GetStatus)				(THIS_ LPDWORD) PURE; // 56
HACKY_COM_BEGIN(IA3dSource, 56)
  hacky_printf("GetStatus\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);

  A3DSOURCE* this = Memory(stack[1]);

enum {
  API(A3DSTATUS_PLAYING) =           0x00000001,
  API(A3DSTATUS_BUFFERLOST) =        0x00000002,
  API(A3DSTATUS_LOOPING) =           0x00000004,
  API(A3DSTATUS_WAITING_FOR_FLUSH) = 0x00001000
};

  ALint state;
  alGetSourcei(this->al_source, AL_SOURCE_STATE, &state);

  switch(state) {
  case AL_INITIAL:
  case AL_PAUSED:
  case AL_STOPPED:
    *(uint32_t*)Memory(stack[2]) = 0; // ?
    break;
  case AL_PLAYING: {
    ALint looping;
    alGetSourcei(this->al_source, AL_LOOPING, &looping);
    *(uint32_t*)Memory(stack[2]) = looping ?  API(A3DSTATUS_LOOPING) : API(A3DSTATUS_PLAYING);
    break;
  }
  default:
    assert(false);
    break;
  }

  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetPanValues)				(THIS_ DWORD, LPA3DVAL) PURE; // 57
HACKY_COM_BEGIN(IA3dSource, 57)
  hacky_printf("SetPanValues\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  uint32_t a = stack[2];
  hacky_printf("a 0x%" PRIX32 "\n", a);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);

  float* b = Memory(stack[3]);
  for(unsigned int i = 0; i < a; i++) {
    printf("  PanValues[%d] = %f\n", i, b[i]);
  }
  //FIXME: Implement
  
  eax = 0;
  esp += 3 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetWaveEvent)				(THIS_ DWORD, HANDLE) PURE; // 59
HACKY_COM_BEGIN(IA3dSource, 59)
  hacky_printf("SetWaveEvent\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("b 0x%" PRIX32 "\n", stack[3]);
  
  A3DSOURCE* this = Memory(stack[1]);
  this->event_offset = stack[2];
  this->event_handle = stack[3];

  //FIXME: This is a hack to disable streaming audio
  alSourcef(this->al_source, AL_GAIN, 0.0f);

  eax = 0;
  esp += 3 * 4;
HACKY_COM_END()

// IA3dSource -> STDMETHOD(SetTransformMode)			(THIS_ DWORD) PURE; // 61
HACKY_COM_BEGIN(IA3dSource, 61)
  hacky_printf("SetTransformMode\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("a 0x%" PRIX32 "\n", stack[2]);
  eax = 0;
  esp += 2 * 4;
HACKY_COM_END()



// IA3dListener -> STDMETHOD_(ULONG,Release)			(THIS) PURE; // 2
HACKY_COM_BEGIN(IA3dListener, 2)
  hacky_printf("Release\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  //FIXME: Implement
  eax = 0;
  esp += 1 * 4;
HACKY_COM_END()

// IA3dListener -> STDMETHOD(SetPosition3f)		(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 3
HACKY_COM_BEGIN(IA3dListener, 3)
  hacky_printf("SetPosition3f\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  float a = *(float*)&stack[2];
  float b = *(float*)&stack[3];
  float c = *(float*)&stack[4];
  hacky_printf("a %f\n", a);
  hacky_printf("b %f\n", b);
  hacky_printf("c %f\n", c);
  alListener3f(AL_POSITION, a, b, c);
  eax = 0;
  esp += 4 * 4;
HACKY_COM_END()

// IA3dListener -> STDMETHOD(SetOrientation6f)		(THIS_ A3DVAL, A3DVAL, A3DVAL, A3DVAL, A3DVAL, A3DVAL) PURE; // 11
HACKY_COM_BEGIN(IA3dListener, 11)
  hacky_printf("SetOrientation6f\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  float a = *(float*)&stack[2];
  float b = *(float*)&stack[3];
  float c = *(float*)&stack[4];
  float d = *(float*)&stack[5];
  float e = *(float*)&stack[6];
  float f = *(float*)&stack[7];
  float orientation[6] = { a, b, c, d, e, f };
  alListenerfv(AL_ORIENTATION, orientation);
  hacky_printf("a %f\n", a);
  hacky_printf("b %f\n", b);
  hacky_printf("c %f\n", c);
  hacky_printf("d %f\n", d);
  hacky_printf("e %f\n", e);
  hacky_printf("f %f\n", f);
  eax = 0;
  esp += 7 * 4;
HACKY_COM_END()

// IA3dListener -> STDMETHOD(SetVelocity3f)		(THIS_ A3DVAL, A3DVAL, A3DVAL) PURE; // 15
HACKY_COM_BEGIN(IA3dListener, 15)
  hacky_printf("SetVelocity3f\n");
  hacky_printf("p 0x%" PRIX32 "\n", stack[1]);
  float a = *(float*)&stack[2];
  float b = *(float*)&stack[3];
  float c = *(float*)&stack[4];
  hacky_printf("a %f\n", a);
  hacky_printf("b %f\n", b);
  hacky_printf("c %f\n", c);
  alListener3f(AL_VELOCITY, a, b, c);
  eax = 0;
  esp += 4 * 4;
HACKY_COM_END()

