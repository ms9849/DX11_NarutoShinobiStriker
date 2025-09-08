#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CShader;
class COrthogonal;
class CPooling;
NS_END

NS_BEGIN(Client)

class CEffect_CoolDown : public CGameObject
{
public:
	typedef struct tagEffectCoolDownDesc : public GAMEOBJECT_DESC {
		_float fX;
		_float fY;
	} EFFECT_COOLDOWN_DESC;
private:
	CEffect_CoolDown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CEffect_CoolDown(const CEffect_CoolDown& Prototype);
	virtual ~CEffect_CoolDown() = default;

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
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	COrthogonal*	m_pOrthogonalCom = { nullptr };
	CPooling*		m_pPoolingCom = { nullptr };

	_float			m_fTimeAcc = { 0.f };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
public:
	static CEffect_CoolDown* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
