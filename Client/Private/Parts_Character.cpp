#include "Parts_Character.h"

#include "GameInstance.h"

CParts_Character::CParts_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CPartObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CParts_Character::CParts_Character(const CParts_Character& rhs)
	: CPartObject { rhs }
{
}

_float CParts_Character::Get_AnimProgress()
{
	return m_pModelCom->Get_CurAnimProgress();
}

void CParts_Character::Set_AnimProgress(_float fProgress)
{
	m_pModelCom->Set_CurAnimProgress(fProgress);
}

void CParts_Character::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlend, _float fBlendRatio, _bool IsLoop)
{
	/* 추후 문자열로 수정해야 함. */
	if (nullptr != m_pModelCom)
		m_pModelCom->Set_AnimIndex(pAnimName, fAnimationPlayRate, IsBlend, fBlendRatio, IsLoop);
}

_bool CParts_Character::Play_Animation(_float fTimeDelta)
{
	return m_pModelCom->Play_Animation(fTimeDelta);
}

HRESULT CParts_Character::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParts_Character::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strModelName = static_cast<CHARACTER_PART_DESC*>(pArg)->strModelName;

	return S_OK;
}

void CParts_Character::Priority_Update(_float fTimeDelta)
{
}

void CParts_Character::Update(_float fTimeDelta)
{
}

void CParts_Character::Late_Update(_float fTimeDelta)
{
}

HRESULT CParts_Character::Render()
{
	return S_OK;
}

void CParts_Character::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
