#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CCamera;
class CTransform;
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
/*
록온 시스템 -> 시야 내에 있고 (외적 결과 음수인가 양수인가로 판정) & 가장 가까운 몬스터가 록온되어야 할 것

플레이어는 공격할때 록온된 몬스터가 있다면 해당 몬스터를 향한다
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
	HRESULT		Set_NextLevelID(LEVEL eLevelID);

public:
	HRESULT		Initialize_GameManager();
	void		Release_GameManager();
	void		Clear();

#pragma region CAMERA
	CTransform* Get_TargetTransform();
	void		SetUp_Target();
	HRESULT		Add_TargetTransform(CTransform* pTransformCom);
	HRESULT		Add_Camera(LEVEL eLevelID, const _wstring& strCameraTag, CCamera* pCamera);
	HRESULT		Change_Camera(LEVEL eLevelID, const _wstring& strCameraTag);
#pragma endregion

private:
	CGameInstance* m_pGameInstance = { nullptr };

	class CCamera_Manager* m_pCamera_Manager = { nullptr };

	class CPlayer* m_pPlayer = {};
	LEVEL			m_eNextLevel = {};
public:
	virtual void Free() override;
};

NS_END

