#include "WoodHand.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Parts_WoodHand.h"
#include "Player.h"

CWoodHand::CWoodHand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CContainerObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CWoodHand::CWoodHand(const CWoodHand& rhs)
    : CContainerObject{ rhs }
{
}

void CWoodHand::Attack()
{
	for (auto& pPart : m_PartObjects)
	{
		CParts_WoodHand* pWoodHand = dynamic_cast<CParts_WoodHand*>(pPart.second);
		if (nullptr != pWoodHand)
			pWoodHand->Attack();
	}
}

HRESULT CWoodHand::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWoodHand::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	WOODHAND_DESC* pDesc = static_cast<WOODHAND_DESC*>(pArg);
    m_vTargetPos = pDesc->vTargetPos;
  

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vJetsuPos), 1.f));

	m_pTransformCom->LookAt_XZ(CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION));

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMVectorSetY(XMLoadFloat3(&m_vTargetPos), m_vTargetPos.y - 5.f), 1.f));


    if (FAILED(Ready_Childs()))
        return E_FAIL;

    return S_OK;
}

void CWoodHand::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CWoodHand::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
    

    m_IsAttackFinished = static_cast<CParts_WoodHand*>(Find_PartObject(TEXT("Part_Hand_L")))->IsAttackFinished();
    _float fHeight = XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION));
	m_IsMoveFinished = fHeight >= m_vTargetPos.y;

    /* 위로 올라와서 */
    if (false == m_IsMoveFinished)
    {
        m_pTransformCom->Go_Direction(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta / 2.f);
    }
    /* 공격 한번 하고 */
	else if (true == m_IsMoveFinished && false == m_IsAttackFinished)
    {
        Attack();
    }
    /* 공격 애니 끝나고 2초후 사라지게 */
    else if (true == m_IsMoveFinished && true == m_IsAttackFinished)
    {
        m_fTimeAcc += fTimeDelta;

        if (m_fTimeAcc >= 3.f)
            m_isDead = true;
    }
}

void CWoodHand::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CWoodHand::Render()
{
    return S_OK;
}

HRESULT CWoodHand::Ready_Components()
{
    return S_OK;
}

HRESULT CWoodHand::Ready_Childs()
{
    CParts_WoodHand::PARTS_WOODHAND_DESC Desc{};
    Desc.pParentTransform = m_pTransformCom;
	Desc.eHandType = CParts_WoodHand::HAND_TYPE::LEFT;


    /* Part_Hand_L */
    if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Part_WoodHand"),
        TEXT("Part_Hand_L"), &Desc)))
        return E_FAIL;

    Desc.eHandType = CParts_WoodHand::HAND_TYPE::RIGHT;

    /* Part_Hand_R */
    if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Part_WoodHand"),
        TEXT("Part_Hand_R"), &Desc)))
        return E_FAIL;

    return S_OK;
}

CWoodHand* CWoodHand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CWoodHand* pInstance = new CWoodHand(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
		MSG_BOX("Create Failed : Wood Hand");
		Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWoodHand::Clone(void* pArg)
{
    CWoodHand* pInstance = new CWoodHand(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : Wood Hand");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWoodHand::Free()
{
    __super::Free();
}
