#pragma once

#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Client)

class CTestCamera final : public CCamera
{
public:
	typedef struct tagTestCameraDesc : public CAMERA_DESC {
		_float fMouseSensitiy;
	} TEST_CAMERA_DESC;

	enum class TESTCAMERA_STATE
	{
		IDLE,
		SLOPE,
	};
private:
	CTestCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CTestCamera(const CTestCamera& rhs);
	virtual ~CTestCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_fMouseSensivity = { 0.f };
	_float m_fSlopeTime = { 1.f };
	_float m_vSlopeDir = { 0.f };
	TESTCAMERA_STATE m_eState = { TESTCAMERA_STATE::IDLE };

public:
	static CTestCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
