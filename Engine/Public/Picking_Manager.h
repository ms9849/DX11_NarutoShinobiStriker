#pragma once

#include "Base.h"

/*
피킹을 담당하는 클래스.

피킹 기능 + 피킹 된 오브젝트들에 대한 제어를 담당한다.

피킹 된 물체도 가져올 수 있게 해야하나?

-> 피킹 클래스 내부에 Picking List 작성.
-> 일단은 레벨에 따라 나눠지게만 작성. 추후 필요에 따라 피킹할 객체 타입 구분

-> 이후 클릭 들어오면 Check_PickingLists 등의 메서드로 피킹 체크해줄 것

-> 피킹할 객체는 게임인스턴스를 통해 추가해줄 수 있게 한다.

-> 정말 엔진 단에선 기능만 제공해야 하나?
-> 피킹 기능 자체는 기존과 비슷하게 가져오고 클라단에서 처리하게 해야하는지..
*/

/*
피킹 용도 -> 현재 모작할 게임은 맵 배치용만 사용
+ 추가적으로 사용한다면 터레인 하이트 맵, 오브젝트 피킹 정도.

피킹한 뒤 필요한 것 -> 
1. 피킹된 오브젝트 정보 
2. 피킹 그 자체로 오브젝트 상태 변경

-> 무슨 상태? 트랜스폼, 버퍼, 혹은 셰이더 (강조효과)

피킹된 오브젝트는 어디서 제어? -> 

1. 피킹된 오브젝트 내부 컴포넌트 ( VIBuffer )
2. 클라이언트
3. 피킹 클래스 (현재 이 파일)

1,3번 다 상관없을 것 같다. 다만 2번에서 해야될 일이 생기면 골치아파짐..

예외) Get_PickedGameObject 로 피킹된 오브젝트 가져오게 기능 제공?
*/

NS_BEGIN(Engine)

class CPicking_Manager final : public CBase
{
private:
	CPicking_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking_Manager() = default;

public:
	_float3 Get_RayPos(RAY eRayType) {
		return m_vRayPos[ENUM_CLASS(eRayType)];
	}

	_float3 Get_RayDir(RAY eRayType) {
		return m_vRayDir[ENUM_CLASS(eRayType)];
	}
public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY, _uint iNumLevels);
	void Update();

public:
	_bool Picking(_uint iLevelIdx, _float3* pOut);
	_bool Picking_InWorldSpace(_fvector vPointA, _fvector vPointB, _fvector vPointC, _float3* pOut);
	void Transform_ToLocalSpace(_fmatrix WorldMatrixInverse);
	_bool Picking_InLocalSpace(_fvector vPointA, _fvector vPointB, _fvector vPointC, _float3* pOut);

public:
	/* 레벨 / 객체 / 버퍼 가져옴. 트랜스폼은 직접 꺼내준다. */
	/* 메쉬가 여러개일 수 있으니까 (Model 컴포넌트), CComponent 타입으로 받아올까? */

	HRESULT Add_GameObject_ToPicking(_uint iLevelIdx, class CGameObject* pGameObject, class CVIBuffer* pVIBuffer);
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

	list<pair<class CGameObject *, class CVIBuffer *>>*			m_pPickingTargets = {};

public:
	static CPicking_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWinSizeX, _uint iWinSizeY, HWND hWnd, _uint iNumLevels);
	virtual void Free() override;
};

NS_END