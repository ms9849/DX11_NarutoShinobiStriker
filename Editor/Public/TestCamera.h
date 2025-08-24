#pragma once

#include "Editor_Defines.h"
#include "Camera.h"


/*
클라단에서 가져온 테스트용 카메라. 기본적인 기능만 들어가있음
*/

NS_BEGIN(Editor)

class CTestCamera final : public CCamera
{
public:
	typedef struct tagTestCameraDesc : public CAMERA_DESC {
		_float fMouseSensitiy;
	} TEST_CAMERA_DESC;

private:
	CTestCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
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

public:
	static CTestCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
