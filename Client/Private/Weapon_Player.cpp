#include "Weapon_Player.h"

#include "GameInstance.h"

CWeapon_Player::CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CParts_Player { pDevice, pContext, eObjectID }
{
}

CWeapon_Player::CWeapon_Player(const CWeapon_Player& Prototype)
    : CParts_Player { Prototype }
{
}

void CWeapon_Player::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlend, _float fBlendRatio)
{
    m_strCurrentAnimName = pAnimName;

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

}

_bool CWeapon_Player::Play_Animation(_float fTimeDelta)
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

HRESULT CWeapon_Player::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon_Player::Initialize(void* pArg)
{
    WEAPON_PLAYER_DESC* pDesc = static_cast<WEAPON_PLAYER_DESC*>(pArg);

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

void CWeapon_Player::Priority_Update(_float fTimeDelta)
{
}

void CWeapon_Player::Update(_float fTimeDelta)
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
}

void CWeapon_Player::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CWeapon_Player::Render()
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

HRESULT CWeapon_Player::Ready_Components()
{
    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Weapon_Player"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon_Player::Bind_ShaderResources()
{
    /*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    /* 셰이더 & Constant anem 끌고 들어가서 다 세팅? */
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamState(STATE::POSITION), sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CWeapon_Player* CWeapon_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CWeapon_Player* pInstance = new CWeapon_Player(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed ! : Weapon_Player ");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon_Player::Clone(void* pArg)
{
    CWeapon_Player* pInstance = new CWeapon_Player(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed ! : Weapon_Player ");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon_Player::Free()
{
    __super::Free();

    Safe_Release(m_pUpper_Player);
}
