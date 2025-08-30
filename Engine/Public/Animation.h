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
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const class CModel* pModel, const aiAnimation* pAIAnimation);
	void Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta);

private:
	_char						m_szName[MAX_PATH] = {};
	_float						m_fCurrentTrackPosition = {};
	/* 애니메이션 재생을 위한 전체 길이 */
	_float						m_fDuration = {};
	/* 초당 얼마나 재생되어야하는지 : 재생 속도 */
	_float						m_fTickPerSecond = {};

	_uint						m_iNumChannels = {};
	vector<class CChannel*>		m_Channels;

public:
	static CAnimation* Create(const class CModel* pModel, const aiAnimation* pAIAnimation);
	virtual void Free() override;
};

NS_END