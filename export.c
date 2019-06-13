// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include <stddef.h>

typedef struct {
  const char* name;
  void(*callback)();
} ExportFunction;

typedef struct {
  const char* name;
  unsigned int functionCount;
  ExportFunction functions[];
} ExportLibrary;


ExportLibrary* RegisterExportLibrary(const char* name) {
  return NULL;
}

ExportLibrary* FindExportLibrary(const char* name) {
  return NULL;
}

ExportFunction* FindExportFunction(const char* name) {
  return NULL;
}

void RegisterExportFunction(ExportLibrary* library, const char* name) {
}
