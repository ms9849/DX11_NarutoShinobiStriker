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

void CEffectContainer::Set_ParentMatrix(_fmatrix ParentMatrix)
{
    XMStoreFloat4x4(&m_ParentWorldMatrix, ParentMatrix);
}

void CEffectContainer::Set_Position(_fvector vPos)
{
    m_pMainEffect->Set_Position(vPos);

    for (auto& Pair : m_EffectObjects)
    {
        static_cast<CEffectObject*>(Pair.second)->Set_Position(vPos);
    }

}

void CEffectContainer::LookAt(_fvector vPos)
{
    m_pMainEffect->Get_Transform()->LookAt(vPos);

    for (auto& Pair : m_EffectObjects)
    {
        static_cast<CEffectObject*>(Pair.second)->Get_Transform()->LookAt(vPos);
    }
}

void CEffectContainer::Rotation(_float fRadianX, _float fRadianY, _float fRadianZ)
{
    m_pMainEffect->Get_Transform()->Rotation(fRadianX, fRadianY, fRadianZ);

    for (auto& Pair : m_EffectObjects)
    {
        static_cast<CEffectObject*>(Pair.second)->Get_Transform()->Rotation(fRadianX, fRadianY, fRadianZ);
    }
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

void CEffectContainer::Pop(const _wstring& strEffectTag)
{
    auto iter = m_EffectObjects.find(strEffectTag);

    if (iter != m_EffectObjects.end())
    {
        Safe_Release(iter->second);
        m_EffectObjects.erase(iter);
    }
}

void CEffectContainer::Save_Container_ToBinary(const _char* pFilePath)
{
    _char szEffectPath[MAX_PATH] = {};
    _tchar szPerfectModelName[MAX_PATH] = {};

    strcpy_s(szEffectPath, pFilePath);

    /* ../Bin/Resources/Fiona_eff.bin*/
    strcat_s(szEffectPath, "_eff.bin");
    /*  char to tchar */
    MultiByteToWideChar(CP_ACP, 0, szEffectPath, (_int)strlen(szEffectPath), szPerfectModelName, MAX_PATH);

    DWORD	dwByte(0);
    HANDLE hHandle = CreateFile(szPerfectModelName,
        GENERIC_WRITE,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
        NULL,			// 공유 방식(NULL인 경우 공유하지 않음)
        NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
        CREATE_ALWAYS,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
        FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
        NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

    if (hHandle == INVALID_HANDLE_VALUE)
        return;

    m_pMainEffect->Save_ToBinary(pFilePath, dwByte, hHandle);

    /* 서브 이펙트들 저장 */
    _uint iSize = m_EffectObjects.size();

    WriteFile(hHandle, &iSize, sizeof(size_t), &dwByte, nullptr);

    for (auto& Pair : m_EffectObjects)
    {
        /* 태그 */
        /* 하나의 바이너리에 저장하게 된다. */
        size_t StringSize = Pair.first.size();
        WriteFile(hHandle, &StringSize, sizeof(size_t), &dwByte, nullptr);
        /* 이펙트 스트링을 저장안함. */
        WriteFile(hHandle, Pair.first.c_str(), static_cast<DWORD>(Pair.first.size() * sizeof(wchar_t)), &dwByte, NULL);

        Pair.second->Save_ToBinary(pFilePath, dwByte, hHandle);
    }

    CloseHandle(hHandle);
}

void CEffectContainer::Load_Container_FromBinary(const _tchar* pFilePath)
{
    /* ../Bin/Resources/Models/Binary/Fiona_eff.bin */
    DWORD	dwByte(0);
    HANDLE hHandle = CreateFile(pFilePath,
        GENERIC_READ,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
        FILE_SHARE_READ,			// 공유 방식(NULL인 경우 공유하지 않음)
        NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
        OPEN_EXISTING,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
        FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
        NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

    if (hHandle == INVALID_HANDLE_VALUE)
        return;

    /* 메인 이펙트 로드 */
    m_pMainEffect = CEffectObject::Create(m_pDevice, m_pContext, Client::OBJECTID::EFFECTOBJECT);
    CEffectObject::EFFECT_OBJECT_DESC Desc;
    m_pMainEffect->Initialize(&Desc);
    m_pMainEffect->Load_FromBinary(pFilePath, dwByte, hHandle);

    /* 서브 이펙트 로드 */
    _uint iSize = {};

    ReadFile(hHandle, &iSize, sizeof(size_t), &dwByte, nullptr);

    for (_uint i = 0; i < iSize; ++i)
    {
        /* 모델 태그 */
        size_t StringSize = {};

        ReadFile(hHandle, &StringSize, sizeof(size_t), &dwByte, nullptr);
        _wstring strEffectTag = {};
        strEffectTag.resize(StringSize);

        ReadFile(hHandle, &strEffectTag[0], static_cast<DWORD>(strEffectTag.size() * sizeof(wchar_t)), &dwByte, NULL);
        
        CEffectObject* pEffectObj = CEffectObject::Create(m_pDevice, m_pContext, Client::OBJECTID::EFFECTOBJECT);
        CEffectObject::EFFECT_OBJECT_DESC Desc;
        pEffectObj->Initialize(&Desc);
        pEffectObj->Load_FromBinary(pFilePath, dwByte, hHandle);

        m_EffectObjects.emplace(strEffectTag, pEffectObj);
    }

    CloseHandle(hHandle);
}

void CEffectContainer::Set_Blur(_bool bFlag) 
{
    m_IsBlur = bFlag;
    m_pMainEffect->Set_Blur(bFlag);

    for (auto& pEffectObject : m_EffectObjects)
    {
        static_cast<CEffectObject*>(pEffectObject.second)->Set_Blur(bFlag);
    }
}

HRESULT CEffectContainer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffectContainer::Initialize(void* pArg)
{
    XMStoreFloat4x4(&m_ParentWorldMatrix, XMMatrixIdentity());

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    EFFECT_CONTAINER_DESC* pDesc = static_cast<EFFECT_CONTAINER_DESC*>(pArg);

    m_fLifeTime = pDesc->fLifeTime;
    m_fSpeedRatio = pDesc->fSpeedRatio;

    if (pDesc->IsBinary)
        Load_Container_FromBinary(pDesc->strFilePath.c_str());
    
    return S_OK;
}

void CEffectContainer::Priority_Update(_float fTimeDelta)
{
	if (false == m_IsVisible)
		return;

    m_pMainEffect->Priority_Update(fTimeDelta * m_fSpeedRatio);

    for (auto& pEffectObject : m_EffectObjects)
    {
        if (pEffectObject.second->Get_StartTime() <= m_pMainEffect->Get_CurLifeTime())
            pEffectObject.second->Priority_Update(fTimeDelta * m_fSpeedRatio);
    }
}

void CEffectContainer::Update(_float fTimeDelta)
{
    m_fLifeTimeAcc += fTimeDelta* m_fSpeedRatio;

    if (m_fLifeTimeAcc >= m_fLifeTime)
        m_IsDead = true;

    if (false == m_IsVisible)
        return;

    m_pMainEffect->Update(fTimeDelta * m_fSpeedRatio);

    for (auto& pEffectObject : m_EffectObjects)
    {
        if(pEffectObject.second->Get_StartTime() <= m_pMainEffect->Get_CurLifeTime())
            pEffectObject.second->Update(fTimeDelta * m_fSpeedRatio);
    }
}

void CEffectContainer::Late_Update(_float fTimeDelta)
{
    if (false == m_IsVisible)
        return;

    if (true == m_pMainEffect->IsDead())
        m_IsDead = true;

    /* 부모 행렬 세팅. Late Update 단에서 수행해준다.*/
    m_pMainEffect->Set_ParentMatrix(XMLoadFloat4x4(&m_ParentWorldMatrix));

    for (auto& pEffectObject : m_EffectObjects)
    {
        pEffectObject.second->Set_ParentMatrix(XMLoadFloat4x4(&m_ParentWorldMatrix));
    }

    m_pMainEffect->Late_Update(fTimeDelta * m_fSpeedRatio);

    for (auto& pEffectObject : m_EffectObjects)
    {
        if (pEffectObject.second->Get_StartTime() <= m_pMainEffect->Get_CurLifeTime())
            pEffectObject.second->Late_Update(fTimeDelta * m_fSpeedRatio);
    }

    if (true == m_IsBlur)
    {
        m_pMainEffect->Set_Blur(true);
        for (auto& pEffectObject : m_EffectObjects)
        {
            pEffectObject.second->Set_Blur(true);
        }
    }
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
