#pragma once

#include "Client_Defines.h"
#include "Camera.h"

/*
늘 플레이어를 추적하는 카메라.
스킬 필살기 별로 카메라 따로 추가할 예정
*/
NS_BEGIN(Client)
class CCutSceneCamera : public CCamera
{
public:
	typedef struct tagCutSceneCameraDesc : public CAMERA_DESC {
	} CUTSCENE_CAMERA_DESC;

private:
	CCutSceneCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CCutSceneCamera(const CCutSceneCamera& rhs);
	virtual ~CCutSceneCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_fTimeAcc = { 0.f };
	_float3 m_vCameraPos = {};
	class CGameManager* m_pGameManager = { nullptr };
	class CTransform* m_pPlayerTransform = { nullptr };

public:
	static CCutSceneCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

