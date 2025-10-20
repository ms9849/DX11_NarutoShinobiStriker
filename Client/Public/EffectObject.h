#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CEffectModel;
class CTexture;
NS_END 

NS_BEGIN(Client)

/*
모델과 텍스쳐를 마음대로 갈아끼울 수 있게끔 세팅해준다.
*/

class CEffectObject final : public CGameObject
{
public:
	typedef struct tagEffectDesc : public GAMEOBJECT_DESC
	{
		_bool			IsBinary = { false };
		/* 포지션 및 태그 변경은 아직 미구현. */
		_float3			vPosition = { 0.f, 0.f, 0.f };
		_wstring		strTextureTag = { TEXT("") };
		_int			iTextureNum = { 0 };

		_wstring		strMaskTextureTag = { TEXT("") };
		_int			iMaskTextureNum = { 0 };

		_wstring		strNoiseTextureTag = { TEXT("") };
		_int			iNoiseTextureNum = { 0 };

		_wstring		strModelTag = { TEXT("") };
		_float			fDeltaU = { 0.f };
		_float			fDeltaV = { 0.f };
		_int			iNumWidth = { 1 };
		_int			iNumHeight = { 1 };
		_int			iCurrentIdx = { 0 };
		_int			iShaderPassIdx = { 0 };
		_float			fFrameTime = { 0.f }; 
		_float			fLifeTime = { 0.1f };
		//
		_float			fStartTime = { 0.f };
		_float4			vMainColor = {0.f, 0.f, 0.f, 1.f};
		_float4			vSubColor = {0.f, 0.f, 0.f, 1.f};

		_float3 		vRotation = { 0.f, 0.f, 0.f };	
		_bool			IsRotation = { false };

		_float3			vScale = { 1.f, 1.f, 1.f };
		_float3			vDeltaScale = { 1.f, 1.f, 1.f };
		_bool			IsLoop = { true };
		_bool			IsBlend = { true };
	} EFFECT_OBJECT_DESC;

private:
	CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CEffectObject(const CEffectObject& rhs); 
	virtual ~CEffectObject() = default;

public:
	void Set_Visible(_bool bFlag) { m_IsVisible = bFlag; }
	HRESULT Save_ToBinary(const _char* pEffectName);
	HRESULT Load_FromBinary(const _tchar* pEffectName);
	HRESULT Save_ToBinary(const _char* pEffectName, DWORD dwByte, HANDLE hHandle = nullptr);
	HRESULT Load_FromBinary(const _tchar* pEffectName, DWORD dwByte, HANDLE hHandle = nullptr);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Reset_LifeTime() { m_fLifeTimeAcc = 0.f;  m_fTimeAcc = 0.f; m_iCurrentIdx = 0; m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
	}
	void Play_Sprite(_float fTimeDelta);
	void Check_LifeTime(_float fTimeDelta);
	_float Get_StartTime() { return m_fStartTime; }
	_float Get_LifeTime() { return m_fLifeTime; }
	_float Get_CurLifeTime() { return m_fLifeTimeAcc; }
	_bool IsVisible() { return m_IsVisible; }
	void Set_ParentMatrix(_fmatrix pParentWorldMatrix);
public:
	virtual void Set_Desc(void* pArg) override;
	CEffectObject::EFFECT_OBJECT_DESC Get_Desc();
private:
	CEffectModel*		m_pModelCom = { nullptr };
	/* 모델 이름 저장.. */
	_bool				m_IsBlur = { false };
	_wstring			m_strModelName = {};

	CShader*			m_pShaderCom = { nullptr };
	_uint				m_iShaderPassIdx = { 0 };
	_uint				m_iNumMeshes = {};

	CTexture*			m_pDiffuseTextureCom = { nullptr };
	CTexture*			m_pMaskTextureCom = { nullptr };
	CTexture*			m_pNoiseTextureCom = { nullptr };

	_uint				m_iDiffuseTextureIdx = { 0 };
	_uint				m_iMaskTextureIdx = { 0 };
	_uint				m_iNoiseTextureIdx = { 0 };

	_float				m_fDeltaU = { 0.f };
	_float				m_fDeltaV = { 0.f };
	_uint				m_iNumWidth = { 1 };
	_uint 				m_iNumHeight = { 1 };

	_uint				m_iCurrentIdx = { 0 };
	_uint				m_iMaxIdx = { 0 };
	_float				m_fTimeAcc = { 0.f };
	_float				m_fFrameTime = { 0.1f };
	/* 컨테이너 안에서 나타나기 시작하는 순간. */
	_float				m_fStartTime = { 0.f };
	_float				m_fLifeTimeAcc = { 0.f };
	_float				m_fLifeTime = { 0.1f };
	
	_float4				m_vMainColor = {};
	_float4				m_vSubColor = {}; 

	_float3				m_vRotation = { 0.f, 0.f, 0.f };
	_bool				m_IsRotation = { false };

	_float3				m_vScale = { 1.f, 1.f, 1.f };
	_float3				m_vDeltaScale = { 1.f, 1.f, 1.f};
	_bool				m_IsLoop = { true };
	_bool				m_IsVisible = { true };
	_bool				m_IsBlend = { true };

	_float4x4			m_CombinedMatrix = {};
private:
	HRESULT Ready_Components(const _wstring& strModelTag);
	HRESULT Bind_ShaderResources();
	
public:
	static CEffectObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
