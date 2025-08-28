#include "Channel.h"
#include "Model.h"

CChannel::CChannel()
{
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
			KeyFrame.fTrackPosition = pAIChannel->mScalingKeys[i].mTime;
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

void CChannel::Update_TransformationMatrix(_float fCurrentTrackPosition)
{
	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	_vector			vScale{};
	_vector			vRotation{};
	_vector			vTranslation{};

	/* 마지막 프레임이라면 이전 키프레임에만 영향을 받아야한다. */
	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}
	/*
	위 케이스가 아니라면 현재 진행중인 키프레임 & 키프레임 + 1,
	두 개의 키프레임으로부터 선형보간 하여 상태 행렬을 뽑아내야 한다.
	*/
	else
	{
		if (fCurrentTrackPosition >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].fTrackPosition)
			++fCurrentTrackPosition;

		_float3		vSourScale{}, vDestScale{};
		_float4		vSourRotation{}, vDestRotation{};
		_float3		vSourTranslation{}, vDestTranslation{};

		vSourScale = m_KeyFrames[m_iCurrentKeyFrameIndex].vScale;
		vDestScale = m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale;

		vSourRotation = m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation;
		vDestRotation = m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation;

		vSourTranslation = m_KeyFrames[m_iCurrentKeyFrameIndex].vTranslation;
		vDestTranslation = m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vTranslation;

		_float		fRatio = (fCurrentTrackPosition - m_KeyFrames[m_iCurrentKeyFrameIndex].fTrackPosition) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[m_iCurrentKeyFrameIndex].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&vSourTranslation), XMLoadFloat3(&vDestTranslation), fRatio), 1.f);

	}

	// _matrix		BoneTransformationMatrix = XMMatrixScaling() * 자전행렬 * ;
	_matrix		BoneTransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	// Bones[m_iBoneIndex]->Set_Transformation(BoneTransformationMatrix);


}

CChannel* CChannel::Create(const CModel* pModel, const aiNodeAnim* pAIChannel)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(pModel, pAIChannel)))
    {
        MSG_BOX("Create Failed : CChannel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChannel::Free()
{
    __super::Free();
}
