#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END 

NS_BEGIN(Client)

class CLight_GUI : public CBase
{
private:
	CLight_GUI();
	virtual ~CLight_GUI() = default;

public:
	void Update(_float fTimeDelta);
	void Save_Lights();
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_float m_fDiffuseX{}, m_fDiffuseY{}, m_fDiffuseZ{}, m_fDiffuseW{};
	_float m_fAmbientX{}, m_fAmbientY{}, m_fAmbientZ{}, m_fAmbientW{};
	_float  m_fRange{};
	_float4 m_vPosition{}; //피킹으로
public:
	static CLight_GUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END 
/*
		LIGHT			eType;
		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;

		XMFLOAT4		vDirection;
		XMFLOAT4		vPosition;
		float			fRange;
*/