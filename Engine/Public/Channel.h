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
	HRESULT Initialize(const class CModel* pModel, const aiNodeAnim* pAIChannel);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition);
	HRESULT Save_Channel_ToBinary(HANDLE hHandle, DWORD* dwByte, const aiNodeAnim* pAIChannel) const;
	HRESULT Load_Channel_ToBinary(HANDLE hHandle, DWORD* dwByte);

private:
	/* 접근할 뼈의 이름 (CBone) */
	_char					m_szName[MAX_PATH] = {};

	/* 접근할 뼈의 인덱스 (CBone) */
	_int					m_iBoneIndex = { -1 };

	/* 총 키프레임의 갯수와 키프레임들. */
	_uint					m_iNumKeyFrames = {};
	vector<KEYFRAME>		m_KeyFrames;

	/* 현재 재생중인 키프레임 */
	_uint					m_iCurrentKeyFrameIndex = { 0 };

public:
	static CChannel* Create(const class CModel* pModel, const aiNodeAnim* pAIChannel);
	virtual void Free() override;
};

NS_END
