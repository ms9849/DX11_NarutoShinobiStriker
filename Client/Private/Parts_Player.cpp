#include "Parts_Player.h"

#include "GameInstance.h"

CParts_Player::CParts_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CPartObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CParts_Player::CParts_Player(const CParts_Player& rhs)
	: CPartObject { rhs }
{
}

void CParts_Player::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlend, _float fBlendRatio)
{
	/* 추후 문자열로 수정해야 함. */
	if (nullptr != m_pModelCom)
		m_pModelCom->Set_AnimIndex(pAnimName, fAnimationPlayRate, IsBlend, fBlendRatio);
}

_bool CParts_Player::Play_Animation(_float fTimeDelta)
{
	return m_pModelCom->Play_Animation(fTimeDelta);
}

HRESULT CParts_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParts_Player::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CParts_Player::Priority_Update(_float fTimeDelta)
{
}

void CParts_Player::Update(_float fTimeDelta)
{
}

void CParts_Player::Late_Update(_float fTimeDelta)
{
}

HRESULT CParts_Player::Render()
{
	return S_OK;
}

void CParts_Player::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
