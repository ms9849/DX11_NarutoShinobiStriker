#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CCamera;
NS_END

NS_BEGIN(Client)

class CCamera_Manager final: public CBase
{
	DECLARE_SINGLETON(CCamera_Manager);

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT Initialize_CameraManager();
	void Update(_float fTimeDelta);

public:
	HRESULT Add_Camera(const _wstring& strCameraTag, class CCamera* pCamera);
	HRESULT Change_Camera(const _wstring& strCameraTag);

private:
	map<_wstring, class CCamera*> m_Cameras;
	class CCamera* m_ActivatedCamera = {};
	class CGameInstance* m_pGameInstance = {};
public:
	void Release_CameraManager();
	virtual void Free() override;
};

NS_END