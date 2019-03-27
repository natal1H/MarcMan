#ifndef DLWRAPPER_H
#define DLWRAPPER_H

#ifdef WIN32

#include <windows.h>

#define DLIB_OPEN(lib,param) LoadLibrary(lib)
#define DLIB_SYM(handle,function) GetProcAddress(handle,function)
#define DLIB_CLOSE(handle) FreeLibrary(handle)
#define DLIB_ERROR() "Error while working with shared library"
#define DLIB_HANDLE HINSTANCE
#define RTLD_NOW
#define RTLD_LAZY

#else

#include <dlfcn.h>

#define DLIB_OPEN(lib,param) dlopen(lib, param)
#define DLIB_SYM(handle,function) dlsym(handle,function)
#define DLIB_CLOSE(handle) dlclose(handle)
#define DLIB_ERROR() dlerror()
#define DLIB_HANDLE void *

#endif

#endif
