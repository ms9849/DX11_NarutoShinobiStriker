#include "Light_GUI.h"

#include "Light.h"
#include "GameInstance.h"

/*
		LIGHT			eType;
		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;

		XMFLOAT4		vDirection;
		XMFLOAT4		vPosition;
		float			fRange;
*/

//점광원만 저장 

CLight_GUI::CLight_GUI()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

void CLight_GUI::Update(_float fTimeDelta)
{
	//ImGui::Begin("LIGHT DESC");

	//ImGui::PushItemWidth(80.f);


	//ImGui::InputFloat("DiffuseX", &m_fDiffuseX);
	//ImGui::SameLine();
	//ImGui::InputFloat("DiffuseY", &m_fDiffuseY);
	//ImGui::SameLine();
	//ImGui::InputFloat("DiffuseZ", &m_fDiffuseZ);
	//ImGui::SameLine();
	//ImGui::InputFloat("DiffuseW", &m_fDiffuseW);

	//ImGui::InputFloat("AmbientX", &m_fAmbientX);
	//ImGui::SameLine();
	//ImGui::InputFloat("AmbientY", &m_fAmbientY);
	//ImGui::SameLine();
	//ImGui::InputFloat("AmbientZ", &m_fAmbientZ);
	//ImGui::SameLine();
	//ImGui::InputFloat("AmbientW", &m_fAmbientW);

	//ImGui::InputFloat("Range", &m_fRange);

	//ImGui::PopItemWidth();

	//if (ImGui::Button("Save Lights"))
	//{
	//	Save_Lights();
	//}

	//ImGui::End();


	//if (m_pGameInstance->Key_Down(DIK_F5))
	//{
	//	//여기서 빛 추가
	//	LIGHT_DESC Desc;
	//	Desc.vDiffuse = _float4(m_fAmbientX, m_fAmbientY, m_fAmbientZ, m_fAmbientW);
	//	Desc.vAmbient = _float4(m_fDiffuseX, m_fDiffuseY, m_fDiffuseZ, m_fDiffuseW);
	//	Desc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	//	Desc.fRange = m_fRange;
	//	Desc.eType = LIGHT::SPOT;

	//	_float3 vPos;
	//	m_pGameInstance->Picking(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), &vPos);
	//	XMStoreFloat4(&Desc.vPosition, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));

	//	m_pGameInstance->Add_Light(Desc);
	//}

	//if (m_pGameInstance->Key_Pressing(DIK_LCONTROL) && m_pGameInstance->Key_Down(DIK_Z))
	//{
	//	m_pGameInstance->Pop_Light();
	//}


}

void CLight_GUI::Save_Lights()
{
	/* 핸들 열어서 여기서 저장 */
	/* 로드는 반대로 */

	DWORD	dwByte(0);
	HANDLE hHandle = CreateFile(TEXT("../Bin/LightInfo.bin"),
		GENERIC_WRITE,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
		NULL,			// 공유 방식(NULL인 경우 공유하지 않음)
		NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
		CREATE_ALWAYS,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
		FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
		NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

	if (hHandle == INVALID_HANDLE_VALUE)
		return;

	_int iSize = m_pGameInstance->Get_LightSize();

	WriteFile(hHandle, &iSize, sizeof(_uint), &dwByte, nullptr);

	for (_int i = 0; i < iSize; ++i)
	{
		LIGHT_DESC pDesc = *m_pGameInstance->Get_LightDesc(i);
		WriteFile(hHandle, &pDesc, sizeof(LIGHT_DESC), &dwByte, nullptr);
	}

	CloseHandle(hHandle);
}

CLight_GUI* CLight_GUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new CLight_GUI();
}

void CLight_GUI::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
