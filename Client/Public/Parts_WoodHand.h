#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CParts_WoodHand final : public CPartObject
{
public:
	enum class HAND_TYPE{ LEFT, RIGHT, END };
	
	typedef struct tagPartsWoodHandDesc : public CPartObject::PARTOBJECT_DESC
	{
		_float3 vTargetPos;
		_float	fTargetHeight;
		HAND_TYPE eHandType;

	} PARTS_WOODHAND_DESC;

private:
	CParts_WoodHand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CParts_WoodHand(const CParts_WoodHand& rhs);
	virtual ~CParts_WoodHand() = default;

public:
	_bool IsAttackFinished() {
		return m_IsAttackFinished;
	}

	void Play_Animation(_float fTimeDelta);
public:
	void Attack();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };

	_uint		m_iNumMeshes = { 0 };
	HAND_TYPE	m_eHandType = {};
	_bool		m_IsAttack = { false };
	_bool		m_IsAttackFinished = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CParts_WoodHand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

///* Com_Shader */
//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
//	return E_FAIL;
//
///* Com_Model */
//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_WhiteJetsu"),
//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
//	return E_FAIL;

NS_END