#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

/*
-> UI 컴포넌트들 다 UIObject로 올려둘 것.
-> 작성하고 보니 다 같은 컴포넌트들을 공유한다. 
*/
/*
Panel -> 그릴 수 있는 사각형 형태의 UI. Panel 본인도 출력할 수 있고, 출력하지 않을 수 있음.
 -> UI 덩어리 같은거임. 인벤토리, 스탯창 등등.. 
 -> 자신의 위치를 기준으로 하여 자식들의 위치를 세팅해주는 의의가 큼

Panel 내부의 자식들은 Panel의 위치를 기준으로 삼아 세팅되어야 할 것이다. 
 -> Desc 받아와서 Create 해줄때 Panel의 중점을 기준으로 한 상대적인 위치로 세팅할 것

Panel을 제외한 모든 UI들은 Panel에 종속적이게 될 것.
(텍스트 박스, 버튼 등..)

-> 벡터로 하면 뭐가 무슨 ui인지 찾기 어렵지 않음?
-> 어차피 패널 안에 많아봐야 수십개의 UI가 들어갈거고, 
   정말 찾기 어렵다면 해당 패널 클래스 내부에 이넘 클래스 선언해서 찾는게 맞다고 봄.

매니저 필요한가? 
-> X. 어차피 뿌려줄 UI들은 정해져 있고, 그때그때 뿌려줘야하는 UI들은 패널 내에서 뿌려줄 것.

=========================================
타이틀 화면 어떻게 구현?
-> Logo Panel 안에 필요한 UI들을 담아둔다.
-> Title, PressAnyButton 등..
=========================================
현재 필요한 UI 타입

-> 폰트 적용된 텍스트 띄우는 UI
-> 폰트 시스템에 대해서 찾아봐야 함

-> UI 블렌딩 + 사이즈 조절하면서 띄우는 애니메이션 컴포넌트
-> 블랜딩 배우면 들어가야할 듯.

-> 8월 5일자로 일단 UI 기본은 끝.. 타이머와 테스트 패널 작성해서 테스트 성공했음.
-> 필요한 UI들 붙이는건 일단 전체적인 흐름 보고 방학동안 할 것.

방학동안 할 일 
1. 프로그레스 바 만들어서 로딩창에 적용
2. 로고 UI 붙여놓기
=========================================

8월 6일 할 일 

로고 붙여놓고, UI 스케일 애니메이션 + 사라지는 애니메이션 해보기 (블렌딩 대신 테스트로 일단 집어넣고)
*/

NS_BEGIN(Engine)
class CVIBuffer;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CPanel abstract : public CUIObject
{
protected:
	CPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CPanel(const CPanel& rhs);
	virtual ~CPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	vector<CUIObject*> m_Childs = {};
	class CGameManager* m_pGameManager = { nullptr };

protected:
	virtual HRESULT Bind_ShaderResources() override;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END
