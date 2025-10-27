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

class CEffect_HitSprite : public CGameObject
{
public:
	typedef struct tagEffectHitSpriteDesc : public GAMEOBJECT_DESC {
		_float4 vPosition = {};
		_float  fLifeTime = {};
		_float  fStartScale = {};
		_float	fDeltaScale = {};
		_float4 vColor = {};
		_bool	IsBlur = {};
	} EFFECT_HIT_SPRITE_DESC;
private:
	CEffect_HitSprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CEffect_HitSprite(const CEffect_HitSprite& Prototype);
	virtual ~CEffect_HitSprite() = default;

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

	_float			m_fTimeAcc = { 0.f };
	_float			m_fLifeTime = { 0.f };
	_float			m_fStartScale = { 1.f };
	_float			m_fDeltaScale = { 1.f };
	_float4			m_vColor = { 0.f, 0.f, 0.f, 1.f };
	_bool			m_IsBlur = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect_HitSprite* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
