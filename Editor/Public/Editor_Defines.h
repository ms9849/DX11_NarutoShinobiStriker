#pragma once

#include "../../Client/Public/Client_Defines.h"
#include "../Default/framework.h"
#include <process.h>

namespace Editor
{
	/* 에디터에서 필요한 정보들 넘겨줄 것 */
	enum class LEVEL { STATIC, EDIT, END };

}

using namespace Editor;

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;