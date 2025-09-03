#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

/* 입력된 트랙포지션에 맞는 키프레임 반환 */
KEYFRAME CChannel::Get_KeyFrame(_float fTrackPosition)
{
	/* 죵나 위험해보이는데???... */
	if (m_KeyFrames[0].fTrackPosition > fTrackPosition)
		return m_KeyFrames[0];

	for (_uint i = 1; i < m_KeyFrames.size(); ++i)
	{
		if (m_KeyFrames[i].fTrackPosition > fTrackPosition && i != 0)
			return m_KeyFrames[i - 1];
	}

	return m_KeyFrames[m_KeyFrames.size() - 1];
}

HRESULT CChannel::Initialize(const CModel* pModel, const aiNodeAnim* pAIChannel)
{
    strcpy_s(m_szName, pAIChannel->mNodeName.data);
    
    /* 
    뼈 하나만 쓰는 이유? -> 어차피 뼈는 계층구조니까.. 
    최상위 뼈 하나만 알고 있다면 나머진 그 뼈를 최상단 노드로 삼아서 영향을 받게되니
    상관 없어진다.
    */
    m_iBoneIndex = pModel->Get_BoneIndex(m_szName);
	if (-1 == m_iBoneIndex)
		return E_FAIL;

    /* 프레임 키 중 가장 많은거 */
    m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	/* 
	Scale, Rotation, Translation은 루프문 바깥에 생성.
	(이전 키프레임 정보 받아올때 사용하기 유용함.)
	*/
	_float3				vScale{};
	_float4				vRotation{};
	_float3				vTranslation{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME		KeyFrame{};

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));

			/* 
			키프레임에 트랙 포지션 여러번 넣어도 되는 이유 -> 
			어차피 키프레임 번호(i)가 같으면, 키의 종류(크,자,이) 에 상관없이 
			같은 Track Position을 반환하니까.
			*/
			KeyFrame.fTrackPosition = (_float)pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

			KeyFrame.fTrackPosition = pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.push_back(KeyFrame);
	}

    return S_OK;
}

HRESULT CChannel::Initialize(HANDLE hHandle, DWORD* dwByte)
{
	if (FAILED(Load_Channel_FromBinary(hHandle, dwByte)))
		return E_FAIL;
	return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
{
	/* 현재 키프레임이 아예 0이라면, 애니메이션이 시작된 순간밖에 없음. */
	if (0.f == fCurrentTrackPosition)
		*pCurrentKeyFrameIndex = 0;

	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	_vector			vScale{};
	_vector			vRotation{};
	_vector			vTranslation{};

	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}

	else /* 선형보간을 해야겠다. */
	{
		while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
			++*pCurrentKeyFrameIndex;

		_float3		vSourScale{}, vDestScale{};
		_float4		vSourRotation{}, vDestRotation{};
		_float3		vSourTranslation{}, vDestTranslation{};

		vSourScale = m_KeyFrames[*pCurrentKeyFrameIndex].vScale;
		vDestScale = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale;

		vSourRotation = m_KeyFrames[*pCurrentKeyFrameIndex].vRotation;
		vDestRotation = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation;

		vSourTranslation = m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation;
		vDestTranslation = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation;

		_float		fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) /
			(m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&vSourTranslation), XMLoadFloat3(&vDestTranslation), fRatio), 1.f);

	}

	_matrix		BoneTransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_Transformation(BoneTransformationMatrix);


}

void CChannel::Update_Blending_TransformationMatrix(_float BlendRatio, KEYFRAME PreKeyFrame, const vector<class CBone*>& Bones, _float PreTrackPosition)
{
	_vector			vScale{};
	_vector			vRotation{};
	_vector			vTranslation{};

	_float3		vSourScale{}, vDestScale{};
	_float4		vSourRotation{}, vDestRotation{};
	_float3		vSourTranslation{}, vDestTranslation{};

	/* 이전 채널의 정보 가져올 것 */
	vSourScale = PreKeyFrame.vScale;
	vDestScale = m_KeyFrames[0].vScale;

	vSourRotation = PreKeyFrame.vRotation;
	vDestRotation = m_KeyFrames[0].vRotation;

	vSourTranslation = PreKeyFrame.vTranslation;
	vDestTranslation = m_KeyFrames[0].vTranslation;

	vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), BlendRatio);
	vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), BlendRatio);
	vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&vSourTranslation), XMLoadFloat3(&vDestTranslation), BlendRatio), 1.f);


	_matrix		BoneTransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_Transformation(BoneTransformationMatrix);
}

HRESULT CChannel::Save_Channel_ToBinary(HANDLE hHandle, DWORD* dwByte) const
{
	/* 저장할 정보들 */
	/* 1. 접근할 뼈의 이름 (CBone) */
	/* 2. 접근할 뼈의 인덱스 (CBone) */
	/* 3. 총 키프레임의 갯수 */
	/* 4. 키프레임들. */

	/* 뼈의 이름 저장 */
	if (false == WriteFile(hHandle, &m_szName, BONE_MAX, dwByte, nullptr))
		return E_FAIL;
	/* 접근할 뼈의 인덱스 저장 */
	if (false == WriteFile(hHandle, &m_iBoneIndex, sizeof(_int), dwByte, nullptr))
		return E_FAIL;
	/* 키프레임 갯수 저장 */
	if (false == WriteFile(hHandle, &m_iNumKeyFrames, sizeof(_int), dwByte, nullptr))
		return E_FAIL;
	/* 순회하면서 키프레임까지 저장. */
	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		if (false == WriteFile(hHandle, &m_KeyFrames[i], sizeof(KEYFRAME), dwByte, nullptr))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CChannel::Load_Channel_FromBinary(HANDLE hHandle, DWORD* dwByte)
{
	/* 뼈의 이름 저장 */
	if (false == ReadFile(hHandle, &m_szName, BONE_MAX, dwByte, nullptr))
		return E_FAIL;
	/* 접근할 뼈의 인덱스 저장 */
	if (false == ReadFile(hHandle, &m_iBoneIndex, sizeof(_int), dwByte, nullptr))
		return E_FAIL;
	/* 키프레임 갯수 저장 */
	if (false == ReadFile(hHandle, &m_iNumKeyFrames, sizeof(_int), dwByte, nullptr))
		return E_FAIL;
	/* 순회하면서 키프레임까지 저장. */
	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME Dest;

		if (false == ReadFile(hHandle, &Dest, sizeof(KEYFRAME), dwByte, nullptr))
			return E_FAIL;

		m_KeyFrames.push_back(Dest);
	}

	return S_OK;
}

CChannel* CChannel::Create(const CModel* pModel, const aiNodeAnim* pAIChannel)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(pModel, pAIChannel)))
    {
        MSG_BOX("Create Failed : CChannel By Assimp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CChannel* CChannel::Create(HANDLE hHandle, DWORD* dwByte)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(hHandle, dwByte)))
	{
		MSG_BOX("Create Failed : CChannel By Binary");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
    __super::Free();
}
