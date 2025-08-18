#pragma once

#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Client)

class COutfitSelectCamera : public CCamera
{
public:
	enum class SELECT_STATE {
		HEAD,
		UPPER,
		LOWER,
		BODY,
		END
	};

private:
	COutfitSelectCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	COutfitSelectCamera(const COutfitSelectCamera& rhs);
	virtual ~COutfitSelectCamera() = default;

public:
	void Key_Input();

	void Set_SelectState(SELECT_STATE eState) {
		m_eSelectState = eState;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
private:
	SELECT_STATE m_eSelectState = { SELECT_STATE::BODY };

private:
	void Camera_Action(_float fTimeDelta);

public:
	static COutfitSelectCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END