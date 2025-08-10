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
// 0. 플레이어 작성해서 모델 붙이기 -> 완
// 1. 스킬 구조 플레이어 내부에 작성 -> AttackType은 열거형으로 작성. 스킬 두개 (나선환 나선수리검) 작성해서 일단 추가해놓고.. 
// 2. 퀘스트 작성 및 UI 묶기

NS_BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CPlayer final : public CGameObject
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CHARACTER_ATTACK_TYPE eAttackType = { CHARACTER_ATTACK_TYPE::MELEE };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	void Key_Input(_float fTimeDelta);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
