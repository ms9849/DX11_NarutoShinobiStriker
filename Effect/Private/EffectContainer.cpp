#include "EffectContainer.h"

#include "EffectObject.h"

CEffectContainer::CEffectContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
    : CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CEffectContainer::CEffectContainer(const CEffectContainer& rhs)
    : CGameObject{ rhs }
{
}

void CEffectContainer::Add_EffectObject(const _wstring& strEffectTag, CEffectObject* pEffectObject)
{
    m_EffectObjects.emplace(strEffectTag, pEffectObject);
    m_pMainEffect->Reset_LifeTime();
    pEffectObject->Reset_LifeTime();

    for (auto& Pair : m_EffectObjects)
    {
        Pair.second->Reset_LifeTime();
    }
}

void CEffectContainer::Add_MainEffect(const _wstring& strEffectTag, CEffectObject* pEffectObject)
{
    m_pMainEffect = pEffectObject;
    m_pMainEffect->Reset_LifeTime();
    pEffectObject->Reset_LifeTime();

    for (auto& Pair : m_EffectObjects)
    {
        Pair.second->Reset_LifeTime();
    }
}

void CEffectContainer::Add_ParticleObject(CParticleObject* pParticleObject)
{
    m_ParticleObjects.push_back(pParticleObject);
}

HRESULT CEffectContainer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffectContainer::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CEffectContainer::Priority_Update(_float fTimeDelta)
{
	if (false == m_IsVisible)
		return;

    for (auto& pEffectObject : m_EffectObjects)
    {
        if (pEffectObject.second->Get_StartTime() <= m_pMainEffect->Get_CurLifeTime())
            pEffectObject.second->Priority_Update(fTimeDelta);
    }

    m_pMainEffect->Priority_Update(fTimeDelta);
}

void CEffectContainer::Update(_float fTimeDelta)
{
    if (false == m_IsVisible)
        return;

    for (auto& pEffectObject : m_EffectObjects)
    {
        if(pEffectObject.second->Get_StartTime() <= m_pMainEffect->Get_CurLifeTime())
            pEffectObject.second->Update(fTimeDelta);
    }

    m_pMainEffect->Update(fTimeDelta);
}

void CEffectContainer::Late_Update(_float fTimeDelta)
{
    if (false == m_IsVisible)
        return;

    if (true == m_pMainEffect->IsDead())
        m_IsDead = true;

    for (auto& pEffectObject : m_EffectObjects)
    {
        if (pEffectObject.second->Get_StartTime() <= m_pMainEffect->Get_CurLifeTime())
            pEffectObject.second->Late_Update(fTimeDelta);
    }

    m_pMainEffect->Late_Update(fTimeDelta);
}

HRESULT CEffectContainer::Render()
{
    return S_OK;
}

HRESULT CEffectContainer::Ready_Components()
{
    return S_OK;
}

CEffectContainer* CEffectContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
{
    CEffectContainer* pInstance = new CEffectContainer(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : EffectContainer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffectContainer::Clone(void* pArg)
{
    CEffectContainer* pInstance = new CEffectContainer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Create Failed : EffectContainer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffectContainer::Free()
{
    __super::Free();

	for (auto& pEffectObject : m_EffectObjects)
		Safe_Release(pEffectObject.second);

    m_EffectObjects.clear();

    Safe_Release(m_pMainEffect);
}
