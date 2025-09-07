#include "Model.h"

#include "GameInstance.h"
#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "Animation.h"
#include "Channel.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
	, m_eType { Prototype.m_eType }
	, m_iNumMeshes { Prototype.m_iNumMeshes }
	, m_MeshNames{ Prototype.m_MeshNames }
	, m_Meshes { Prototype.m_Meshes }
	, m_iNumMaterials { Prototype.m_iNumMaterials }
	, m_Materials { Prototype.m_Materials }
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	, m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
{
	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_Materials)
		Safe_AddRef(pMaterial);

	for (auto& pPair : Prototype.m_Animations)
		m_Animations.emplace(pPair.first, pPair.second->Clone());
}

HRESULT CModel::Load_Model_FromBinary(const _tchar* pBinaryFilePath)
{
	/* ../Bin/Resources/Models/Binary/Fiona.bin */
	_char szModelFilePath[MAX_PATH];

	WideCharToMultiByte(CP_ACP, 0, pBinaryFilePath, (_int)_tcslen(pBinaryFilePath), szModelFilePath, MAX_PATH, NULL, NULL);

	_char szModelPath[MAX_PATH] = {};
	_tchar szPerefectModelName[MAX_PATH] = {};

	DWORD	dwByte(0);
	HANDLE hHandle = CreateFile(pBinaryFilePath,
		GENERIC_READ,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
		NULL,			// 공유 방식(NULL인 경우 공유하지 않음)
		NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
		OPEN_EXISTING,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
		FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
		NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

	if (hHandle == INVALID_HANDLE_VALUE)
		return E_FAIL;

	/* 본 갯수 로딩 */
	if (false == ReadFile(hHandle, &m_iNumBones, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;
	/* 본 정보 로딩 */
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		CBone* pBone = CBone::Create(hHandle, &dwByte);
		m_Bones.push_back(pBone);
	}

	/* 머테리얼 갯수 로딩. */
	if (false == ReadFile(hHandle, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;
	/* 머테리얼 정보 로딩 */
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, hHandle, &dwByte, pBinaryFilePath);
		m_Materials.push_back(pMaterial);
	}

	/* 메쉬 갯수 로딩 */
	if (false == ReadFile(hHandle, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;
	/* 메쉬 이름 로딩 */
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, this, hHandle, &dwByte, XMLoadFloat4x4(&m_PreTransformMatrix));
		m_Meshes.push_back(pMesh);
	}

	/* 애니메이션 갯수 로딩 */
	if (false == ReadFile(hHandle, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;
	/* 애니메이션 로딩 */
	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(hHandle, &dwByte);
		m_Animations.emplace(pAnimation->Get_Name(), pAnimation);
	}

	CloseHandle(hHandle);

	return S_OK;
}

void CModel::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlended, _float fBlendRatio, _bool IsLoop)
{
	if (nullptr == pAnimName)
		return;

	string strAnimName = pAnimName;

	if (strAnimName == m_strCurrentAnimName && false == IsLoop)
		return;

	auto iter = m_Animations.find(strAnimName);
	if (m_Animations.end() == iter)
		return;

	m_fAnimationPlayRate = fAnimationPlayRate;

	if (m_strCurrentAnimName == "")
		m_strCurrentAnimName = strAnimName;

	if (true == IsBlended)
	{
		m_bAnimationBlending = true;
		m_fPreTrackPosition = m_Animations[m_strCurrentAnimName]->Get_CurrentTrackPosition();
		m_PreAnimKeyFrames = m_Animations[m_strCurrentAnimName]->Get_KeyFrames(m_fPreTrackPosition);
		m_fBlendRatio = fBlendRatio;
	}

	m_Animations[m_strCurrentAnimName]->Reset_Animation();
	m_strCurrentAnimName = strAnimName;
	m_Animations[m_strCurrentAnimName]->Reset_Animation();
}


HRESULT CModel::Save_Model_ToBinary(const _char* pModelSavePath)
{
	if (true == m_isBinary)
		return S_OK;

	_char szModelPath[MAX_PATH] = {};
	_tchar szPerefectModelName[MAX_PATH] = {};
	
	/* ../Bin/Resources/ */

	strcpy_s(szModelPath, pModelSavePath);
	
	/* ../Bin/Resources/Fiona.bin */
	strcat_s(szModelPath, m_szModelName);
	strcat_s(szModelPath, ".Bin");
	/*  char to tchar */
	MultiByteToWideChar(CP_ACP, 0, szModelPath, (_int)strlen(szModelPath), szPerefectModelName, MAX_PATH);

	DWORD	dwByte(0);
	HANDLE hHandle = CreateFile(szPerefectModelName, 
								GENERIC_WRITE,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
								NULL,			// 공유 방식(NULL인 경우 공유하지 않음)
								NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
								CREATE_ALWAYS,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
								FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
								NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

	if (hHandle == INVALID_HANDLE_VALUE)
		return E_FAIL;

	/* 본 갯수 저장 */
	WriteFile(hHandle, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < m_iNumBones; ++i)
		if(FAILED(m_Bones[i]->Save_Bone_ToBinary(hHandle, &dwByte)))
			return E_FAIL;

	/* 머테리얼 갯수 저장 */
	WriteFile(hHandle, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);
	/* 머테리얼 정보 저장 */
	for (_uint i = 0; i < m_iNumMaterials; ++i)
		if(FAILED(m_Materials[i]->Save_Material_ToBinary(hHandle, &dwByte, m_pAIScene->mMaterials[i])))
			return E_FAIL;

	/* 메쉬 갯수 저장 */
	WriteFile(hHandle, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);
	/* 메쉬 이름 저장 (메쉬 문자열 크기, 문자열 순). */
	for (_uint i = 0; i < m_iNumMeshes; ++i)
		if(FAILED(m_Meshes[i]->Save_Mesh_ToBinary(hHandle, &dwByte, m_pAIScene->mMeshes[i])))
			return E_FAIL;

	/* 애니메이션 갯수 저장 */
	WriteFile(hHandle, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);
	/* 메쉬 이름 저장 (메쉬 문자열 크기, 문자열 순). */
	for(auto& Pair : m_Animations)
		if (FAILED(Pair.second->Save_Animation_ToBinary(hHandle, &dwByte)))
			return E_FAIL;

	CloseHandle(hHandle);

	return S_OK;
}

_float CModel::Get_CurAnimProgress()
{
	if (MODEL::NONANIM == m_eType)
		return 0.f;

	return m_Animations[m_strCurrentAnimName]->Get_AnimProgress();
}

void CModel::Set_CurAnimProgress(_float fProgress)
{
	if (MODEL::NONANIM == m_eType)
		return;

	m_Animations[m_strCurrentAnimName]->Set_AnimProgress(fProgress);
}

_wstring CModel::Get_MeshName(_uint iIdx) const
{
	return m_Meshes[iIdx]->Get_Name();
}

const _float4x4* CModel::Get_BoneMatrixPtr(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
	{
		if (true == pBone->Compare_Name(pBoneName))
			return true;

		return false;
	});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationMatrixPtr();
}

_int CModel::Get_BoneIndex(const _char* pBoneName) const
{
	_int	iBoneIndex = {};

	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (true == pBone->Compare_Name(pBoneName))
				return true;

			++iBoneIndex;

			return false;
		});
	
	if (iter == m_Bones.end())
		return -1;

	return iBoneIndex;
}

_bool CModel::Picking_Meshes(_fmatrix WolrdMatrixInverse, _float3* vOut)
{
	_float4 vCamPos = *m_pGameInstance->Get_CamState(STATE::POSITION);
	_float  fMinDist = FLT_MAX;
	_float3 vMinOut;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (m_Meshes[i]->Picking(WolrdMatrixInverse, vOut))
		{
			_float fDist = XMVectorGetX(XMVector4Length(XMVectorSet(vOut->x, vOut->y, vOut->z, 1.f) - XMLoadFloat4(&vCamPos)));

			if (fDist < fMinDist)
			{
				fMinDist = fDist;
				vMinOut = *vOut;
			}
		}
	}

	if (fMinDist != FLT_MAX)
	{
		*vOut = vMinOut;
		return true;
	}

	return false;
}

HRESULT CModel::Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	_uint			iFlag = {};

	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (MODEL::NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_eType = eType;
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	_char szFileName[MAX_PATH];
	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	/* Fiona  */
	/* 확장자는 버린다. */
	strcpy_s(m_szModelName, szFileName);

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	m_iNumBones = (_uint)m_Bones.size();

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

// 바이너리로 로딩하는 함수
HRESULT CModel::Initialize_Prototype(MODEL eType, const _tchar* pBinaryFilePath, _fmatrix PreTransformMatrix)
{
	m_eType = eType;
	m_isBinary = true;
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	if(FAILED(Load_Model_FromBinary(pBinaryFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pConstantName)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(m_Bones, pShader, pConstantName);
}

HRESULT CModel::Bind_Material(_uint iMeshIndex, CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex)
{
	/* 머테리얼과 메시 양쪽 다 접근하는 함수. 헷갈릴 수 있으니 순서를 잘 봐둘 것 *
	/*
	1. MeshIndex 번째의 Mesh의 "MaterialIndex"를 가져온다.
	2. 해당 Material Index에 해당하는 Material에 접근하여 Bind_SRV를 수행한다.

		pShader->Bind_SRV(pConstantName, m_SRVs[eType][iTextureIndex]);

	셰이더를 넘겨준 뒤, 어떤 재질 타입 (Diffuse Ambient 등 )의 몇번째 텍스쳐를 바인딩 할 것인지 결정한다.
	*/

	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex]->Bind_SRV(pShader, pConstantName, eType, iTextureIndex);
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
	if (m_Animations.end() == m_Animations.find(m_strCurrentAnimName))
		return false;

	/* 내가 재생하고자하는 애니메이션(공격모션)이 이용하고 있는 뼈들의 상태 변환정보(TransformationMatrix)를 갱신해준다.*/
	if (false == m_bAnimationBlending)
		m_isAnimFinished = m_Animations[m_strCurrentAnimName]->Update_TransformationMatrices(m_Bones, true, fTimeDelta * m_fAnimationPlayRate);

	else if (true == m_bAnimationBlending)
		m_isAnimFinished = m_Animations[m_strCurrentAnimName]->Update_Blending_TransformationMatrices(&m_bAnimationBlending, &m_PreAnimKeyFrames, m_Bones, m_fPreTrackPosition, m_fBlendRatio, fTimeDelta * m_fAnimationPlayRate);


	/* 모든 뼈를 순회하면서 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));

	m_fCurTrackPosition = m_Animations[m_strCurrentAnimName]->Get_CurrentTrackPosition();

	return m_isAnimFinished;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	//여기서 활성화된 메시들만 출력하도록 한다.
	m_Meshes[iMeshIndex]->Bind_Resources();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, this, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_MeshNames.push_back(m_pGameInstance->ToWstring(string(m_pAIScene->mMeshes[i]->mName.C_Str())));
		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, pModelFilePath, m_pAIScene->mMaterials[i]);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_Materials.push_back(pMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_int	iParent = m_Bones.size() - 1;

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
		/* 재귀 형태로 굴러가게 된다. (계층 구조 탐색을 위함) */
		Ready_Bones(pAINode->mChildren[i], iParent);

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(this, m_pAIScene->mAnimations[i]);
		if (nullptr == pAnimation)
			return E_FAIL;

		string strAnimKey = pAnimation->Get_Name();

		//같은 이름의 애니메이션이 이미 존재한다면
		if (m_Animations.end() != m_Animations.find(strAnimKey))
			return E_FAIL;

		m_Animations.emplace(pAnimation->Get_Name(), pAnimation);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Create Failed : CModel (By Assimp)");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _tchar* pBinaryFilePath, _fmatrix PreTransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pBinaryFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Create Failed : CModel (By Binary)");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& Pair : m_Animations)
		Safe_Release(Pair.second);
	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& pMaterial : m_Materials)
		Safe_Release(pMaterial);
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	m_Importer.FreeScene();
}
