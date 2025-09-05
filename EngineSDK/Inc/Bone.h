#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	const _float4x4* Get_CombinedTransformationMatrixPtr() const {
		return &m_CombinedTransformationMatrix;
	}
	
	_bool Compare_Name(const _char* pBoneName) const {
		return !strcmp(pBoneName, m_szName);
	}

	_matrix Get_TransformMatrix() {
		return XMLoadFloat4x4(&m_TransformationMatrix);
	}

	void Set_Transformation(_fmatrix BoneTransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, BoneTransformationMatrix);
	}
public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentIndex);
	HRESULT Initialize(HANDLE hHandle, DWORD* dwByte);

	HRESULT Save_Bone_ToBinary(HANDLE hHandle, DWORD* dwByte);
	HRESULT Load_Bone_FromBinary(HANDLE hHandle, DWORD* dwByte);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);

private:
	_char				m_szName[BONE_MAX] = {};
	_float4x4			m_TransformationMatrix = {}; /* 이 뼈만의 상태변환행렬 */
	_float4x4			m_CombinedTransformationMatrix = {}; /* m_TransformatinoMatrix * Parent`s m_CombinedTransformationMatrix */
	_int				m_iParentBoneIndex = { -1 };
	/*CBone*				m_pParent = { nullptr };*/

public:
	static CBone* Create(const aiNode* pAINode, _int iParentIndex);
	static CBone* Create(HANDLE hHandle, DWORD* dwByte);
	CBone* Clone();
	virtual void Free() override;
};

NS_END
