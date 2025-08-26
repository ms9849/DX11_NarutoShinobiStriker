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
	} VTXMESH;

	typedef struct tagVertexAnimationMesh
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT3			vTangent;
		XMFLOAT2			vTexcoord;

		/* 이 정점이 어떤 뼈들의 상태를 받아야하는가? */
		XMUINT4				vBlendIndex;
		/* 각 뼈의 상태가 어떤 비율로 적용되야할지? */
		XMFLOAT4			vBlendWeight;

		static constexpr unsigned int					iNumElements = { 6 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
	} VTXANIMMESH;

	/*
	$$$$$$$$\ $$\   $$\ $$$$$$$\   $$$$$$\  $$$$$$$\ $$$$$$$$\ 
	$$  _____|$$ |  $$ |$$  __$$\ $$  __$$\ $$  __$$\\__$$  __|
	$$ |      \$$\ $$  |$$ |  $$ |$$ /  $$ |$$ |  $$ |  $$ |   
	$$$$$\     \$$$$  / $$$$$$$  |$$ |  $$ |$$$$$$$  |  $$ |   
	$$  __|    $$  $$<  $$  ____/ $$ |  $$ |$$  __$$<   $$ |   
	$$ |      $$  /\$$\ $$ |      $$ |  $$ |$$ |  $$ |  $$ |   
	$$$$$$$$\ $$ /  $$ |$$ |       $$$$$$  |$$ |  $$ |  $$ |   
	\________|\__|  \__|\__|       \______/ \__|  \__|  \__|                           
	*/

	/* 메쉬 하나의 정보 */
	typedef struct tagExportMeshDesc {
		unsigned int iMaterialIndex;
		unsigned int iNumVertices;
		unsigned int iNumFaces;

		//VTXMESH* pVertices;
		//unsigned int* pIndices;
	} EXPORT_MESH_DESC;

	/* 머테리얼 하나의 정보. */
	typedef struct tagExportMaterialDesc {
		unsigned int iNumSRVs[AI_TEXTURE_TYPE_MAX];
	} EXPORT_MATERIAL_DESC;

	/* 모델 하나의 정보. */
	typedef struct tagExportModelDesc {
		unsigned int iNumMaterials;
		unsigned int iNumMeshes;

	} EXPORT_MODEL_DESC;

	/*
	$$$$$$\ $$\      $$\ $$$$$$$\   $$$$$$\  $$$$$$$\ $$$$$$$$\ 
	\_$$  _|$$$\    $$$ |$$  __$$\ $$  __$$\ $$  __$$\\__$$  __|
	  $$ |  $$$$\  $$$$ |$$ |  $$ |$$ /  $$ |$$ |  $$ |  $$ |   
	  $$ |  $$\$$\$$ $$ |$$$$$$$  |$$ |  $$ |$$$$$$$  |  $$ |   
	  $$ |  $$ \$$$  $$ |$$  ____/ $$ |  $$ |$$  __$$<   $$ |   
	  $$ |  $$ |\$  /$$ |$$ |      $$ |  $$ |$$ |  $$ |  $$ |   
	$$$$$$\ $$ | \_/ $$ |$$ |       $$$$$$  |$$ |  $$ |  $$ |   
	\______|\__|     \__|\__|       \______/ \__|  \__|  \__|   
	*/
	/* 메쉬 하나의 읽어들일 정보 */
	typedef struct tagImportMeshDesc {
		unsigned int iMaterialIndex;
		unsigned int iNumVertices;
		unsigned int iNumFaces;

		VTXMESH* pVertices;
		unsigned int* pIndices;
	} IMPORT_MESH_DESC;

	/* 머테리얼 하나의 읽어들일 정보. */
	typedef struct tagImportMaterialDesc {
		unsigned int iNumSRVs[AI_TEXTURE_TYPE_MAX];
		char szTexturePath[AI_TEXTURE_TYPE_MAX][MAX_PATH];
	} IMPORT_MATERIAL_DESC;

	/* 모델 하나의 읽어들일 정보. */
	typedef struct tagImportModelDesc {
		unsigned int iNumMaterials;
		unsigned int iNumMeshes;

	} IMPORT_MODEL_DESC;
}

#endif // Engine_Struct_h__
