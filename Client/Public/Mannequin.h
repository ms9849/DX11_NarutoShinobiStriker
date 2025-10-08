#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

/*
게임 매니저에 세팅해둔 아웃핏 목록들 가져와서
마네킹용 파트 오브젝트에 세팅할 것.
-> 마네킹용 파트오브젝트는 하나로 퉁쳐도 되고, 하나의 애니메이션만을 재생하므로 그리 어렵진 않다.
*/

class CMannequin final : public CContainerObject
{
private:
	CMannequin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CMannequin(const CMannequin& rhs);
	virtual ~CMannequin() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
	_uint  m_iNumMeshes = {};
	_uint m_iActivatedModel[ENUM_CLASS(SELECT_TYPE::END)] = {};

private:
	HRESULT Ready_AllOutfits();
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMannequin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
