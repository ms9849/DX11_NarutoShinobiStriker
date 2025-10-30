#include "ThousandHandCamera.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Boss.h"
#include "Player.h"
#include "ThousandArm.h"

CThousandHandCamera::CThousandHandCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CCamera{ pDevice, pContext, ENUM_CLASS(eObjectID) }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

CThousandHandCamera::CThousandHandCamera(const CThousandHandCamera& rhs)
    : CCamera{ rhs }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

HRESULT CThousandHandCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CThousandHandCamera::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CThousandHandCamera::Priority_Update(_float fTimeDelta)
{
    if (m_fTimeAcc >= 2.0f)
    {
        m_pGameManager->Change_Camera(LEVEL::KONOHA_VILLAGE, TEXT("Main_Camera"), nullptr);
        //플레이어 활성화도
        m_pGameManager->Get_PlayerPtr()->Set_Visible(true);
        static_cast<CThousandArm*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_ThousandArm"), 0))->Set_Active(true);
        m_fTimeAcc = 0.f;
    }
    else
        m_fTimeAcc += fTimeDelta;

    __super::Bind_Matrices();
}

void CThousandHandCamera::Update(_float fTimeDelta)
{
}

void CThousandHandCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CThousandHandCamera::Render()
{
    return S_OK;
}

void CThousandHandCamera::OnChange(const _float4x4* pWorldMatrix)
{
    m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(pWorldMatrix));

    m_pGameInstance->PlaySoundOnce(TEXT("ThousandHand_Visible.wav"), CHANNELID::EFFECT, 0.6f);
}

CThousandHandCamera* CThousandHandCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CThousandHandCamera* pInstance = new CThousandHandCamera(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CThousandHandCamera");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CThousandHandCamera::Clone(void* pArg)
{
    CThousandHandCamera* pInstance = new CThousandHandCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Create Failed : CThousandHandCamera");
        Safe_Release(pInstance);
    }
    return pInstance;;
}

void CThousandHandCamera::Free()
{
    __super::Free();

    Safe_Release(m_pGameManager);
}
