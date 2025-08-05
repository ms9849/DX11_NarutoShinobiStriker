#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public GAMEOBJECT_DESC {
		_float4 vEye{}, vAt{};
		_float fFovy{}, fNear{}, fFar{};
	} CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint eObjectID);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float							m_fFovy = {};
	_float							m_fAspect = {};
	_float							m_fNear = {};
	_float							m_fFar = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END
