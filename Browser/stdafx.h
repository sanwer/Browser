#pragma once

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE

#include "include/cef_client.h"
#include "include/cef_app.h"

#include <windows.h>
#include <objbase.h>
#include "resource.h"

#include "..\DuiLib\UIlib.h"
using namespace DuiLib;

#define  DEBUG_MODEL

#define TIEM_LOGINDLG_AUTO 1001
