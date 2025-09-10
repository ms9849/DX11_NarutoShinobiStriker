#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CParts_Enemy abstract : public CPartObject
{
protected:
	CParts_Enemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CParts_Enemy(const CParts_Enemy& rhs);
	virtual ~CParts_Enemy() = default;

public:
	_float	Get_AnimProgress();
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
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override = 0;
};

NS_END