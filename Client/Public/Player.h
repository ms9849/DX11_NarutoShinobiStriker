#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

 
// 플레이어에게 필요한 정보
// 1. 스킬 타입 
// 2. 스킬 쿨타임
// 3. 필살기 게이지

// 이걸 굳이 플레이어의 정보 저장소를 따로 만들어서 빼야하나?
// 트랜스폼 같은건 이미 묶여있는데..
// SkillType 클래스 만들어서 구분 가능하게 하고
// Activated Skill Type 반환하게 하고.. (쿨타임 / 게이지도 그렇게 관리 )
// Getter Setter는 어쩔수 없이 열어야함...


//그럼 내일 할 일은...
// 0. 플레이어 작성해서 모델 붙이기
// 1. 스킬 구조 플레이어 내부에 작성
// . 퀘스트 작성 및 UI 묶기

NS_BEGIN(Client)

class CPlayer : public CGameObject
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual ~CPlayer() = default;

public:

private:

public:

};

NS_END
