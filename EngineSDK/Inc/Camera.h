#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	/* 포지션 세팅할땐 Float4로 받아와서 w값 1로 살려줘야함*/
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

	virtual void OnChange() {};
protected:
	_float							m_fFovy = {};
	_float							m_fAspect = {};
	_float							m_fNear = {};
	_float							m_fFar = {};

protected:
	HRESULT Bind_Matrices();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END
