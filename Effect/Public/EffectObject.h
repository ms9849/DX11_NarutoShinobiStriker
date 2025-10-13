#pragma once

#include "Effect_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CEffectModel;
class CTexture;
NS_END 

NS_BEGIN(EffectTool)

/*
모델과 텍스쳐를 마음대로 갈아끼울 수 있게끔 세팅해준다.
*/

class CEffectObject final : public CGameObject
{
public:
	typedef struct tagEffectDesc
	{
		_float3			vPosition;
		_wstring		strTextureTag = { TEXT("") };
		_uint			iTextureNum = { -1 };

		_wstring		strMaskTextureTag = { TEXT("") };
		_uint			iMaskTextureNum = { -1 };

		_wstring		strModelTag = { TEXT("") };
		_float			fDeltaU = { 0.f };
		_float			fDeltaV = { 0.f };

	} EFFECT_OBJECT_DESC;

private:
	CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CEffectObject(const CEffectObject& rhs); 
	virtual ~CEffectObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_Desc(void* pArg) override;

private:
	CEffectModel*		m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	_uint				m_iNumMeshes = { };

	CTexture*			m_pDiffuseTextureCom = { nullptr };
	CTexture*			m_pMaskTextureCom = { nullptr };
	_uint				m_iDiffuseTextureIdx = {};
	_uint				m_iMaskTextureIdx = {};

	_float				m_fDeltaU = { 0.f };
	_float				m_fDeltaV = { 0.f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffectObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
