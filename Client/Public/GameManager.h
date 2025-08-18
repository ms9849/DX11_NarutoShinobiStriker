#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CCamera;
NS_END

NS_BEGIN(Client)

/*
클라이언트에 존재하는 유일한 싱글톤.

플레이어 관리, 
카메라 관리,
게임 내에서 다른 객체의 정보 필요할 때 세팅,

등등..

다양한 게임 내의 흐름을 컨트롤하는데 쓰이는 매니저가 될 것이다.

-> 콜리전 매니저는? 엔진 단에서 적용.
-> 이펙트는? 어차피 풀링될테니까 풀링 매니저 (게임인스턴스) 에서 꺼내오는게 합당할 것.
*/

class CGameManager final : public CBase
{
	DECLARE_SINGLETON(CGameManager);

private:
	CGameManager();
	virtual ~CGameManager() = default;

public:
	class CPlayer* Get_PlayerPtr() {
		return m_pPlayer;
	}

	LEVEL		Get_NextLevel();
	HRESULT		Set_PlayerPtr(class CPlayer* pPlayer);
	HRESULT		Set_QuestPtr(class CQuestLog* pQuestLog);
	HRESULT		Set_NextLevelID(LEVEL eLevelID);

public:
	HRESULT		Add_Camera(LEVEL eLevelID, const _wstring& strCameraTag, CCamera* pCamera);
	HRESULT		Change_Camera(LEVEL eLevelID, const _wstring& strCameraTag);

public:
	HRESULT		Initialize_GameManager();
	void		Release_GameManager();
	void		Clear();

private:
	/* 카메라는 동적으로 추가되는게 아니라 
	개발자의 의도에 따라 정적인 갯수만큼 추가 될 것이므로 전부 ENUM CLASS화 한다. */

	map<const _wstring, CCamera*> m_Cameras[ENUM_CLASS(LEVEL::END)] = {};
	CCamera* m_pActivatedCamera = {};

	class CPlayer* m_pPlayer = {};
	class CQuestLog* m_pQuestLog = {};

	CGameInstance* m_pGameInstance = { nullptr };
	LEVEL			m_eNextLevel = {};

public:
	virtual void Free() override;
};

NS_END

