#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct tagUIObjectDesc : public GAMEOBJECT_DESC
	{
		_float  fX, fY, fZ = { 0.f };
		_float  fSizeX, fSizeY;
	} UIOBJECT_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iObjectID);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	class COrthogonal* m_pOrthogonalCom = { nullptr };
	// 빌보드 패턴은 안쓸수도 있잖아.
	// class CBillBoard* m_pBillBoard = { nullptr };
	_float m_fX{}, m_fY{}, m_fZ{};
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

NS_END