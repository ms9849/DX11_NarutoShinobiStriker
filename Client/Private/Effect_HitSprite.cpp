#include "Effect_HitSprite.h"

#include "GameInstance.h"
#include "GameManager.h"

CEffect_HitSprite::CEffect_HitSprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID)}
{
}

CEffect_HitSprite::CEffect_HitSprite(const CEffect_HitSprite& Prototype)
    : CGameObject{ Prototype }
{
}

void CEffect_HitSprite::Set_Desc(void* pArg)
{
}

HRESULT CEffect_HitSprite::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_HitSprite::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;
    /*
    	typedef struct tagEffectHitSpriteDesc : public GAMEOBJECT_DESC {
		_float4 vPosition = {};
		_float  fLifeTime = {};
		_float  fStartScale = {};
		_float	fDeltaScale = {};
		_float4 vColor = {};
		_bool	IsBlur = {};
	} EFFECT_HIT_SPRITE_DESC;
    */
    EFFECT_HIT_SPRITE_DESC* pDesc = static_cast<EFFECT_HIT_SPRITE_DESC*>(pArg);

    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vPosition));
    m_fLifeTime = pDesc->fLifeTime;
    m_fStartScale = pDesc->fStartScale;
    m_fDeltaScale = pDesc->fDeltaScale;
    m_vMainColor = pDesc->vMainColor;
    m_vSubColor = pDesc->vSubColor;
    m_IsBlur = pDesc->IsBlur;
    m_iTextureNum = pDesc->iTextureNum;

    m_pTransformCom->Set_Scale(m_fStartScale, m_fStartScale, m_fStartScale);

    return S_OK;
}

void CEffect_HitSprite::Priority_Update(_float fTimeDelta)
{
}

void CEffect_HitSprite::Update(_float fTimeDelta)
{
    /* 카메라랑 같은 시점 바라보게끔 설정 */
    _float4x4 CameraWorld = *m_pGameInstance->Get_PipeLine_InverseFloat4x4(D3DTS::VIEW);

    m_pTransformCom->Set_State(STATE::RIGHT, *reinterpret_cast<_vector*>(&CameraWorld.m[0]));
    m_pTransformCom->Set_State(STATE::UP, *reinterpret_cast<_vector*>(&CameraWorld.m[1]));
    m_pTransformCom->Set_State(STATE::LOOK, *reinterpret_cast<_vector*>(&CameraWorld.m[2]));

    m_fTimeAcc += fTimeDelta;

    if (m_fTimeAcc >= m_fLifeTime)
        m_IsDead = true;

    m_pTransformCom->Set_Scale(m_fStartScale + m_fTimeAcc *m_fDeltaScale, m_fStartScale + m_fTimeAcc * m_fDeltaScale, m_fStartScale + m_fTimeAcc * m_fDeltaScale);
}

void CEffect_HitSprite::Late_Update(_float fTimeDelta)
{
    if (true == m_IsDead)
        return;

    m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);

    if (true == m_IsBlur)
        m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);
}

HRESULT CEffect_HitSprite::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(11)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_HitSprite::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_HitEffect"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_HitSprite::Bind_ShaderResources()
{
    /*
        m_fLifeTime = pDesc->fLifeTime;
    m_fStartScale = pDesc->fStartScale;
    m_fDeltaScale = pDesc->fDeltaScale;
    m_vMainColor = pDesc->vMainColor;
    m_vSubColor = pDesc->vSubColor;
    m_IsBlur = pDesc->IsBlur;
    */

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fHitSpriteLifeTime", &m_fLifeTime, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fHitSpriteLifeTimeAcc", &m_fTimeAcc, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vHitSpriteMainColor", &m_vMainColor, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vHitSpriteSubColor", &m_vSubColor, sizeof(_float4))))
        return E_FAIL;


    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
        return E_FAIL;

    return S_OK;
}

CEffect_HitSprite* CEffect_HitSprite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CEffect_HitSprite* pInstance = new CEffect_HitSprite(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CEffect_HitSprite");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_HitSprite::Clone(void* pArg)
{
    CEffect_HitSprite* pInstance = new CEffect_HitSprite(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CEffect_HitSprite");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_HitSprite::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

}
