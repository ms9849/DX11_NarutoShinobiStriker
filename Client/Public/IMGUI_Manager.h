#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CIMGUI_Manager final : public CBase
{
private:
	CIMGUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CIMGUI_Manager() = default;

public:
	HRESULT Initialize();
	void	Update_Imgui();
	void	Render_Imgui();

private:
	HRESULT Ready_Imgui();
	void	Release_Imgui();

private:
	//여기서 IMGUI와 관련된 기능들을 추가해줄 것.

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CIMGUI_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
