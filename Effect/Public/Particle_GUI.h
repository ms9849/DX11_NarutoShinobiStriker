#pragma once

#include "Effect_Defines.h"
#include "ParticleObject.h"
#include "EffectObject.h"

NS_BEGIN(Engine)
class CGameInstance;
class CVIBuffer_Point_Instance;
NS_END

NS_BEGIN(EffectTool)

/*
이번엔 레벨에 붙이지말고 ㅇㅇ
*/

class CParticle_GUI final : public CBase
{
public:
	enum class TEXTURE_TYPE { DIFFUSE, MASK, NOISE, END };
private:
	CParticle_GUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CParticle_GUI() = default;

public:
	HRESULT Add_SRV(const _tchar* pTextureFilePath, _uint iNumTextures, TEXTURE_TYPE eTextureType);

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);

public:
	void Particle_GUI();
	HRESULT Save_ToBinary();
	HRESULT Load_FromBinary();
private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	vector<ID3D11ShaderResourceView*> m_DiffuseSRVs = {};
	vector<ID3D11ShaderResourceView*> m_MaskSRVs = {};
	vector<ID3D11ShaderResourceView*> m_NoiseSRVs = {};

private:
	_bool m_IsEffectVisible = { true };
	_bool m_IsMainEffectVisible = { false };

	/* PARTICLE */
	_char		m_szParticleName[MAX_PATH] = {};
	_char		m_szParticleLoadPath[MAX_PATH] = {};

	_float2     m_vLifeTime = {1.f, 1.f};
	_int		m_iType = { 0 };
	_bool		m_isLoop = { false };
	_int		m_iNumInstance = { 15 };
	_float3		m_vCenter = {0.f, 4.f, 0.f};
	_float3		m_vRange = {1.f, 1.f, 1.f};
	_float2		m_vSize = {0.1f, 0.1f};
	_float3		m_vPivot = {0.f, 4.f, 0.f};
	_float2		m_vSpeed = {1.f, 1.f};
	_float4		m_vMainColor = { 1.f, 1.f, 1.f ,1.f };
	_float4		m_vSubColor = {1.f, 1.f, 1.f, 1.f};
	_float3		m_vRotation = { 0.f, 0.f, 0.f };
	_uint		m_iTextureNum = { 0 }, m_iMaskTextureNum = { 0 }, m_iNoiseTextureNum = { 0 };
	_uint		m_iShaderPassIdx = { 0 };
	_float		m_fFrameTime = { 0.1f };
	_int		m_iNumWidth = { 1 };
	_int		m_iNumHeight = { 1 };

public:
	static CParticle_GUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END