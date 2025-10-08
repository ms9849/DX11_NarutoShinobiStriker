#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class COrthogonal;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class COutfitBackGround final : public CGameObject
{
private:
	COutfitBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	COutfitBackGround(const COutfitBackGround& rhs);
	virtual ~COutfitBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	/* vtxpostex*/
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	COrthogonal*	m_pOrthogonalCom = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static COutfitBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

NS_END