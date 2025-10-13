#pragma once

/* 클라 디파인도 추가 */
#include "../../Client/Public/Client_Defines.h"
#include "../Default/framework.h"
#include <process.h>

namespace EffectTool
{
	/* 에디터에서 필요한 정보들 넘겨줄 것 */
	enum class LEVEL { EFFECT, END };

}

using namespace EffectTool;

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;