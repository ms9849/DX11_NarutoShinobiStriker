#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CPipeLine final : public CBase
{
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	/* 역행렬들도 전부 Update 단에서 세팅해둘 것 */
	/* 뷰행렬 Inverse해서 벡터 4개 뽑아두고 넣어둘 것. Right up look도 필요할 예정.*/
	/* 연산용, 저장용 두 가지 타입 다 반환해주게 함. */
	void Update();

public:
	void Set_Pipeline_Matrix(D3DTS eState, _fmatrix PipeLineMatrix);

	const _float4x4* Get_PipeLine_Float4x4(D3DTS eState);
	_matrix Get_PipeLine_Matrix(D3DTS eState);

	const _float4x4* Get_PipeLine_InverseFloat4x4(D3DTS eState);
	_matrix Get_PipeLine_InverseMatrix(D3DTS eState);

	const _float4* Get_CamState(STATE eState);

private:
	_float4x4	m_PipeLine_Matrices[ENUM_CLASS(D3DTS::END)];
	_float4x4	m_PipeLine_InverseMatrices[ENUM_CLASS(D3DTS::END)];
	_float4		m_vCamStates[ENUM_CLASS(STATE::END)];

public:
	static CPipeLine* Create();
	virtual void Free() override;
};

NS_END
