#pragma once

/* 애니메이션 하나의 동작. */

/*
Channel 벡터? -> 채널은 해당 애니메이션이 영향을 주는 뼈 하나를 의미함.
(채널이 10개라면 10개의 뼈에 영향을 주는 애니메이션이라는 것.)

따라서 애니메이션을 통해 채널과 통신하고, 채널에 뼈 정보를 넘겨주는 형식으로 작성해야 한다.
*/

#include "Base.h"

NS_BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	/* 애니메이션 이름 반환 */
	const _char* Get_Name() const { return m_szName; }
	/* 현재 Current Position 반환 ( 블렌딩 용 ) */
	_float Get_CurrentTrackPosition();
	void   Set_AnimProgress(_float fProgress);
	/* 애니메이션 진행 척도 반환 */
	_float Get_AnimProgress();

public:
	/* 트랙 포지션 & 키프레임 초기화 */
	void Reset_Animation();
	map<_int, KEYFRAME> Get_KeyFrames(_float fTrackPoistion);

public:
	HRESULT Initialize(const class CModel* pModel, const aiAnimation* pAIAnimation);
	HRESULT Initialize(HANDLE hHandle, DWORD* dwByte);
	_bool	Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta);
	_bool	Update_Blending_TransformationMatrices(_bool* bFlag, map<_int, KEYFRAME>* pPreKeyFrames, const vector<class CBone*>& Bones, _float PreTrakcPosition, _float fBlendRatio, _float fTimeDelta);

	HRESULT Save_Animation_ToBinary(HANDLE hHandle, DWORD* dwByte) const;
	HRESULT Load_Animation_FromBinary(HANDLE hHandle, DWORD* dwByte);

private:
	_char						m_szName[ANIM_MAX] = {};
	_float						m_fCurrentTrackPosition = { 0 };
	/* 애니메이션 재생을 위한 전체 길이 */
	_float						m_fDuration = {};
	/* 초당 얼마나 재생되어야하는지 : 재생 속도 */
	_float						m_fTickPerSecond = {};

	/* 이 채널들이 어느 본을 움직이는지는 모름. 그럼.. 순회해서 찾아야겠지 */
	_uint						m_iNumChannels = {};
	vector<class CChannel*>		m_Channels;
	vector<_uint>				m_CurrentKeyFrameIndices;
	_float						m_fBlendRatio = { 0.f };
public:
	static CAnimation* Create(const class CModel* pModel, const aiAnimation* pAIAnimation);
	static CAnimation* Create(HANDLE hHandle, DWORD* dwByte);
	CAnimation* Clone();
	virtual void Free() override;
};

NS_END