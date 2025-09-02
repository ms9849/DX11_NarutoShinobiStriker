#pragma once

#include "Base.h"

/* 시간대별로 취해야할 뼈 하나의 상태들 .*/
/* Update함수에서 내가 취해야할 시간을 넣어주면 시간에 맞는 상태행렬을 생성. */
/* 만든 행렬의 데이터를 CBone 에게 전달해줘야한다. */

NS_BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	_int Get_BoneIndex() {
		return m_iBoneIndex;
	}

	KEYFRAME Get_KeyFrame(_float fTrackPosition);

	_bool Compare_BoneIndex(_int iIdx) {
		return iIdx == m_iBoneIndex;
	}

public:
	HRESULT Initialize(const class CModel* pModel, const aiNodeAnim* pAIChannel);
	HRESULT Initialize(HANDLE hHandle, DWORD* dwByte);

	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex);
	void Update_Blending_TransformationMatrix(_float BlendRatio, KEYFRAME PreKeyFrame, const vector<class CBone*>& Bones, _float PreTrackPosition);
	HRESULT Save_Channel_ToBinary(HANDLE hHandle, DWORD* dwByte, const aiNodeAnim* pAIChannel) const;
	HRESULT Load_Channel_FromBinary(HANDLE hHandle, DWORD* dwByte);

private:
	/* 접근할 뼈의 이름 (CBone) */
	_char					m_szName[BONE_MAX] = {};

	/* 접근할 뼈의 인덱스 (CBone) */
	_int					m_iBoneIndex = { -1 };

	/* 총 키프레임의 갯수와 키프레임들. */
	_uint					m_iNumKeyFrames = {};

	vector<KEYFRAME>		m_KeyFrames;

public:
	static CChannel* Create(const class CModel* pModel, const aiNodeAnim* pAIChannel);
	static CChannel* Create(HANDLE hHandle, DWORD* dwByte);
	virtual void Free() override;
};

NS_END
