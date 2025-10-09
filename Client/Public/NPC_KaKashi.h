#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameInstance;
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CNPC_KaKashi : public CGameObject
{
public:
	enum class ANIM_STATE { ANIM_IDLE, ANIM_GREET, ANIM_EXIT };

	typedef struct tagNPCKaKashiDesc {
		_float3 vPosition;
	} NPC_KAKASHI_DESC;

private:
	CNPC_KaKashi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CNPC_KaKashi(const CNPC_KaKashi& rhs);
	virtual ~CNPC_KaKashi() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Kakashi_Transform();
	void Create_Dialog();
	void Start_Dialog();
	void End_Dialog();
	void Check_Talkable();
	void Talk();

private:
	vector<_wstring> m_DialogTexts = {};

	class CIcon* m_pIcon = { nullptr };
	class CGameManager* m_pGameManager = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	_uint		m_iNumMeshes = {};
	ANIM_STATE	m_eAnimState = {};

	_bool		m_IsTalkable = { false };
	_bool		m_IsTalking = { false };
	_int		m_iCurrentDialog = {};
	_int		m_iDialogSize = { 0 };
	_float      m_fTimeAcc = { 0.f };
	_float		m_fDeathCount = { 5.f };
	_bool		m_IsActiveDeath = { false };
private:
	HRESULT Ready_Components();
	HRESULT Ready_KaKashiIcon();
	HRESULT Bind_ShaderResources();

public:
	static CNPC_KaKashi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
