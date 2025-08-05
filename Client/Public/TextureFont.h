#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Client)

class CTextureFont final : public CUIObject
{
private:

public:

private:

public:
	static CTextureFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

NS_END
