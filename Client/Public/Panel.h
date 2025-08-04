#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

/*
Panel -> 그릴 수 있는 사각형 형태의 UI. Panel 본인도 출력할 수 있고, 출력하지 않을 수 있음.
xxx
패널 -> 출력 X / UI 덩어리 같은거임. 인벤토리, 스탯창 등등

Panel 내부의 자식들은 Panel의 위치를 기준으로 삼아 세팅되어야 할 것. 
-> Desc 받아와서 Create 해줄때 Panel의 중점을 기준으로 한 상대적인 위치로 세팅할 것

Panel을 제외한 모든 UI들은 Panel에 종속적이게 될 것.
(텍스트 박스, 버튼 등..)

매니저 필요한가 -> X. 어차피 뿌려줄 UI들은 정해져 있고, 얘넨 수명 정해주면 될 듯

=========================================
타이틀 화면 어떻게 구현?
-> 하나의 Panel에 자식 Panel들 삽입. 
-> 특수한 기능이 있는게 아니라면 Panel을 그대로 사용하도록 한다.
=========================================
현재 필요한 UI 타입
-> 숫자에서 텍스쳐로 매핑시키는 시스템이 필요함.
Timer 클래스 만들어서 숫자 3자리 다 출력시킬 것.

-> 폰트 적용된 텍스트 띄우는 UI

-> UI 블렌딩 + 사이즈 조절하면서 띄우는 애니메이션 컴포넌트

=========================================
*/

NS_BEGIN(Engine)
class CVIBuffer_Rect;
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
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	vector<CUIObject*> m_Childs = {};

protected:
	virtual HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END
