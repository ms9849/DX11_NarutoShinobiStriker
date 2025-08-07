#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

void CPipeLine::Update()
{
	for (_int i = 0; i < ENUM_CLASS(D3DTS::END); ++i)
		XMStoreFloat4x4(&m_PipeLine_InverseMatrices[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_PipeLine_Matrices[i])));

	for (_int i = 0; i < ENUM_CLASS(STATE::END); ++i)
		memcpy(&m_vCamStates[i], m_PipeLine_InverseMatrices[ENUM_CLASS(D3DTS::VIEW)].m[i], sizeof(_float4));
}

void CPipeLine::Set_Pipeline_Matrix(D3DTS eState, _fmatrix PipeLineMatrix)
{
	XMStoreFloat4x4(&m_PipeLine_Matrices[ENUM_CLASS(eState)], PipeLineMatrix);
}

const _float4* CPipeLine::Get_CamState(STATE eState)
{
	return &m_vCamStates[ENUM_CLASS(eState)];
}

const _float4x4* CPipeLine::Get_PipeLine_Float4x4(D3DTS eState)
{
	return &m_PipeLine_Matrices[ENUM_CLASS(eState)];
}

_matrix CPipeLine::Get_PipeLine_Matrix(D3DTS eState)
{
	return XMLoadFloat4x4(&m_PipeLine_Matrices[ENUM_CLASS(eState)]);
}

const _float4x4* CPipeLine::Get_PipeLine_InverseFloat4x4(D3DTS eState)
{
	return &m_PipeLine_InverseMatrices[ENUM_CLASS(eState)];
}

_matrix CPipeLine::Get_PipeLine_InverseMatrix(D3DTS eState)
{
	return XMLoadFloat4x4(&m_PipeLine_InverseMatrices[ENUM_CLASS(eState)]);
}

CPipeLine* CPipeLine::Create()
{
	return new CPipeLine();
}

void CPipeLine::Free()
{
	__super::Free();
}
