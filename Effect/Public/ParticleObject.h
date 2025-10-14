#pragma once

#include "Effect_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameInstnace;
class CVIBuffer_Point_Instance;
class CTexture;
class CShader;
NS_END

NS_BEGIN(EffectTool)

/*
굳이굳이 래핑하는 이유?
-> 1. 일단 이펙트 툴에서 테스트 편해짐
-> 2. 상속, 재활용 편해짐

그리고 일단 하나로 돌려쓰고 싶은 마음이 큼..
*/

class CParticleObject final : public CGameObject
{
public:
	/* 셰이더 순서와 해당 enum의 순서는 맞춰줘야 한다. */
	enum class PARTICLE_TYPE { EXPLOSION, DROP, END };

	typedef struct tagParticleDesc : public GAMEOBJECT_DESC {
		PARTICLE_TYPE eType;
		wstring strDiffuseTextureTag = { TEXT("") };
		_int iDiffuseTextureNum = { 0 };

		wstring strMaskTextureTag = { TEXT("") };
		_int iMaskTextureNum = { 0 };
		/* 파티클 시스템 하나당 사용되는 변수 */
		_bool			isLoop;

		/* 중심으로부터 얼마나 떨어져있는지 */
		_float3			vPivot;
		_float2			vSpeed;

		/* 인스턴스 하나당 사용되는 변수 */
		_float4			vColor;
		_float3			vRotation;
		_float2			vLifeTime;
		_uint			iNumInstance = {};
		_float2			vSize = {};
		_float3			vCenter = {};
		_float3			vRange = {};

	} PARTICLE_OBJECT_DESC;

private:
	CParticleObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CParticleObject(const CParticleObject& Prototype);
	virtual ~CParticleObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Ready_VIBuffer(void* pArg);

private:
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

	CTexture* m_pDiffuseTexCom = { nullptr };
	CTexture* m_pMaskTexCom = { nullptr };
	CTexture* m_pNoiseTexCom = { nullptr };

	_uint m_iDiffuseTextureIdx = { 0 };
	_uint m_iMaskTextureIdx = { 0 };
	_uint m_iNoiseTextureIdx = { 0 };

	CShader* m_pShaderCom = { nullptr };
	PARTICLE_TYPE m_eType = {};

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CParticleObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
