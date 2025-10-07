#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CCamera;
class CTransform;
NS_END

NS_BEGIN(Client)

class CCamera_Manager final : public CBase
{
private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	_wstring    Get_CameraName();
	HRESULT		Add_TargetTransform(CTransform* pTransformCom);
	HRESULT		Add_Camera(LEVEL eLevelID, const _wstring& strCameraTag, CCamera* pCamera);
	HRESULT		Change_Camera(LEVEL eLevelID, const _wstring& strCameraTag, const _float4x4* pWorldMatrix = nullptr);

public:
	void		Clear();

private:
	/* 카메라는 동적으로 추가되는게 아니라
	개발자의 의도에 따라 정적인 갯수만큼 추가 될 것이므로 전부 ENUM CLASS화 한다. */

	map<const _wstring, CCamera*> m_Cameras[ENUM_CLASS(LEVEL::END)] = {};
	vector<CTransform*> m_Targets = {};
	CTransform* m_pTargetTransform = { nullptr };

	CCamera* m_pActivatedCamera = {};
	_wstring m_strActivatedCameraTag = {};

	CGameInstance* m_pGameInstance = { nullptr };

public:
	static CCamera_Manager* Create();
	virtual void Free() override;
};

NS_END
