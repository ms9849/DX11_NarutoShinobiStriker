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

class CEffectContainer final : public CGameObject
{
public:
	typedef struct tagEffectContainerDesc : public GAMEOBJECT_DESC {
		_bool IsDistortion = { false };
		_bool IsBinary = {};
		_wstring strFilePath = {};
		_float fLifeTime = { 300.f };
		_float fSpeedRatio = { 1.f };
	} EFFECT_CONTAINER_DESC;

private:
	CEffectContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CEffectContainer(const CEffectContainer& rhs);
	virtual ~CEffectContainer() = default;

public:
	void Add_To_Distortion();
	void Set_ParentMatrix(_fmatrix ParentMatrix);
	void Set_Position(_fvector vPos);
	void LookAt(_fvector vPos);
	void Rotation(_float fRadianX, _float fRadianY, _float fRadianZ);
	void Set_Visible(_bool bFlag) { m_IsVisible = bFlag; }

	void Add_MainEffect(class CEffectObject* pEffectObject)
	{
		Safe_Release(m_pMainEffect);
		m_pMainEffect = nullptr;

		m_pMainEffect = pEffectObject;
	}

	/* 이펙트든 파티클이든 전부 컨테이너 내부에서 처리해준다. 오브젝트 매니저에서 제어 X */
	void Add_EffectObject(const _wstring& strEffectTag, class CEffectObject* pEffectObject);

	void Add_MainEffect(const _wstring& strEffectTag, class CEffectObject* pEffectObject);

	/* 임시로 구현. 쓸진 모름 */
	void Add_ParticleObject(class CParticleObject* pParticleObject);

	map<_wstring, class CEffectObject*>& Get_Effects()
	{
		return m_EffectObjects;
	}

	/* tag에 맞는 녀석 날리는 함수 */
	void Pop(const _wstring& strEffectTag);

public:
	void Save_Container_ToBinary(const _char* pFilePath);
	void Load_Container_FromBinary(const _tchar* pFilePath);
	void Set_Blur(_bool bFlag);

public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool		m_IsDistortion = { false };
	_float		m_fSpeedRatio = { 0.f };
	_float		m_fLifeTimeAcc = { 0.f };
	_float		m_fLifeTime = { 300.f };
	_bool		m_IsBlur = { false };
	_float4x4	m_ParentWorldMatrix = {};
	_bool		m_IsVisible = { true };
	CEffectObject* m_pMainEffect = { nullptr };
	map<_wstring, class CEffectObject*> m_EffectObjects = {};
	vector<class CParticleObject*> m_ParticleObjects = {};

private:
	HRESULT Ready_Components();

public:
	static CEffectContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
