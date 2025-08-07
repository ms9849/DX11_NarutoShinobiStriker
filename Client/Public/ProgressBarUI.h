#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer;
class CShader;
NS_END

NS_BEGIN(Client)

class CProgressBarUI final : public CUIObject
{
private:
	CProgressBarUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CProgressBarUI(const CProgressBarUI& rhs);
	virtual ~CProgressBarUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Progress(_float fProgress);
	void Set_MaxProgress(_float fMaxProgress);

private:
	/* 프로그레스 어떻게 받아오지?.. */
	_float m_fProgress = { 0.f };
	_float m_fPreProgress = { 0.f };
	_float m_fMaxProgress = { 1.f };

	_float3 m_vOriginPos = {};
	_float m_fMaxSize = {};

private:
	HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources() override;

private:
	void Play_Animation(_float fTimeDelta);

public:
	static CProgressBarUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
