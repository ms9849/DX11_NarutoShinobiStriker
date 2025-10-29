#pragma once

#include "Client_Defines.h"
#include "Skill_Table.h"
#include "Camera.h"

/*
늘 플레이어를 추적하는 카메라.
스킬 필살기 별로 카메라 따로 추가할 예정
*/
NS_BEGIN(Client)
class CBossCamera : public CCamera
{
public:
	typedef struct tagSkillActionCameraDesc : public CAMERA_DESC {
		class CTransform* pPlayerTransform;
		SKILL eSkillType;
	} SKILL_ACTION_CAMERA_DESC;

private:
	CBossCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CBossCamera(const CBossCamera& rhs);
	virtual ~CBossCamera() = default;

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
	_float	m_fKamuiCamSpeedRatio = { 0.25f };
	_float  m_fKamuiCamTimeAcc = { 0.f };
	_float  m_fMaxKamuiCamTimeAcc = { 0.55f };

	_float3 m_vCameraPos = {};

	class CGameManager* m_pGameManager = { nullptr };
	class CTransform* m_pBossTransform = { nullptr };

public:
	static CBossCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

