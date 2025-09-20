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
	enum class ANIM_STATE { ANIM_IDLE, ANIM_GREET };
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
	void Start_Dialog();
private:
	class CGameManager* m_pGameManager = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	_uint		m_iNumMeshes = {};
	ANIM_STATE	m_eAnimState = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CNPC_KaKashi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
