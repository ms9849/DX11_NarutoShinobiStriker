#pragma once

#include "Client_Defines.h"
#include "Camera.h"

/*
늘 플레이어를 추적하는 카메라.
스킬 필살기 별로 카메라 따로 추가할 예정
*/

NS_BEGIN(Client)

class CNPCTalkCamera : public CCamera
{
public:
	typedef struct tagNPCTalkCameraDESC : public CAMERA_DESC {
		class CTransform* pTargetTransform;
	} NPC_TALK_CAMERA_DESC;

private:
	CNPCTalkCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CNPCTalkCamera(const CNPCTalkCamera& rhs);
	virtual ~CNPCTalkCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
	class CTransform* m_pTargetTransform = { nullptr };
	_float3 m_vCameraPos = {};

public:
	static CNPCTalkCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END

