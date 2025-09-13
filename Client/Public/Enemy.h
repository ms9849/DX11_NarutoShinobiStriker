#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CEnemy abstract : public CContainerObject
{
protected:
	CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

public:
	virtual _float	Get_AnimProgress();
	virtual void	Set_AnimProgress(_float fProgress);
	virtual void	Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate = 1.f, _bool IsBlend = true, _float fBlendRatio = 0.15f, _bool IsLoop = false);
	virtual _bool	Play_Animation(_float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Ready_HPBar();

protected:
	_float m_fCurrentHP = { 100 };
	_float m_fMaxHP = { 200 };
	class CEnemy_HPBar* m_pHPBar = { nullptr };
	class CGameManager* m_pGameManager = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override = 0;
};

NS_END
