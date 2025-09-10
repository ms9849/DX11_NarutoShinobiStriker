#include "Parts_Enemy.h"

CParts_Enemy::CParts_Enemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CPartObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CParts_Enemy::CParts_Enemy(const CParts_Enemy& rhs)
    : CPartObject { rhs }
{
}

_float CParts_Enemy::Get_AnimProgress()
{
    //임시로 이렇게 반환
    return 1.f;
}

void CParts_Enemy::Set_AnimProgress(_float fProgress)
{
}

void CParts_Enemy::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlend, _float fBlendRatio, _bool IsLoop)
{
}

_bool CParts_Enemy::Play_Animation(_float fTimeDelta)
{
    return _bool();
}

HRESULT CParts_Enemy::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParts_Enemy::Initialize(void* pArg)
{
    return S_OK;
}

void CParts_Enemy::Priority_Update(_float fTimeDelta)
{
}

void CParts_Enemy::Update(_float fTimeDelta)
{
}

void CParts_Enemy::Late_Update(_float fTimeDelta)
{
}

HRESULT CParts_Enemy::Render()
{
    return S_OK;
}

