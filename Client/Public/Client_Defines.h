#pragma once

#include "../Default/framework.h"
#include <process.h>

namespace Client
{
	const static wstring strMonsterCollisionTag = TEXT("Monster_Body");
	const static wstring strPlayerCollisionTag = TEXT("Player_Body");

	const static wstring strMonsterAttackCollisionTag = TEXT("Monster_Attack");
	const static wstring strPlayerAttackCollisionTag = TEXT("Player_Attack");

	const unsigned int			g_iWinSizeX = 1280;
	const unsigned int			g_iWinSizeY = 720;

	enum class LEVEL { STATIC, LOADING, LOGO, OUTFITSELECT, GAMEPLAY, END };
	/*
	OBJECT ID? -> Abstract 계층이 아닌, 실제 구현되는 객체들만 가지게 될 ID.
	모든 객체는 고유한 ID를 가지게 된다.
	*/
	enum class OBJECTID { 
		KONOHA_VILLAGE, TUTORIAL_MAP,
		PROPS, GATE, TREE,

		BACKGROUND, DUMMY, TERRAIN, 
		MANNEQUIN, 
		
		PLAYER, PLAYER_HEAD, PLAYER_FACE, PLAYER_UPPER, PLAYER_LOWER, PLAYER_WEAPON,
		WHITEJETSU, BIRD, BOXER,
		
		EFFECT,

		TESTPANEL, TESTBUTTON, 
		LOGO_PANEL, LOADING_PANEL, LOADINGBAR_PANEL, SKILLSLOT_PANEL,
		ATTACKTYPE_PANEL, MISSIONALERT_PANEL, COMBOKO_PANEL, OUTFITSELECT_PANEL, MODEL_PANEL,

		CREATECHARACTER_UI,
		SKILLSLOT_UI, DECIMAL_UI, TIMER_UI, TITLE_UI, KO_UI,
		PRESSANYBUTTON_UI, ROTATIONICON_UI, PROGRESSBAR_UI,
		MODELSELECTBUTTON_UI, MODELDECIDEBUTTON_UI,

		ENEMY_HPBAR,
		
		WOODHAND,

		SKYBOX,

		TEST_CAMERA, OUTFITSELECT_CAMERA, MAIN_CAMERA,
		END 
	};

	enum class SHADER_VTXPOSTEX_IDX {
		UI,
		UI_MASK,
		UI_ENEMYHPBAR,
		UI_PROGRESSBAR,
		UI_FADEINOUT,
		UI_SKILL,
		UI_SPECIALSKILL,
		END
	};

	/* OnCollision 이벤트가 발생하면 넘겨줄 콜리전 데스크. */
	
	/* 
	필요한 값 
	1. 맞춘 대상이 누구인가 -> 이건 콜리전 매니저의 아이디로 유추가능 
	( PlayerAttack - Monster ) & ( MonsterAttack - Player )
	*/

	/*
	2. 어떤 처리를 해야하는가
	검 공격이라면 더 밀려나야 하고, 멀리 날아가는 모션이 있어야할 수도 있음.
	하지만 주먹 공격이라면 막타 제외하고 날아가는 모션이 안나온다.
	-> 같은 콜라이더여도 다르게 처리를 해줘야할 수 있음. 
	-> 플레이어든 제츠든 COllision Desc 갖고 있게 하고, 콜라이더에 같이 묶어보내기?
	-> 플레이어는 HandAttack, 검 등 여러 콜리전이 존재함.
	-> 한번에 하나의 상태만을 보장하니까..
	-> 콜리전 데스크도 하나로 써도 될듯?

	-> 일단 기본적으로 몸통의 충돌처리를 할 콜라이더는 
	Com_Collider로 이름을 고정하게 될 것. 
	*/

	enum class COLLIDER_HANDLE_ID {
		PLAYER_HAND_ATTACK,
		PLAYER_HAND_ATTACK_FINAL,
		PLAYER_SWORD_ATTACK,
		PLAYER_SWORD_ATTACK_FINAL,

		PLAYER_NINJUTSU_RASENGAN,
		PLAYER_NINJUTSU_RASENSHURIKEN,
		PLAYER_NINJUTSU_KAMUI,

		PLAYER_NIMJUTSU_CHIDORI,
		PLAYER_NINJUTSU_FIREBALL,
		PLAYER_NINJUTSU_BIGSHARK,

		ENEMY_JETSU_ATTACK,
		ENEMY_JETSU_WOODHAND,

		ENEMY_BIRD_ATTACK,
		ENEMY_BIRD_THROW,

		ENEMY_BOXER_ATTACK,
		ENEMY_BOXER_LEAFHURRICANE
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