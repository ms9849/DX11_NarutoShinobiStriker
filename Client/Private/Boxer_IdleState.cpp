#include "Boxer_IdleState.h"

CBoxer_IdleState::CBoxer_IdleState(CTransform* pTransform, CNavigation* pNavigation, CModel* pModelCom)
    : m_pTransformCom { pTransform}
    , m_pNavigationCom { pNavigation }
    , m_pModelCom { pModelCom }
{
    Safe_AddRef(m_pTransformCom);
    Safe_AddRef(m_pNavigationCom);
    Safe_AddRef(m_pModelCom);
}

void CBoxer_IdleState::Start(_bool IsBlend)
{
}

CBoxerState* CBoxer_IdleState::Update(_float fTimeDelta)
{
    return nullptr;
}

_bool CBoxer_IdleState::End()
{
    return _bool();
}

CBoxer_IdleState* CBoxer_IdleState::Create(CTransform* pTransform, CNavigation* pNavigation, CModel* pModelCom)
{
    return new CBoxer_IdleState(pTransform, pNavigation, pModelCom);
}

void CBoxer_IdleState::Free()
{
    __super::Free();

    Safe_Release(m_pTransformCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pModelCom);
}
