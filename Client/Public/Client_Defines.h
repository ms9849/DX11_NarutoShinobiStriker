#pragma once

#include "../Default/framework.h"
#include <process.h>

namespace Client
{
	const unsigned int			g_iWinSizeX = 1280;
	const unsigned int			g_iWinSizeY = 720;

	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, EDIT, END };
	
	/*
	OBJECT ID? -> Abstract 계층이 아닌, 실제 구현되는 객체들만 가지게 될 ID.
	모든 객체는 고유한 ID를 가지게 된다.
	*/
	enum class OBJECTID { 
		BACKGROUND, DUMMY, TERRAIN, PLAYER, ENEMY,
		
		TESTPANEL, TESTBUTTON, 
		LOGO_PANEL, LOADING_PANEL, LOADINGBAR_PANEL,
		DECIMAL_UI, TIMER_UI, TITLE_UI, PRESSANYBUTTON_UI, ROTATIONICON_UI, PROGRESSBAR_UI,
		
		TEST_CAMERA,
		END 
	};

	enum class SHADER_VTXPOSTEX_IDX {
		UI,
		UI_PROGRESS_BAR,
		UI_FADE_OUT,
	};

	enum class CHARACTER_ATTACK_TYPE {
		MELEE, // 체술 ( 나선환 + 나선 수리검 )
		SHOOTING, // 인술 ( 치도리 호화구 )
		DEFENSIVE, // 방어형 (토둔 + 목둔)
		//수둔형이 필요하다면 그건 나중에 구현할 것..
	};
}

using namespace Client;

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;

/* 프레임워크 */
/* 배경(맵) */
/* 이펙트(파티클 인스턴싱, 스프라이트) */
/* 유아이 */
/* 애니메이션 툴 */
/* 컨텐츠작업 */
/* 플레이어담당, 몬스터 담당. 게임 컨텐츠 작업. */