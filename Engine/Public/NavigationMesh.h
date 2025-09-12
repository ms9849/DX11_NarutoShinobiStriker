#pragma once

/*
Navigation Mesh ? 

-> 말 그대로 네비와 메시의 역할을 함.
Cell들을 만들고 처리하는 로직이 존재하고.. 플레이어를 태울 수 있을 것.
*/
#include "Navigation.h"

NS_BEGIN(Engine)

class ENGINE_DLL CNavigationMesh final : public CNavigation
{
private:
	CNavigationMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigationMesh(const CNavigationMesh& rhs);
	virtual ~CNavigationMesh() = default;

public:
	virtual void Compute_Height(class CTransform* pTransform) override;

public:
	HRESULT Initialize_Prototype(const _char* pFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	void Update(_fmatrix WorldMatrix) {
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}

	_bool isMove(_fvector vPosition);

private:
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer;

private:
	void SetUp_Neighbors();

public:
	static CNavigationMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
