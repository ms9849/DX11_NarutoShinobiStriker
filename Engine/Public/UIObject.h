#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct tagUIObjectDesc : public GAMEOBJECT_DESC
	{
		_float  fX, fY, fZ;
		_float  fSizeX, fSizeY;
		_uint	iTextureNum = 0;
		_float  fAngle = { 0.f };
	} UIOBJECT_DESC;

	static const tagUIObjectDesc CreateDesc(_float fX, _float fY, _float fZ, _float fSizeX, _float fSizeY, _uint iTextureNum = 0, _float fAngle = 0.f)
	{
		tagUIObjectDesc Desc;

		Desc.fX = fX;
		Desc.fY = fY;
		Desc.fZ = fZ;
		Desc.fSizeX = fSizeX;
		Desc.fSizeY = fSizeY;
		Desc.iTextureNum = iTextureNum;
		Desc.fAngle = fAngle;

		return Desc;
	}
protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iObjectID);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	_float Get_ZOrder() {
		return XMVectorGetZ(m_pTransformCom->Get_State(STATE::POSITION));
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	class COrthogonal*		m_pOrthogonalCom = { nullptr };
	class CTexture*			m_pTextureCom = { nullptr };
	class CVIBuffer_Rect*	m_pVIBufferCom = { nullptr };
	class CShader*			m_pShaderCom = { nullptr };

	// 빌보드 패턴은 안쓸수도 있고, 로직이 긴 편은 아니라서 생략.
	// class CBillBoard* m_pBillBoard = { nullptr };
	_float m_fX{}, m_fY{}, m_fZ{};
	_float m_fSizeX{}, m_fSizeY{};
	_float m_fAngle{};
	_uint  m_iShaderPassIdx = {0};
	_uint  m_iTextureIdx = { 0 };

protected:
	virtual HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END