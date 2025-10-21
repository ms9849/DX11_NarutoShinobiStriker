#pragma once

#include "Effect_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Cube;
class CTexture;
class CShader;
NS_END

NS_BEGIN(EffectTool)

class CDepthCube final : public CGameObject
{
private:
	CDepthCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CDepthCube(const CDepthCube& rhs);
	virtual ~CDepthCube() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDepthCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
