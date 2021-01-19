#pragma once

#pragma comment(lib, "Shlwapi.lib")

#include <Windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <stdlib.h>

#define DS_STREAM_RENAME L":wtfbbq"
#define DS_DEBUG_LOG(msg) wprintf(L"[LOG] - %s\n", msg)