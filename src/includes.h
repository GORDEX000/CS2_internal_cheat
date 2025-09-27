#pragma once
#include <Windows.h>
#include <WinUser.h>
#include <d3d11.h>
#include <dxgi.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <dwmapi.h>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <array>
#include <dinput.h>
#include <d3dcompiler.h>
#include <exception>
#include <unordered_map>
#include <algorithm>
#include <cfloat>

#include "Lolz/Lolz.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "MemoryHandler/Offsets.h"
#include "MemoryHandler/Math.h"

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;