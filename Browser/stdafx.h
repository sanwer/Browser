#ifndef _STDAFX_H_
#define _STDAFX_H_
#pragma once

#ifndef WINVER
#define WINVER 0x0A00
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0603
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE

#include <windows.h>
#include "resource.h"

#ifdef _DEBUG
 //#define CEF_USE_SANDBOX
 #pragma comment(lib, "libcef_d.lib")
#else
 #pragma comment(lib, "libcef.lib")
#endif

#endif