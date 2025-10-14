#pragma once

#include "Client_Defines.h"
#include "Camera.h"

/* 
늘 플레이어를 추적하는 카메라.
스킬 필살기 별로 카메라 따로 추가할 예정 
*/
NS_BEGIN(Client)
class CMainCamera : public CCamera
{
public:
	typedef struct tagMainCameraDesc : public CAMERA_DESC {
		class CTransform* pPlayerTransform;
	} MAIN_CAMERA_DESC;

private:
	CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CMainCamera(const CMainCamera& rhs);
	virtual ~CMainCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnChange(const _float4x4* pWorldMatrix) override;

	void Look_Target(_float fTimeDelta);
	void Chase_Target(_float fTimeDelta);

	void Rotate_Point(_float fTimeDelta);

	void Mouse_Lock();

private:
	_float m_fTimeAcc = { 0.f };
	_float m_fPhi = { 0.f };
	_float m_fTheta = { 0.f };
	_float3 m_vCameraPoint = {};

	_float3 m_vCameraPos = {};
	_float m_fRotateX = {};
	_float m_fRotateY = {};
	class CGameManager* m_pGameManager = { nullptr };
	class CTransform*	m_pPlayerTransform = { nullptr };
	_bool m_IsLockOn = { true };

public:
	static CMainCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

