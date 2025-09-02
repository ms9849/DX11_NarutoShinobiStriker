#pragma once

#include "Client_Defines.h"
#include "Base.h"

/*
State 클래스 ? -> FSM을 구현한 상태 제어 클래스.

플레이어의 애니메이션 & 상태를 제어한다.		

입력은 플레이어 단에서 입력받아 처리하게 끔 사용

-> 상태들 생성 후 삭제? 
-> 현재 들고 있는 상태가 다른 상태를 반환해야 교체가 용이하고 플레이어에 로직이 안묶인다.

133 ~ 134페이지 참고.
*/

/* 추상 클래스 단 */
class CCharacterState abstract : public CBase
{
public:
	/* 근데 이거 start가 없는 것도 있는데 어떡하지.. */
	enum class STATE_PHASE{ START, LOOP, END };

protected:
	CCharacterState();
	virtual ~CCharacterState() = default;

public:
	virtual void	Key_Input() = 0;
	/* Start */
	virtual void	Start(_float fTimeDelta) = 0;
	/* Loop */
	virtual void	Update(_float fTimeDelta) = 0;
	/* End */
	virtual void	End(_float fTimeDelta) = 0;

protected:
	STATE_PHASE m_Phase = {};

public:
	virtual void Free() override;
};

