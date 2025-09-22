#pragma once

#include "Client_Defines.h"
#include "Skill_Table.h"
#include "Camera.h"

/*
늘 플레이어를 추적하는 카메라.
스킬 필살기 별로 카메라 따로 추가할 예정
*/
NS_BEGIN(Client)
class CSkillActionCamera : public CCamera
{
public:
	typedef struct tagSkillActionCameraDesc : public CAMERA_DESC {
		class CTransform* pPlayerTransform;
		SKILL eSkillType;
	} SKILL_ACTION_CAMERA_DESC;

private:
	CSkillActionCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CSkillActionCamera(const CSkillActionCamera& rhs);
	virtual ~CSkillActionCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnChange(const _float4x4* pWorldMatrix);

private:
	_float	m_fKamuiCamSpeedRatio = { 1.f };
	_float  m_fKamuiCamTimeAcc = { 0.f };
	_float  m_fMaxKamuiCamTimeAcc = { 0.15f };

	_float  m_fFireBallCamSpeedRatio = { 0.3f };
	_float  m_fZoomTimeAcc = { 0.f };
	_float  m_fMaxZoomTimeAcc = { 0.4f };
	_bool   m_IsZoom = { false };

	SKILL   m_eSkillType = {};
	_float3 m_vCameraPos = {};
	class CGameManager* m_pGameManager = { nullptr };
	class CTransform* m_pPlayerTransform = { nullptr };

public:
	static CSkillActionCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

