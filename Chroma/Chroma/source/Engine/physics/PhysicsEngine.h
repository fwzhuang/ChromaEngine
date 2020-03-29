#ifndef _CHROMA_PHYSICS_
#define _CHROMA_PHYSICS_

//common
#include <common/PrecompiledHeader.h>

// bullet
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

// chroma
#include <ChromaConfig.h> 
#include <component/PhysicsComponent.h>
#include <physics/PhysicsDebug.h>
#include <buffer/DebugBuffer.h>
#include <common/CoreCommon.h>

class IEntity;

struct CollisionData
{
	glm::vec3 m_ColliderAContactPoint;
	glm::vec3 m_ColliderBContactPoint;
	glm::vec3 m_ColliderBContactNormal;
};


namespace Chroma
{
	class Physics
	{
	public:

		static void Init();
		static void Update();
		static void ToggleDrawDebug();
		static void DrawDebug();

		static void BindDebugBuffer(DebugBuffer* DebugRenderer);

		static void AddBodyToWorld(btRigidBody*& rigidBody);
		static void RemoveBodyFromWorld(btRigidBody*& rigidBody);

		static void SetGravity(glm::vec3& newGravity);

		// Ray Queries
		static IEntity* GetEntityRayTest(glm::vec3& worldRay_origin, glm::vec3& worldRay_end);
		static bool RayTest(glm::vec3& worldRay_origin, glm::vec3& worldRay_end);
		static bool SphereTest(glm::vec3 const& sphereCenter, float const& sphereRadius);

		// Collision Data
		static std::vector<CollisionData> GetRigidBodyCollisionData(btRigidBody*& rigidBody);

		// utilities
		static float CalcInertiaToReachHeight(float const& targetHeight, float const& gravityMagnitude);

		Physics();
		~Physics();

	private:
		// attrs
		static bool m_DrawDebug;
		static glm::vec3 m_Gravity;
		static float m_TerrainFriction;
		static btCollisionObject* m_CollisionObject;
		static btSphereShape* m_SphereShape;

		// bullet objects
		static btBroadphaseInterface* m_Broadphase;
		static btDefaultCollisionConfiguration* m_CollisionConfiguration;
		static btCollisionDispatcher* m_Dispatcher;
		static btSequentialImpulseConstraintSolver* m_Solver;
		static btDiscreteDynamicsWorld* m_World;

		// debug
		static PhysicsDebug* m_Debug;

		// functions
		static void InitPhysicsWorld();
		static void InitTerrain();
		static void UpdateGravity();
	};
}



#endif
