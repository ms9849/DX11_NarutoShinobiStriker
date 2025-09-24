#include "PhysxManager.h"

#include "GameObject.h"
#include "Model.h"
#include "Mesh.h"

CPhysxManager::CPhysxManager()
{
}

HRESULT CPhysxManager::Initialize()
{
    /* 피직스 초기화 */
	m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocator, m_DefaultErrorCallback);

    m_Pvd = physx::PxCreatePvd(*m_PxFoundation);

    physx::PxPvdTransport* Transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_Pvd->connect(*Transport, physx::PxPvdInstrumentationFlag::eALL);

    m_PxPhysx = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, physx::PxTolerancesScale(), true, m_Pvd);
	if (nullptr == m_PxPhysx)
		return E_FAIL;

    PxInitExtensions(*m_PxPhysx, m_Pvd);

    physx::PxSceneDesc sceneDesc(m_PxPhysx->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, 0.0f);

    m_PxDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

    sceneDesc.cpuDispatcher = m_PxDispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    m_PxScene = m_PxPhysx->createScene(sceneDesc);

    physx::PxPvdSceneClient* pvdClient = m_PxScene->getScenePvdClient();
    
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    Transport->release();

    return S_OK;
}

void CPhysxManager::Add_GameObject_ToPhysx(CGameObject* pGameObject)
{
    /* 객체의 트랜스폼을 받아와서, 피직스에서 쓸 수 있는 상태로 변환한다. */
    
    /* 
    공식 문서 상에선 피직스 상의 단위 개념은 따로 없고, 
    사용자가 가져다 쓸때 하나의 단위계로만 통일해서 사용하면 
    문제 없을거라고 했음.
    */
    
    _matrix matWorld = XMLoadFloat4x4(pGameObject->Get_Transform()->Get_WorldMatrixPtr());
	_vector vPos, vRotation, vScale;

    XMMatrixDecompose(&vPos, &vRotation, &vScale, matWorld);

    PxVec3 vPxPosition = PxVec3(XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos));
    PxQuat vPxQuaternion = PxQuat(XMVectorGetX(vRotation), XMVectorGetY(vRotation), XMVectorGetZ(vRotation), XMVectorGetW(vRotation));

	PxTransform PxWorldMatrix = PxTransform(vPxPosition, vPxQuaternion);
    PxRigidDynamic* pDynamicActor = m_PxPhysx->createRigidDynamic(PxWorldMatrix);

    /* 머테리얼은 튜토리얼 대로 세팅 */
    PxMaterial* Material = m_PxPhysx->createMaterial(0.5f, 0.5f, 0.6f);
    /* 0.5 반지름 (구), 0.4 반높이 (원통) 크기의 캡슐 콜라이더 세팅. */
    PxShape* pShape = m_PxPhysx->createShape(PxCapsuleGeometry(0.5f, 0.4f), *Material);

    pDynamicActor->attachShape(*pShape);

    pShape->release();
    Material->release();

    m_DynamicActors.push_back(make_pair(pGameObject, pDynamicActor));
    m_PxScene->addActor(*pDynamicActor);
}

/* 지형 처리할 오브젝트 내부에서 Initialize 할때 수행 */
void CPhysxManager::Add_Geometry_ToPhysx(CGameObject* pGameObject, CModel* pModel)
{
    _int iMeshNum = pModel->Get_MeshNum();

    for (_int i = 0; i < iMeshNum; ++i)
    {
        CMesh* pMesh = pModel->Get_Mesh(i);
        _float3* pVertexPositions = pMesh->Get_VertexPositions();
        _uint* pIndices = pMesh->Get_Indices();
        _uint iNumVertices = pMesh->Get_NumVertices();
        _uint iNumIndices = pMesh->Get_NumIndices();
        _uint iNumPrimitives = iNumIndices / 3;

        vector<PxVec3> Vertices;
        Vertices.reserve(iNumVertices);

        _matrix matWorld = XMLoadFloat4x4(pGameObject->Get_Transform()->Get_WorldMatrixPtr());

        /* 정점 정보 뽑아오는 과정 */
        for (_uint i = 0; i < iNumVertices; ++i)
        {
            /* 로컬 좌표 뽑아옴 */
            _vector vMeshLocalPos;
            vMeshLocalPos = XMLoadFloat3(&pVertexPositions[i]);
            /* 뽑아온 로컬 좌표 기반으로 월드 좌표로 올림 */
            _vector vMeshWorldPos = XMVector3TransformCoord(vMeshLocalPos, matWorld);

            /* Vertices 행렬에 PxVec3 형태로 담아줌 */
            Vertices.push_back(PxVec3(XMVectorGetX(vMeshWorldPos), XMVectorGetY(vMeshWorldPos), XMVectorGetX(vMeshWorldPos)));
        }

        /* 인덱스 정보 뽑아오는 과정 */
        vector<PxU32> Indices;
        Indices.reserve(iNumIndices);

        _uint iIndex = { 0 };

        for (_uint i = 0; i < iNumPrimitives; ++i)
        {
            Indices.push_back(pIndices[iIndex++]);
            Indices.push_back(pIndices[iIndex++]);
            Indices.push_back(pIndices[iIndex++]);
        }

        PxTriangleMeshDesc Desc;

        Desc.points.count = iNumVertices;
        Desc.points.stride = sizeof(PxVec3);
        Desc.points.data = Vertices.data();

        Desc.triangles.count = iNumPrimitives;
        Desc.triangles.stride = sizeof(PxU32) * 3;
        Desc.triangles.data = Indices.data();


        PxTriangleMesh* pTriangleMesh = PxCreateTriangleMesh(PxCookingParams(PxTolerancesScale(0.0f, 0.0f)), Desc);
        PxTriangleMeshGeometry* pGeometry = new PxTriangleMeshGeometry(pTriangleMesh);

        /* 지형 완성은 여기서 끝 */
        m_TriangleMeshes.push_back(pTriangleMesh);
        m_Geometries.push_back(pGeometry);
    }
}

void CPhysxManager::Check_GeometryCollision(_float fTimeDelta)
{
    for (auto& Pair : m_DynamicActors)
    {
        /* 계산 하기 전, 트랜스폼 가져와서 actor 최신화. */
        _matrix matWorld = XMLoadFloat4x4(Pair.first->Get_Transform()->Get_WorldMatrixPtr());
        _vector vPos, vRotation, vScale;

        XMMatrixDecompose(&vPos, &vRotation, &vScale, matWorld);

        PxVec3 vPxPosition = PxVec3(XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos));
        PxQuat vPxQuaternion = PxQuat(XMVectorGetX(vRotation), XMVectorGetY(vRotation), XMVectorGetZ(vRotation), XMVectorGetW(vRotation));

        PxTransform PxWorldMatrix = PxTransform(vPxPosition, vPxQuaternion);
        Pair.second->setGlobalPose(PxWorldMatrix);

        ///* 계산 하기 전, 트랜스폼 가져와서 actor 최신화. */
        //_vector vObjectPosition = Pair.first->Get_Transform()->Get_State(STATE::POSITION);
        //PxTransform ActorPos(PxVec3(XMVectorGetX(vObjectPosition), XMVectorGetY(vObjectPosition), XMVectorGetZ(vObjectPosition)),
        //    Pair.second->getGlobalPose().q); // 회전은 기존 Actor 회전 유지

        //Pair.second->setGlobalPose(ActorPos);

        /* Actor에서 현재 위치 / 회전 가져오기 */
        PxTransform ActorTransform = Pair.second->getGlobalPose();

        /* 캡술 콜라이더 정보 꺼내오기 */
        PxShape* pShape = nullptr;
        Pair.second->getShapes(&pShape, 1);

        PxCapsuleGeometry Geom;
        PxGeometryHolder GeomHolder = pShape->getGeometry(); 
        if (GeomHolder.getType() == PxGeometryType::eCAPSULE)
            Geom = GeomHolder.capsule(); 

        /* 월드 기준 캡슐 콜라이더 정보 계산 */
        PxVec3 vPxUp = ActorTransform.q.rotate(PxVec3(0, 1, 0));
        PxVec3 vPxStartPos = ActorTransform.p + vPxUp * -Geom.halfHeight;
        PxVec3 vPxEndPos = ActorTransform.p + vPxUp * Geom.halfHeight;

        /* Triangle Mesh Geometry와 충돌 처리 */
        _bool IsCollision = false;
        _bool IsGround = false;


        for (auto& Mesh : m_Geometries)
        {
            PxVec3 vDir;
            PxReal fLength;
            if (PxComputeTriangleMeshPenetration(vDir, fLength, Geom, ActorTransform, *Mesh, PxTransform(PxIDENTITY::PxIdentity), 1))
            {
                if (fLength > 0.1f)
                {
                    _vector vResultDir = XMVectorSet(vDir.x, vDir.y, vDir.z, 0.f);
                    vResultDir = XMVector3Normalize(vResultDir);

                    // 땅 체크
                    _vector vDown = XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);
                    _float fAngle = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDown, vResultDir))));
                    if (fAngle <= 60.0f)
                        IsGround = true;

                    fLength -= 0.1f;
                    vResultDir *= fLength;

                    // 플레이어 Transform 위치 보정
                    _vector vOriginPos = Pair.first->Get_Transform()->Get_State(STATE::POSITION);
                    _vector vResultPos = vOriginPos + vResultDir;
                    Pair.first->Get_Transform()->Set_State(STATE::POSITION, vResultPos);

                    IsCollision = true;
                }
            }
        }
    }
}

CPhysxManager* CPhysxManager::Create()
{
    CPhysxManager* pInstance = new CPhysxManager();
    if (FAILED(pInstance->Initialize()))
    {
		MSG_BOX("Create Failed : PhysxManager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPhysxManager::Free()
{
    __super::Free();

    PxCloseExtensions();
    
    for (auto& Pair : m_DynamicActors)
        Pair.second->release();
    
    for (auto& Geometry : m_Geometries)
        Safe_Delete(Geometry);

    for (auto& TriangleMesh : m_TriangleMeshes)
        TriangleMesh->release();

    if (nullptr != m_PxScene)      
        m_PxScene->release();

    if (nullptr != m_PxDispatcher)
        m_PxDispatcher->release();

    if (nullptr != m_PxPhysx)
        m_PxPhysx->release();

    if (nullptr != m_Pvd)
        m_Pvd->release();

    if (nullptr != m_PxFoundation)
        m_PxFoundation->release();
}
///* 지형을 실제로 PhysX 씬에 세팅 */
//_matrix matWorld = XMLoadFloat4x4(pGameObject->Get_Transform()->Get_WorldMatrixPtr());
//_vector vPos, vRotation, vScale;

//XMMatrixDecompose(&vPos, &vRotation, &vScale, matWorld);

//PxVec3 vPxPosition = PxVec3(XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos));
//PxQuat vPxQuaternion = PxQuat(XMVectorGetX(vRotation), XMVectorGetY(vRotation), XMVectorGetZ(vRotation), XMVectorGetW(vRotation));

//PxTransform PxWorldMatrix = PxTransform(vPxPosition, vPxQuaternion);

//PxRigidStatic* pActor = m_PxPhysx->createRigidStatic(PxWorldMatrix);

//PxShape* pShape = m_PxPhysx->createShape(*pGeometry, *m_PxPhysx->createMaterial(0.5f, 0.5f, 0.6f));

//pActor->attachShape(*pShape);

//m_PxScene->addActor(*pActor);
