#include "Player.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject{ pDevice, pContext, ENUM_CLASS(eObjectID)}
{
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
}
