#pragma once

#include "Effect_Defines.h"
#include "ParticleObject.h"

NS_BEGIN(Engine)
class CGameInstance;
class CVIBuffer_Point_Instance;
NS_END

NS_BEGIN(EffectTool)

/*
이번엔 레벨에 붙이지말고 ㅇㅇ
*/

class CEffect_GUI final : public CBase
{
public:
	enum class TEXTURE_TYPE { DIFFUSE, MASK, NOISE, END };
private:
	CEffect_GUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEffect_GUI() = default;

public:
	HRESULT Add_SRV(const _tchar* pTextureFilePath, _uint iNumTextures, TEXTURE_TYPE eTextureType);
	HRESULT Add_EffeectObject(class CEffectObject* pEffectObject);
	void Add_EffectModelTag(const _wstring& strEffectModelTag) { m_EffectModelTags.push_back(strEffectModelTag); }
	const _wstring& Get_EffectModelTag(_uint iIdx) { return m_EffectModelTags[iIdx]; }
public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);

public:
	void Effect_GUI();
	void Particle_GUI();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	class CEffectObject* m_pEffectObject = { nullptr };
	vector<ID3D11ShaderResourceView*> m_DiffuseSRVs = {};
	vector<ID3D11ShaderResourceView*> m_MaskSRVs = {};
	vector<ID3D11ShaderResourceView*> m_NoiseSRVs = {};

private:
	_char m_szSaveEffectName[MAX_PATH] = {};
	_char m_szLoadEffectName[MAX_PATH] = {};
	_int m_iCurrentTagIndex = {};
	vector<_wstring> m_EffectModelTags = {};

	_bool m_IsEffectVisible = { false };

	/* EFFECT (MESH) */
	_wstring m_strSelectedEffectModelTag = { TEXT("") };
	_float m_fDeltaU = { 0.f }, m_fDeltaV = { 0.f };
	_uint  m_iNumWidth = { 1 }, m_iNumHeight = { 1 };
	_uint  m_iCurrentIdx = { 0 };
	_uint  m_iTextureNum = { 0 }, m_iMaskTextureNum = { 0 }, m_iNoiseTextureNum = { 0 };
	_float m_fFrameTime = { 0.1f };

	/* PARTICLE */
	_int   m_iType = { 0 };
	_bool	m_isLoop = { false };
	_int	m_iNumInstance = {};
	_float3 m_vCenter = {};
	_float3 m_vRange = {};
	_float2 m_vSize = {};
	_float3 m_vPivot = {};
	_float2 m_vSpeed = {};
	_float4 m_vColor = {};
	_float3 m_vRotation = {};
	_float2 m_vLifeTime = {};

public:
	static CEffect_GUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END