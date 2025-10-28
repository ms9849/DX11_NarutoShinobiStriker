#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
class COrthogonal;
class CPooling;
class CTransform;

NS_END

NS_BEGIN(Client)

class CEffect_Icon final : public CGameObject
{
public:
	typedef struct tagEffectIconDesc : public GAMEOBJECT_DESC {
		CTransform* pTargetTransform;
	} EFFECT_ICON_DESC;
private:
	CEffect_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CEffect_Icon(const CEffect_Icon& Prototype);
	virtual ~CEffect_Icon() = default;

public:
	virtual void Set_Desc(void* pArg) override;	

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CTransform* m_pTargetTransform = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CPooling* m_pPoolingCom = { nullptr };
	_float			m_fTimeAcc = { 0.f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
 	virtual void Free() override;
};

NS_END
