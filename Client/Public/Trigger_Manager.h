#pragma once

/*
게임의 흐름 진행에 관여하는 매니저. 

튜토리얼 맵 -> 본 맵으로의 이동도 담당할 수 있게 처리한다. 

필요한 기능 
0. 카카시랑 대화 시작한 뒤 스포너 생성.
1. 튜토리얼 -> 본 맵으로 넘어가는 순간
-> 트리거박스가 사라질떄 (Check Tutorial Clear) 체크 시작
-> 체크 시작한 이후 맵에 모든 몬스터가 사라졌다면

2. 본 맵 트리거 박스 생성 
3. 대사 생성 
*/

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CTrigger_Manager final : public CBase
{
private:
	CTrigger_Manager();
	virtual ~CTrigger_Manager() = default;

public:
	HRESULT	OnTrigger(TRIGGER_TYPE eTriggerType);
	TRIGGER_TYPE Get_CurrentTrigger() { return m_eCurrentTrigger; }
	_bool IsActivated(TRIGGER_TYPE eTriggerType) { return m_IsTriggerActivated[ENUM_CLASS(eTriggerType)]; }

private:
	class CGameManager* m_pGameManager = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	TRIGGER_TYPE m_eCurrentTrigger = { TRIGGER_TYPE::END };
	_bool m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::END)] = {};

public:
	static CTrigger_Manager* Create();
	virtual void Free() override;
};

NS_END 
