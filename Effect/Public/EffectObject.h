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
		_bool			IsBinary = { false };
		/* 포지션 및 태그 변경은 아직 미구현. */
		_float3			vPosition = {};
		_wstring		strTextureTag = { TEXT("") };
		_int			iTextureNum = { -1 };

		_wstring		strMaskTextureTag = { TEXT("") };
		_int			iMaskTextureNum = { -1 };

		_wstring		strNoiseTextureTag = { TEXT("") };
		_int			iNoiseTextureNum = { -1 };

		_wstring		strModelTag = { TEXT("") };
		_float			fDeltaU = { -1.f };
		_float			fDeltaV = { -1.f };
		_int			iNumWidth = { -1 };
		_int			iNumHeight = { -1 };
		_int			iCurrentIdx = { -1 };

		_float			fFrameTime = { -1.f }; 

	} EFFECT_OBJECT_DESC;

private:
	CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CEffectObject(const CEffectObject& rhs); 
	virtual ~CEffectObject() = default;

public:
	void Set_Visible(_bool bFlag) { m_IsVisible = bFlag; }
	HRESULT Save_ToBinary(const _char* pEffectName);
	HRESULT Load_FromBinary(const _tchar* pEffectName);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Play_Sprite(_float fTimeDelta);
	void Check_LifeTime(_float fTimeDelta);

public:

	virtual void Set_Desc(void* pArg) override;

private:
	CEffectModel*		m_pModelCom = { nullptr };
	/* 모델 이름 저장.. */
	_wstring			m_strModelName = {};

	CShader*			m_pShaderCom = { nullptr };
	_uint				m_iNumMeshes = {};

	CTexture*			m_pDiffuseTextureCom = { nullptr };
	CTexture*			m_pMaskTextureCom = { nullptr };
	CTexture*			m_pNoiseTextureCom = { nullptr };

	_uint				m_iDiffuseTextureIdx = {};
	_uint				m_iMaskTextureIdx = {};
	_uint				m_iNoiseTextureIdx = {};

	_float				m_fDeltaU = { 0.f };
	_float				m_fDeltaV = { 0.f };
	_uint				m_iNumWidth = { 1 };
	_uint 				m_iNumHeight = { 1 };

	_uint				m_iCurrentIdx = { 0 };
	_uint				m_iMaxIdx = { 0 };
	_float				m_fTimeAcc = { 0.f };
	_float				m_fFrameTime = { 0.1f };

	_float				m_fLifeTimeAcc = { 0.f };
	_float				m_fLifeTime = { 0.1f };

	_bool				m_IsVisible = { false };

private:
	HRESULT Ready_Components(const _wstring& strModelTag);
	HRESULT Bind_ShaderResources();

public:
	static CEffectObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
