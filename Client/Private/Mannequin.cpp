#include "Mannequin.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Part_Mannequin.h"

CMannequin::CMannequin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CContainerObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

CMannequin::CMannequin(const CMannequin& rhs)
	: CContainerObject { rhs }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

void CMannequin::Activate_Parts(SELECT_TYPE eType, _uint iModelNum)
{
	if (SELECT_TYPE::ONE_CLOTH == eType)
		m_IsOneCloth = true;

	if (SELECT_TYPE::UPPER == eType || SELECT_TYPE::LOWER == eType)
		m_IsOneCloth = false;

	m_iActivatedModelNum[ENUM_CLASS(eType)] = iModelNum;
	m_pGameManager->Set_PlayerModelInfo(eType, iModelNum);
}

void CMannequin::Play_Animation(_float fTimeDelta)
{

	/* 활성화된 녀석들에 대해서만 애니 재생 */
	//for (_uint i = 1; i < ENUM_CLASS(SELECT_TYPE::END); ++i)
	//{
	//	if (i == ENUM_CLASS(SELECT_TYPE::ONE_CLOTH) && (false == m_IsOneCloth))
	//		continue;
	//	if ((i == ENUM_CLASS(SELECT_TYPE::UPPER) || i == ENUM_CLASS(SELECT_TYPE::LOWER)) && (true == m_IsOneCloth))
	//		continue;

	//	////if (m_iActivatedModelNum[i] != -1)
	//	//	m_OutFits[i][m_iActivatedModelNum[i]]->Play_Animation(fTimeDelta);
	//}

	for (_uint i = 1; i < ENUM_CLASS(SELECT_TYPE::END); ++i)
		for (_uint j = 0; j < m_OutFits[i].size(); ++j)
			m_OutFits[i][j]->Play_Animation(fTimeDelta);

}

HRESULT CMannequin::Initialize_Prototype()
{
	return  S_OK;
}

HRESULT CMannequin::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	for (_uint i = 0; i < ENUM_CLASS(SELECT_TYPE::END); ++i)
		m_iActivatedModelNum[i] = -1;

	if (FAILED(Ready_AllOutfits()))
		return E_FAIL;

	return S_OK;
}

void CMannequin::Priority_Update(_float fTimeDelta)
{
}

void CMannequin::Update(_float fTimeDelta)
{
	/* 
	원랜 super의 업데이트 & 레이트 업데이트를 호출해야 하지만...
	활성화 된 녀석들만 골라서 쓰고 싶으므로 사용 안하게끔 작성.
	*/
	for (_uint i = 1; i < ENUM_CLASS(SELECT_TYPE::END); ++i)
	{
		if (i == ENUM_CLASS(SELECT_TYPE::ONE_CLOTH) && (false == m_IsOneCloth))
			continue;
		if ((i == ENUM_CLASS(SELECT_TYPE::UPPER) || i == ENUM_CLASS(SELECT_TYPE::LOWER)) && (true == m_IsOneCloth))
			continue;

		if (m_iActivatedModelNum[i] != -1)
			m_OutFits[i][m_iActivatedModelNum[i]]->Update(fTimeDelta);
	}

	Play_Animation(fTimeDelta);
}

void CMannequin::Late_Update(_float fTimeDelta)
{
	for (_uint i = 1; i < ENUM_CLASS(SELECT_TYPE::END); ++i)
	{
		if (i == ENUM_CLASS(SELECT_TYPE::ONE_CLOTH) && (false == m_IsOneCloth))
			continue;
		if ((i == ENUM_CLASS(SELECT_TYPE::UPPER) || i == ENUM_CLASS(SELECT_TYPE::LOWER)) && (true == m_IsOneCloth))
			continue;

		if (m_iActivatedModelNum[i] != -1)
			m_OutFits[i][m_iActivatedModelNum[i]]->Late_Update(fTimeDelta);
	}
}

HRESULT CMannequin::Render()
{
	return S_OK;
}

HRESULT CMannequin::Ready_AllOutfits()
{
	for (_uint i = 1; i < ENUM_CLASS(SELECT_TYPE::END); ++i)
	{
		vector<pair<_wstring, _wstring>> OutFits = m_pGameManager->Get_Outfits(static_cast<SELECT_TYPE>(i));
		
		/*
		생성도 마찬가지로 super 써야하지만 그냥 두고 세팅..
		*/
		for (auto& Pair : OutFits)
		{
			m_iActivatedModelNum[i] = 0;

			CParts_Mannequin* pPart = CParts_Mannequin::Create(m_pDevice, m_pContext, OBJECTID::PART_MANNEQUIN);
			CParts_Mannequin::MANNEQUIN_PART_DESC Desc;
			Desc.strModelName = Pair.second;
			Desc.pParentTransform = m_pTransformCom;

			pPart->Initialize(&Desc);

			m_OutFits[i].push_back(pPart);
		}
	}

	return S_OK;
}

CMannequin* CMannequin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CMannequin* pInstance = new CMannequin(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed ! : CMannequin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMannequin::Clone(void* pArg)
{
	CMannequin* pInstance = new CMannequin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed ! : CMannequin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMannequin::Free()
{
	__super::Free();

	for (auto& iter : m_OutFits)
	{
		for (auto& pPartObject : iter)
		{
			Safe_Release(pPartObject);
		}
		iter.clear();
	}

	Safe_Release(m_pGameManager);
}
