#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CPooling;
class CGameInstnace;
class CVIBuffer_Point_Instance;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

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
	enum class PARTICLE_TYPE { EXPLOSION, DROP, FLOAT_DROP, EXPLOSION_FLOAT, EXPLOSION_NON_UV, END };

	typedef struct tagParticleDesc : public GAMEOBJECT_DESC {
		/* 파티클 시스템 하나당 사용되는 변수 */
		_bool			isLoop;
		PARTICLE_TYPE	eType;
		_int			iDiffuseTextureNum = { 0 };
		_int			iMaskTextureNum = { 0 };
		_int			iNoiseTextureNum = { 0 };
		_float4			vMainColor = {0.f, 0.f, 0.f, 0.f};
		_float4			vSubColor = {0.f, 0.f, 0.f, 0.f};
		_float			fFrameTime = { 0.1f };
		_int			iNumWidth = { 1 };
		_int			iNumHeight = { 1 };
		_int			iCurrentIdx = { 0 };
		_int			iShaderPassIdx = { 0 };
		/* 중심으로부터 얼마나 떨어져있는지 */
		_float3			vPivot;
		_float2			vSpeed;

		/* 인스턴스 하나당 사용되는 변수 */
		_float3			vRotation;
		_float2			vLifeTime;
		_uint			iNumInstance = {};
		_float2			vSize = {};
		_float3			vCenter = {};
		_float3			vRange = {};

	} PARTICLE_OBJECT_DESC;

	typedef struct tagParticleLoadDesc : public GAMEOBJECT_DESC {
		_wstring strParticlePath = {};
		/* 트랜스폼 조정용*/
		_float3 vPosition = {};
		PARTICLE_TYPE eType = { PARTICLE_TYPE::EXPLOSION };
	} PARTICLE_LOAD_DESC;
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
	void Set_Position(_vector vPosition);

public:
	virtual void Set_Desc(void* pArg) override; 
	HRESULT Ready_VIBuffer(void* pArg);
	void	Play_Sprite(_float fTimeDelta);
	void	Set_Visible(_bool bFlag) { m_IsVisible = bFlag; }
private:

	_bool			m_IsVisible = { true } ;
	_float4x4		m_CombinedMatrix = {};
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	CTexture*		m_pDiffuseTexCom = { nullptr };
	CTexture*		m_pMaskTexCom = { nullptr };
	CTexture*		m_pNoiseTexCom = { nullptr };

	_uint			m_iDiffuseTextureIdx = { 0 };
	_uint			m_iMaskTextureIdx = { 0 };
	_uint			m_iNoiseTextureIdx = { 0 };

	CShader*		m_pShaderCom = { nullptr };
	PARTICLE_TYPE	m_eType = {};

	_float4			m_vMainColor = {};
	_float4			m_vSubColor = {};
	_float			m_fFrameTime = { 0.1f };
	_float			m_fTimeAcc = { 0.f };
	_int			m_iNumWidth = { 1 };
	_int			m_iNumHeight = { 1 };
	_int			m_iMaxIdx = { 0 };
	_int			m_iCurrentIdx = { 0 };
	_int			m_iShaderPassIdx = { 0 };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CParticleObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
