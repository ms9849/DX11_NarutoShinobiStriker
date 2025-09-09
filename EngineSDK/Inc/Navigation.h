#pragma once

#include "Component.h"

/*
네비게이션 ->
메쉬나 터레인에 태우는 것보다 비용이 훨씬 싸고 
충돌 처리보다도 당연히 훨씬 가격이 싼 편이다.

따라서 Cell (삼각형 하나 단위) 를 여러개 담은 네비게이션을 통해 
*/
NS_BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigaionDesc
	{
		_int				iCurrentCellIndex = { -1 };
	} NAVIGATION_DESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFiles);
	virtual HRESULT Initialize(void* pArg);
	void Update(_fmatrix WorldMatrix) {
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}

	_bool isMove(_fvector vPosition);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	_int							m_iCurrentCellIndex = { -1 };
	vector<class CCell*>			m_Cells;
	static _float4x4				m_WorldMatrix;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

private:
	void SetUp_Neighbors();

public:

	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFiles);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
