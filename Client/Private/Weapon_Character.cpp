#include "Weapon_Character.h"

#include "GameInstance.h"

CWeapon_Character::CWeapon_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CParts_Character { pDevice, pContext, eObjectID }
{
}

CWeapon_Character::CWeapon_Character(const CWeapon_Character& Prototype)
    : CParts_Character { Prototype }
{
}

void CWeapon_Character::Set_Collider_Active(_bool bFlag)
{
    m_pColliderCom->Set_Active(bFlag);
}

void CWeapon_Character::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlend, _float fBlendRatio, _bool IsLoop)
{
    m_strCurrentAnimName = pAnimName;

    //웨폰은 애님 인덱스 따라 뼈 바꿔줘야 한다.
    // -> 애님 정보가 없음. 상체에서 받아올까?
    if (0 == m_strCurrentAnimName.compare("CustomMan_Attack_SnakeSword_cmb_01")
        && m_pUpper_Player->Get_AnimProgress() >= 0.1f
        && m_pUpper_Player->Get_AnimProgress() <= 0.75f)
    {
        IsAttached = false;
    }

    else if (0 == m_strCurrentAnimName.compare("CustomMan_Attack_SnakeSword_cmb_02")
        && m_pUpper_Player->Get_AnimProgress() <= 0.8f)
    {
        IsAttached = false;
    }

    else if (0 == m_strCurrentAnimName.compare("CustomMan_Attack_SnakeSword_cmb_03")
        && m_pUpper_Player->Get_AnimProgress() <= 0.75f)
    {
        IsAttached = false;
    }

    else
        IsAttached = true;
}

_bool CWeapon_Character::Play_Animation(_float fTimeDelta)
{
    //웨폰은 애님 인덱스 따라 뼈 바꿔줘야 한다.
    // -> 애님 정보가 없음. 상체에서 받아올까?
    if (0 == m_strCurrentAnimName.compare("CustomMan_Attack_SnakeSword_cmb_01")
        && m_pUpper_Player->Get_AnimProgress() >= 0.1f
        && m_pUpper_Player->Get_AnimProgress() <= 0.75f)
        IsAttached = false;

    else if (0 == m_strCurrentAnimName.compare("CustomMan_Attack_SnakeSword_cmb_02")
        && m_pUpper_Player->Get_AnimProgress() <= 0.8f)
        IsAttached = false;

    else if (0 == m_strCurrentAnimName.compare("CustomMan_Attack_SnakeSword_cmb_03")
        && m_pUpper_Player->Get_AnimProgress() <= 0.75f)
        IsAttached = false;

    else
        IsAttached = true;

    return true;
}

void CWeapon_Character::Set_AnimProgress(_float fProgress)
{
    return;
}

HRESULT CWeapon_Character::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon_Character::Initialize(void* pArg)
{
    WEAPON_PLAYER_DESC* pDesc = static_cast<WEAPON_PLAYER_DESC*>(pArg);

    m_eType = pDesc->eType;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pAttachMatrix = pDesc->pAttachMatrix;
    m_pHandMatrix = pDesc->pHandMatrix;
    m_pUpper_Player = pDesc->pUpper_Player;

    Safe_AddRef(m_pUpper_Player);

    return S_OK;
}

void CWeapon_Character::Priority_Update(_float fTimeDelta)
{
}

void CWeapon_Character::Update(_float fTimeDelta)
{
    _matrix		SocketMatrix;

    if (true == IsAttached)
        SocketMatrix = XMLoadFloat4x4(m_pAttachMatrix);
    else
        SocketMatrix = XMLoadFloat4x4(m_pHandMatrix);

    for (size_t i = 0; i < 3; i++)
	    SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    /* 부모 행렬 적용 */
    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

    /* 컴바인드 매트릭스 던져주면서 자연스럽게 크기도 따라가게 됨. */
    m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CWeapon_Character::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

#ifdef _DEBUG
    /* 검일때만 렌더링 */
    if(m_eType == WEAPON_TYPE::SWORD)
        m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CWeapon_Character::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CWeapon_Character::Ready_Components()
{
    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), m_strModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (WEAPON_TYPE::SWORD == m_eType)
    {
        CBounding_OBB::BOUNDING_OBB_DESC		OBBDesc{};

        OBBDesc.vSize = _float3(0.5f, 1.2f, 0.5f);
        OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y * -0.5f, 0.f);
        OBBDesc.vAngles = _float3(0.f, 0.f/*XMConvertToRadians(45.0f)*/, 0.f);
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
            TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
            return E_FAIL;

    }

    else if(WEAPON_TYPE::GLOVE == m_eType)
    {
        CBounding_OBB::BOUNDING_OBB_DESC		OBBDesc{};

        OBBDesc.vSize = _float3(0.3f, 0.3f, 0.3f);
        OBBDesc.vCenter = _float3(0.f, OBBDesc.vSize.y * -0.5f, 0.f);
        OBBDesc.vAngles = _float3(0.f, 0.f/*XMConvertToRadians(45.0f)*/, 0.f);
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
            TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CWeapon_Character::Bind_ShaderResources()
{
    /*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CWeapon_Character* CWeapon_Character::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CWeapon_Character* pInstance = new CWeapon_Character(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed ! : Weapon_Player ");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon_Character::Clone(void* pArg)
{
    CWeapon_Character* pInstance = new CWeapon_Character(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed ! : Weapon_Player ");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon_Character::Free()
{
    __super::Free();

    Safe_Release(m_pUpper_Player);
    Safe_Release(m_pColliderCom);
}
