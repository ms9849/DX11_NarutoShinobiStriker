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

class CEffectContainer final : public CGameObject
{
private:
	CEffectContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CEffectContainer(const CEffectContainer& rhs);
	virtual ~CEffectContainer() = default;

public:
	void Set_Visible(_bool bFlag) { m_IsVisible = bFlag; }

	void Add_EffectObject(const _wstring& strEffectTag, class CEffectObject* pEffectObject)
	{
		m_EffectObjects.emplace(strEffectTag, pEffectObject);
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_IsVisible = { true };
	map<_wstring, class CEffectObject*> m_EffectObjects = {};

private:
	HRESULT Ready_Components();

public:
	static CEffectContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
