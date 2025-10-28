#pragma once

namespace Client
{
	enum class ATTACK_TYPE {
		MELEE, NINJUTSU, END
		//DEFENSIVE, END
	};

	enum class SKILLNUM {
		FIRST,
		SECOND,
		THIRD,
		SPECIAL,
		END
	};

	enum class SKILL {
		SHADOW_ESCAPE,
		RASENGAN, RASEN_SHURIKEN, /* 근접? */
		CHIDORI, FIREBALL, /* 인술*/
		WOOD_WALL, WOOD_HAND, /* 방어형 */
		BIG_RASENGAN, BIG_SHARK, KAMUI, /* 필살기류 */
		END
	};

	/* 스킬 정보. 쿨타임과 쿨타임 진행 상황만 담아둔다. */
	typedef struct tagSkillInfo {
		SKILL eSkill = {};

		float fMaxCoolDown = {};
		float fTimeAcc = {};

		bool  IsSpecial = { false };
		float fMaxGauge = { 200.f };
		float fGaugeAcc = { 0.f };

	} SKILL_INFO;

	const SKILL_INFO g_SkillTable[static_cast<int>(SKILL::END)] = {
		{ SKILL::SHADOW_ESCAPE, 10.f, 0.f},
		{ SKILL::RASENGAN, 10.f, 0.f},
		{ SKILL::RASEN_SHURIKEN, 10.f, 0.f},
		{ SKILL::CHIDORI, 10.f, 0.f},
		{ SKILL::FIREBALL, 10.f, 0.f},
		{ SKILL::WOOD_WALL, 10.f, 0.f},
		{ SKILL::WOOD_HAND, 10.f, 0.f},
		/* 필살기들. 일단은 이대로 세팅 */
		{ SKILL::BIG_RASENGAN, 0.f, 0.f, true },
		{ SKILL::BIG_SHARK, 10.f, 0.f, true },
		//20초임
		{ SKILL::KAMUI, 2.f, 0.f, true }
	};
}