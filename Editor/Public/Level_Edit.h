#pragma once

#include "Editor_Defines.h"
#include "Level.h"

NS_BEGIN(Engine)
class CGameObject;
class CTransform;
NS_END

NS_BEGIN(Editor)

class CLevel_Edit final : public CLevel
{
private:
	CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_Edit() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Prototypes();
	/* 이 레벨에서 쓰기위한 객체들을 생성한다. */
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Props(const _wstring& strLayerTag);
public:
	void Editor_GUI();

	void Map_Editor();
	void Show_SelectedObject();
	void Edit_SelectedObject();

	void Model_Editor();
	void Effect_Editor();
	
	void ExportAndImport();
	void Align_Center(const _char* pText);

private:
#pragma region EDITOR
	class CEditCamera*		m_pEditCamera = { nullptr };
	_float					m_fCameraSensivity = {};
	_float					m_fCameraSpeedFactor = {};

	_bool					m_IsCameraOn = { true };
	_float3					m_vScale = { 1.f, 1.f, 1.f };
	_float3					m_vDegrees = { 0.f, 0.f, 0.f };
	_float4					m_vPosition = { 0.f, 0.f, 0.f, 1.f };

	_uint					m_iSelectedMeshNum = {};
	_wstring				m_strLayerMapObjectTag = { TEXT("Layer_Props") };
	_float					m_fMouseActionSpeed = { 1.f };
	_char					m_szClonePrototype[256] = {};
	_char					m_szMapSavePath[MAX_PATH] = {};
	_char					m_szModelSavePath[MAX_PATH] = {};
	vector<_wstring>		m_ModelPrototypeTags = {};

	class CGameObject*		m_pSelectedGameObject = { nullptr };
	class CTransform*		m_pSelectedTransform = { nullptr };
	_uint					m_iSelectedObjectID = {};

	_wstring				m_strSelectedMapName = {};
	_char					m_szMapFileName[MAX_PATH] = {};
	_char					m_szLoadMapFileName[MAX_PATH] = {};
#pragma endregion

	class CMapConverter*		m_MapConverter = { nullptr };
public:
	static CLevel_Edit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END
