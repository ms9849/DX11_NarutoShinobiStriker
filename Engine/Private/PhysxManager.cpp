#include "PhysxManager.h"

CPhysxManager::CPhysxManager()
{
}

HRESULT CPhysxManager::Initialize()
{
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

    /*
    Å×½ºÆ®
    */

	m_pTestMaterial = m_PxPhysx->createMaterial(1.0f, 1.0f, 1.0f);
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*m_PxPhysx, physx::PxPlane(0, 1, 0, 0), *m_pTestMaterial);
	m_PxScene->addActor(*groundPlane);

    return S_OK;
}

void CPhysxManager::Add_Object_ToPhysx(CTransform* pTransform)
{
}

void CPhysxManager::Add_Mesh_ToPhysx(const _wstring strMeshTag, CModel* pModel)
{
}

void CPhysxManager::MeshCollision(_float fTimeDelta)
{
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
    
    if (m_PxScene)      m_PxScene->release();
    if (m_PxDispatcher) m_PxDispatcher->release();
    if (m_PxPhysx)      m_PxPhysx->release();
    if (m_Pvd)          m_Pvd->release();
    if (m_PxFoundation) m_PxFoundation->release();

}
