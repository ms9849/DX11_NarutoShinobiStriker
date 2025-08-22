#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HINSTANCE			hInstance;
		HWND				hWnd;
		unsigned int		iNumLevels;
		unsigned int		iWinSizeX, iWinSizeY;
		WINMODE				eWindowMode;
	} ENGINE_DESC;

	typedef struct tagVertexPositionTexcoord
	{
		XMFLOAT3				vPosition;
		XMFLOAT2				vTexCoord;
		static constexpr unsigned int iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	} VTXPOSTEX;

	typedef struct tagVertexPositionNormalTexCoor
	{
		XMFLOAT3				vPosition;
		XMFLOAT3				vNormal;
		XMFLOAT2				vTexCoord;
		static constexpr unsigned int iNumElements = { 3 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	} VTXNORTEX;

	/* Pos,Nor,Tangent,TexCoord */
	/* */
	typedef struct tagVertexMesh
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT3			vTangent;
		XMFLOAT2			vTexcoord;

		static constexpr unsigned int					iNumElements = { 4 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXMESH;

	/* Save For Mesh Info */
	typedef struct tagMeshDesc {
		int iMaterialIndex;
		int iNumVertices;
		int iNumFaces;

		VTXMESH* pVertices;
		unsigned int* pIndices;
	} MESH_DESC;

	/* Save For Material Info */
	typedef struct tagMaterialDesc {
		int* iNumSRVs[AI_TEXTURE_TYPE_MAX];
		char** pTexturePath[AI_TEXTURE_TYPE_MAX];
	} MATERIAL_DESC;

	/* Save For Model Info */
	typedef struct tagModelDesc {
		unsigned int iNumMeshes;
		MESH_DESC* pMeshInfo;

		unsigned int iNumMaterials;
		MATERIAL_DESC* pMaterialInfo;

	} MODEL_DESC;
}

#endif // Engine_Struct_h__
