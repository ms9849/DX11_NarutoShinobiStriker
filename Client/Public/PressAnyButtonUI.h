#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CVIBuffer;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CPressAnyButtonUI final : public CUIObject
{
private:
	CPressAnyButtonUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CPressAnyButtonUI(const CPressAnyButtonUI& rhs);
	virtual ~CPressAnyButtonUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Play_Animation(_float fTimeDelta);

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	/* 애니메이션 재생 및 플레이 끝남을 확인하기 위함 .*/
	_uint m_iTextureIdx = { 0 };
	_bool m_bTriggered = { false };
	_float m_fTimeAcc = { 0 };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
public:
	static CPressAnyButtonUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
