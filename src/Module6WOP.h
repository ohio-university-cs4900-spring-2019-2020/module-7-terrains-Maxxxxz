#pragma once

#include <WO.h>
#include "PxActor.h"

using physx::PxActor;

class Module6WOP : public Aftr::WO
{
public:
	//WOMacroDeclaration(Module6WOP, WO);
	static Module6WOP* New(physx::PxPhysics* p, physx::PxScene* s, const std::string& path, Aftr::Vector scale, Aftr::MESH_SHADING_TYPE mst, bool stat, physx::PxVec3 pos);
	static PxActor* getPxActor(physx::PxPhysics* p);
	virtual void setPosition(Aftr::Vector v);
	void updatePoseFromPhysicsEngine(physx::PxActor* actor);
protected:
	Module6WOP(physx::PxPhysics* p, physx::PxScene* s);
	void onCreate(const std::string& path, const Aftr::Vector& scale, Aftr::MESH_SHADING_TYPE mst, bool stat, physx::PxVec3& pos);

	physx::PxPhysics* p;
	physx::PxScene* s;
	physx::PxRigidActor* a;
	
	//Module6WOP* t = this;
};