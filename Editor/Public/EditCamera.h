#pragma once

#include "Editor_Defines.h"
#include "Camera.h"


/*
클라단에서 가져온 테스트용 카메라. 기본적인 기능만 들어가있음
*/

NS_BEGIN(Editor)

class CEditCamera final : public CCamera
{
public:
	typedef struct tagEditCameraDesc : public CAMERA_DESC {
		_float fMouseSensitiy;
	} EDIT_CAMERA_DESC;

private:
	CEditCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CEditCamera(const CEditCamera& rhs);
	virtual ~CEditCamera() = default;

public:
	void Activate_Camera(_bool bFlag) {
		m_IsCameraActivated = bFlag;
	}

	virtual void Set_Desc(void* pArg) override;

public:
	void Key_Input(_float fTimeDelta);

public: 
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_fMouseSensivity = { 0.f };
	_float m_fSpeedFactor = { 1.f };

	_bool  m_IsCameraActivated = { true };
public:
	static CEditCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
