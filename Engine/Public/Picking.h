#pragma once

#include "Base.h"

/*
피킹을 담당하는 클래스.

피킹 된 물체도 가져올 수 있게 해야하나?

-> 피킹 클래스 내부에 Picking List 작성.
-> 일단은 레벨에 따라 나눠지게만 작성. 추후 필요에 따라 피킹할 객체 타입 구분

-> 이후 클릭 들어오면 Check_PickingLists 등의 메서드로 피킹 체크해줄 것

-> 피킹할 객체는 게임인스턴스를 통해 추가해줄 수 있게 한다.
*/
NS_BEGIN(Engine)

class CPicking final : public CBase
{
	friend class CIMGUI_Manager;
private:
	CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY, _uint iNumLevels);
	void Update();
	void Transform_ToLocalSpace(const _float4x4* pWorldMatrixInverse);

	_bool Picking_InWorldSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);
	_bool Picking_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);

public:
	HRESULT Add_GameObject_ToPicking(class CGameObject* pGameObject, _uint iLevelIdx);
	void Clear(_uint iLevelIndex);

private:
	class CGameInstance*				m_pGameInstance = {};
	ID3D11Device*						m_pDevice = {};
	ID3D11DeviceContext*				m_pContext = {};
	HWND								m_hWnd = {};

	_uint								m_iWinSizeX{}, m_iWinSizeY{};
	_uint								m_iNumLevels = {};

	_float3								m_vRayPos[ENUM_CLASS(RAY::END)];
	_float3								m_vRayDir[ENUM_CLASS(RAY::END)];
	
	class CGameObject*					m_pPickedObject = { nullptr };

	/* map으로 고쳐야 한다 */
	list<class CGameObject*>*			m_pPickingTargets = {};

public:
	static CPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWinSizeX, _uint iWinSizeY, HWND hWnd, _uint iNumLevels);
	virtual void Free() override;
};

NS_END