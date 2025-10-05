#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

/*
직교투영하지 않는 아이콘. 뷰 포트를 벗어나지 않게끔 처리한다.
*/

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CIcon : public CGameObject
{
public:
	typedef struct tagEnemyHPBar {
		class CTransform* pTargetTransform = {};
		_uint iTextureIdx;
	} ENEMY_HPBAR_DESC;
private:
	CIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CIcon(const CIcon& rhs);
	virtual ~CIcon() = default;

public:
	void Set_HP(_float fCurrentHP, _float fMaxHP);
public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CTransform* m_pTargetTransform = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };

	_uint m_iTextureIdx;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CIcon* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
