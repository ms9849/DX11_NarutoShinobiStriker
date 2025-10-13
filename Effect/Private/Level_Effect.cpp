#include "Level_Effect.h"

#include "GameInstance.h"
#include "Effect_GUI.h"
#include "EffectModel.h"
#include "EffectObject.h"
#include "EffectCamera.h"

CLevel_Effect::CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }	
{
}

HRESULT CLevel_Effect::Initialize()
{
	m_pEffectGUI = CEffect_GUI::Create(m_pDevice, m_pContext);

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Camera()))
		return E_FAIL;

	if (FAILED(Ready_EffectObjects()))
		return E_FAIL;

	CEffectObject* pEffectObject = static_cast<CEffectObject*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_Effect"), 0));
	m_pEffectGUI->Add_EffeectObject(pEffectObject);

	return S_OK;
}

void CLevel_Effect::Update(_float fTimeDelta)
{
	m_pEffectGUI->Update(fTimeDelta);
}

HRESULT CLevel_Effect::Render()
{
	SetWindowText(g_hWnd, TEXT("ÀÌÆåÆ®·¹º§ÀÌºó´Ù."));

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Prototypes()
{
	/* For.Prototype_Component_Shader_VtxEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Shader_VtxEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxEffect.hlsl"), VTXEFFMESH::Elements, VTXEFFMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_EffectModel_Test */
	_fmatrix PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_EffectModel_Test"),
		CEffectModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Client/Bin/Resources/Models/Effect/TestEffect.fbx", PreTransformMatrix))))
		return E_FAIL;


	/* For.Prototype_GameObject_EffectCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_EffectCamera"),
		CEffectCamera::Create(m_pDevice, m_pContext, Client::OBJECTID::MAIN_CAMERA))))
		return E_FAIL;

	/* For.Prototype_GameObject_EffectObject */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_EffectObject"),
		CEffectObject::Create(m_pDevice, m_pContext, Client::OBJECTID::EFFECTOBJECT))))
		return E_FAIL;

#pragma region DIFFUSE
	/* For.Prototype_Component_Texture_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Effect_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Effect/Diffuse/Diffuse%d.png"), 1))))
		return E_FAIL;
	m_pEffectGUI->Add_SRV(TEXT("../../Client/Bin/Resources/Textures/Effect/Diffuse/Diffuse%d.png"), 1, CEffect_GUI::TEXTURE_TYPE::DIFFUSE);
#pragma endregion

#pragma region MASK
	/* For.Prototype_Component_Texture_Effect_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Effect_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Effect/Mask/Mask%d.png"), 1))))
		return E_FAIL;
	m_pEffectGUI->Add_SRV(TEXT("../../Client/Bin/Resources/Textures/Effect/Mask/Mask%d.png"), 1, CEffect_GUI::TEXTURE_TYPE::MASK);
#pragma endregion

#pragma region NOISE
	/* For.Prototype_Component_Texture_Effect_Noise */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Effect_Noise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Effect/Noise/Noise%d.png"), 1))))
		return E_FAIL;
	m_pEffectGUI->Add_SRV(TEXT("../../Client/Bin/Resources/Textures/Effect/Noise/Noise%d.png"), 1, CEffect_GUI::TEXTURE_TYPE::NOISE);
#pragma endregion

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Camera()
{
	CEffectCamera::EFFECT_CAMERA_DESC			CameraDesc{};

	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.vEye = _float4(0.f, 1.f, -1.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensitiy = 0.2f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_EffectCamera"), 
		ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_Camera"), &CameraDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Effect::Ready_EffectObjects()
{
	CEffectObject::EFFECT_OBJECT_DESC Desc;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_EffectObject"),
		ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_Effect"), nullptr)))
		return E_FAIL;

	return S_OK;
}

CLevel_Effect* CLevel_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_Effect* pInstance = new CLevel_Effect(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed : Effect Level");
	}

	return pInstance;
}

void CLevel_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pEffectGUI);
}
