#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTexture;
class CTransform;
class CShader;
NS_END

NS_BEGIN(Client)

class CTrail final : public CGameObject
{
public:
	enum class TRAIL_TYPE { SWORD, FOOT, END };
	typedef struct tagTrailPrototype {
		TRAIL_TYPE eType = {};
		_uint iNumVertices = {};
	} PROTOTYPE_TRAIL_DESC;

	typedef struct tagTrail {
		_wstring strTrailTextureTag = {};
		_float4 vHighPosition = {};
		_float4 vLowPosition = {};
	} TRAIL_DESC;
private:
	CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	_bool IsRenderable();
	void Update_Trail(_fmatrix TargetWorldMatrix, _bool bMakeTrail = false);
	void Set_CombinedWorldMatrix(_fmatrix TargetWorldMatrix);
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(const _wstring& strTextureTag);

private:
	/* 기본적으론 소드 */
	TRAIL_TYPE m_eType = { TRAIL_TYPE::SWORD };
	_float4x4 m_TargetWorldMatrix;
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	_bool			m_IsBlur = { false };
	_float			m_fTimeAcc = { 0.f };
	ID3D11Buffer*	m_pVB = { nullptr };
	ID3D11Buffer*	m_pIB = { nullptr };
	VTXPOSTEX*		m_pVTXPOSTEXs = { nullptr };
	_float4			m_vHigh = {};
	_float4			m_vLow = {};
	_uint			m_iNumPresent = {};
	_uint			m_iEndIndex = {};
	_uint			m_iNumVertices = {};
	_uint			m_iNumIndices = {};

	/* 이전 프레임 2개까지의 컴바인드 월드매트릭스 저장 */
	_float3			m_vPreHighPositions[3] = {};
	_float3			m_vPreLowPositions[3] = {};

private:


public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID, void* pArg);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
