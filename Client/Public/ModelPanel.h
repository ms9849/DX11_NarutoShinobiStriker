#pragma once

#include "Client_Defines.h"
#include "Panel.h"


/* 플레이어의 외형을 결정하는 UI */
/*

1. GameManager를 통해 플레이어의 외형 정보를 넘겨줘야 한다.
2. 버튼들을 매우 많이 들고 있어야 한다. 

선택 가능하게 할 요소

1. 머리 모양
2. 눈동자 모양
3. 상의
4. 하의
5. 얼굴 (마스크류)
6. 장신구 (악세서리)

-> 상의 10종류, 하의 10종류, 한벌옷 10종류 등...
-> 버튼들은 포커스 된 상태로 제어한다. 
-> UI 자체에서 키입력을 받아야할 듯.
-> ESC 누르면 뒤로 가기 등 ..
-> 방향키나 엔터키로 외형 결정하게 해주고.
-> 결정 버튼도 존재해야 한다.

아니면 정말 여러 개의 UI를 비슷하게 만들어서 뿌리기?

-> 패널도 UI니까, Panel 자체에 State 세팅

1. SELECT_PART
2. SELECT_HEAD
3. SELECT_EYE
4. SELECT_UPPER
5. SELECT_LOWER
6. SELECT_MASK
7. SELECT_ACCESSORY

1. 상태가 변할 때 애니메이션 재생하도록 해줌
2. 결정 버튼은 SELECT_PART 일때만 보이게할 것
3. 버튼들은 m_Childs 말고도 ModelPanel만이 가지고 있는 m_ActivatedChilds 로 제어해줄 것.
4. 버튼 종류별로 다 들지 않고, 현재 타입에 따라 몇개의 버튼을 활성화할 것인지 정하고,
텍스트 내용 바꿔주고, 제어하는 부위 바꿔주면 된다.
*/

NS_BEGIN(Client)

class CModelPanel final : public CPanel
{
public:
	enum class SELECT_TYPE { PARTS, HEAD, EYE, UPPER, LOWER, MASK, ACCESSORY, END };

private:
	CModelPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CModelPanel(const CModelPanel& rhs);
	virtual ~CModelPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	/* 0 ~ 8 */
	_int m_iFocusedNum = { 0 };
	SELECT_TYPE m_eSelectType = { SELECT_TYPE::PARTS };
private:
	void Change_FocusedButton(_int iNum);
	void Change_SelectType();
	void Change_Outfits();
	void Key_Input();

private:
	HRESULT Ready_Components();

	HRESULT Bind_ShaderResources();
	HRESULT Ready_Buttons();
	/* 외형 창에서 실제 게임 플레이로 넘어가기 위한 버튼 */
	HRESULT Ready_DecideButton();

public:
	static CModelPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
