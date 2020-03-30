#include "GLViewModule_7.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "AftrGLRendererBase.h"
#include "WOGridECEFElevation.h"

//If we want to use way points, we need to include this.
#include "Module_7WayPoints.h"
#include "PxPhysicsAPI.h"
#include "PxPhysicsAPI.h"
#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "PxFoundation.h"
#include "Module6WOP.h"

using namespace Aftr;

GLViewModule_7* GLViewModule_7::New(const std::vector< std::string >& args)
{
	GLViewModule_7* glv = new GLViewModule_7(args);
	glv->init(Aftr::GRAVITY, Vector(0, 0, -1.0f), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE);
	glv->onCreate();
	return glv;
}

GLViewModule_7::GLViewModule_7(const std::vector< std::string >& args) : GLView(args)
{
	//Initialize any member variables that need to be used inside of LoadMap() here.
	//Note: At this point, the Managers are not yet initialized. The Engine initialization
	//occurs immediately after this method returns (see GLViewModule_7::New() for
	//reference). Then the engine invoke's GLView::loadMap() for this module.
	//After loadMap() returns, GLView::onCreate is finally invoked.


	//The order of execution of a module startup:
	//GLView::New() is invoked:
	//    calls GLView::init()
	//       calls GLView::loadMap() (as well as initializing the engine's Managers)
	//    calls GLView::onCreate()

	//GLViewModule_7::onCreate() is invoked after this module's LoadMap() is completed.
}

void GLViewModule_7::onCreate()
{
	//GLViewModule_7::onCreate() is invoked after this module's LoadMap() is completed.
	//At this point, all the managers are initialized. That is, the engine is fully initialized.

	if (this->pe != NULL)
	{
		//optionally, change gravity direction and magnitude here
		//The user could load these values from the module's aftr.conf
		this->pe->setGravityNormalizedVector(Vector(0, 0, -1.0f));
		this->pe->setGravityScalar(Aftr::GRAVITY);
	}
	this->setActorChaseType(STANDARDEZNAV); //Default is STANDARDEZNAV mode
	//this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
	this->getCamera()->setPosition({ 0, 0, 250 });
	//createFloor();
	r = 0;

}

GLViewModule_7::~GLViewModule_7()
{
	if (scene != nullptr)
		scene->release();
	if (f != nullptr)
		f->release();
	if (p != nullptr)
		p->release();
}


void GLViewModule_7::updateWorld()
{
	GLView::updateWorld(); //Just call the parent's update world first.
						   //If you want to add additional functionality, do it after
						   //this call.

	scene->simulate(0.2);
	scene->fetchResults(true);
	{

		physx::PxU32 numActors = 0;

		physx::PxActor** actors = scene->getActiveActors(numActors);

		//make sure you set physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS, true in your scene!

		//poses that have changed since the last update
		for (physx::PxU32 i = 0; i < numActors; ++i)

		{

			physx::PxActor* actor = actors[i];

			Module6WOP* wo = static_cast<Module6WOP*>(actor->userData);

			wo->updatePoseFromPhysicsEngine(actor);		//add this function to your inherited class
			//worldLst->push_back(wo);

		}

	}
}


void GLViewModule_7::onResizeWindow(GLsizei width, GLsizei height)
{
	GLView::onResizeWindow(width, height); //call parent's resize method.
}


void GLViewModule_7::onMouseDown(const SDL_MouseButtonEvent& e)
{
	GLView::onMouseDown(e);
}


void GLViewModule_7::onMouseUp(const SDL_MouseButtonEvent& e)
{
	GLView::onMouseUp(e);
}


void GLViewModule_7::onMouseMove(const SDL_MouseMotionEvent& e)
{
	GLView::onMouseMove(e);
}


void GLViewModule_7::onKeyDown(const SDL_KeyboardEvent& key)
{
	GLView::onKeyDown(key);
	if (key.keysym.sym == SDLK_0)
		this->setNumPhysicsStepsPerRender(1);

	if (key.keysym.sym == SDLK_1)
	{
		//send netmsg which calls createSphere
		createSphere();
	}
}

void GLViewModule_7::createSphere()
{
	float rx, ry = 0;
	srand(r);
	rx = rand() % 5;
	ry = rand() % 5;

	Module6WOP* wo = Module6WOP::New(p, scene, (ManagerEnvironmentConfiguration::getSMM() + "/models/dice_twelveside_outline.wrl"), { 1, 1, 1 }, Aftr::MESH_SHADING_TYPE::mstAUTO, false, PxVec3{ rx, ry, 250 });

	wo->setPosition({ rx, ry, 250 });
	worldLst->push_back(wo);
	if (scene != nullptr)
	{
		r++;
		std::cout << "added shape" << std::endl;
	}
	else
	{
		std::cout << "scene is null" << std::endl;
	}
}

void GLViewModule_7::createFloor()
{
	//PxMaterial* mat = p->createMaterial(.5f, .2f, .2f);
	//PxShape* shape = p->createShape(PxBoxGeometry(100, 100, 1), *mat, true);
	//PxTransform t({ 0, 0, 0 });

	//physx::PxRigidStatic* a = p->createRigidStatic(t);
	//a->attachShape(*shape);
	//a->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
	//scene->addActor(*a);

	float top = 34.2072593790098f;
	float bottom = 33.9980272592999f;
	float left = -118.65234375f;
	float right = -118.443603515625f;
	
	float vert = top - bottom;
	float horz = right - left;
	
	VectorD offset((top + bottom) / 2, (left + right) / 2, 0);
	/*
	centerOfWorld = offset.toVecS().toECEFfromWGS84();
	gravityDirection = centerOfWorld;
	gravityDirection.normalize();
	*/
	VectorD scale = VectorD(0.1f, 0.1f, 0.1f);
	VectorD upperLeft(top, left, 0);
	VectorD lowerRight(bottom, right, 0);

	cook = PxCreateCooking(PX_PHYSICS_VERSION, *f,
		physx::PxCookingParams(physx::PxTolerancesScale()));

	if (!cook)
	{
		std::cout << "Cooking error" << std::endl;
		std::cin.get();
	}

	WO* grid = WOGridECEFElevation::New(upperLeft, lowerRight, 0, offset, scale, ManagerEnvironmentConfiguration::getLMM() + "/geo/home.tif");

	grid->setLabel("grid");

	worldLst->push_back(grid);

	createFloorMesh(grid);
	/*
	for (size_t i = 0; i < grid->getModel()->getModelDataShared()->getModelMeshes().size(); i++)
		grid->getModel()->getModelDataShared()->getModelMeshes().at(i)->getSkin().getMultiTextureSet().at(0) =
		ManagerTexture::loadTexture(ManagerEnvironmentConfiguration::getSMM() +
			"/images/checkboard512x512 - 4.bmp");
			*/
	grid->getModel()->isUsingBlending(false);
	
}

void GLViewModule_7::createFloorMesh(WO* floor)
{
	//make copy of vertex list and index list
	size_t vertexListSize = floor->getModel()->getModelDataShared()->getCompositeVertexList().size();
	size_t indexListSize = floor->getModel()->getModelDataShared()->getCompositeIndexList().size();
	float* vertexListCopy = new float[vertexListSize * 3];
	unsigned int* indicesCopy = new unsigned int[indexListSize];

	for (size_t i = 0; i < vertexListSize; i++)
	{
		vertexListCopy[i * 3 + 0] = floor->getModel()->getModelDataShared()->getCompositeVertexList().at(i).x;
		vertexListCopy[i * 3 + 1] = floor->getModel()->getModelDataShared()->getCompositeVertexList().at(i).y;
		vertexListCopy[i * 3 + 2] = floor->getModel()->getModelDataShared()->getCompositeVertexList().at(i).z;
	}

	for (size_t i = 0; i < indexListSize; i++)
		indicesCopy[i] = floor->getModel()->getModelDataShared()->getCompositeIndexList().at(i);

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<uint32_t>(vertexListSize);
	meshDesc.points.stride = sizeof(float) * 3;
	meshDesc.points.data = vertexListCopy;
	meshDesc.triangles.count = indexListSize / 3;
	meshDesc.triangles.stride = 3 * sizeof(unsigned int);
	meshDesc.triangles.data = indicesCopy;

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;

	bool status = cook->cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (!status)
	{
		std::cout << "Failed to create Triangular mesh" << std::endl;
		std::cin.get();
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* mesh = p->createTriangleMesh(readBuffer);
	
	PxMaterial* gMaterial = p->createMaterial(0.5f, 0.5f, 0.6f);
	PxShape* shape = p->createShape(PxTriangleMeshGeometry(mesh), *gMaterial, true);
	
	PxTransform t({ 0,0,0 });
	PxRigidStatic* a = p->createRigidStatic(t);
	
	bool b = a->attachShape(*shape);
	a->userData = floor;
	scene->addActor(*a);
}

void GLViewModule_7::onKeyUp(const SDL_KeyboardEvent& key)
{
	GLView::onKeyUp(key);
}


void Aftr::GLViewModule_7::loadMap()
{
	f = PxCreateFoundation(PX_PHYSICS_VERSION, a, e);

	//s = new PxSceneDesc(p->getTolerancesScale());

	gPvd = PxCreatePvd(*f);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	p = PxCreateBasePhysics(PX_PHYSICS_VERSION, *f, PxTolerancesScale(), true, gPvd);
	PxSceneDesc s(p->getTolerancesScale());
	s.gravity = PxVec3(0.0f, 0.0f, -5.0f);
	s.flags = PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	d = PxDefaultCpuDispatcherCreate(2);
	s.cpuDispatcher = d;
	s.filterShader = PxDefaultSimulationFilterShader;
	scene = p->createScene(s);

	this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
	this->actorLst = new WorldList();
	this->netLst = new WorldList();

	ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
	ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
	ManagerOpenGLState::enableFrustumCulling = false;
	Axes::isVisible = true;
	this->glRenderer->isUsingShadowMapping(false); //set to TRUE to enable shadow mapping, must be using GL 3.2+

	this->cam->setPosition(15, 15, 10);

	std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");
	std::string wheeledCar(ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl");
	std::string grass(ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl");
	std::string human(ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl");

	//SkyBox Textures readily available
	std::vector< std::string > skyBoxImageNames; //vector to store texture paths
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
	skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg");
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );

	float ga = 0.1f; //Global Ambient Light level for this module
	ManagerLight::setGlobalAmbientLight(aftrColor4f(ga, ga, ga, 1.0f));
	WOLight* light = WOLight::New();
	light->isDirectionalLight(true);
	light->setPosition(Vector(0, 0, 100));
	//Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
	//for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
	light->getModel()->setDisplayMatrix(Mat4::rotateIdentityMat({ 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD));
	light->setLabel("Light");
	worldLst->push_back(light);

	//Create the SkyBox
	WO* wo = WOSkyBox::New(skyBoxImageNames.at(0), this->getCameraPtrPtr());
	wo->setPosition(Vector(0, 0, 0));
	wo->setLabel("Sky Box");
	wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	worldLst->push_back(wo);

	////Create the infinite grass plane (the floor)
	//wo = WO::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
	//wo->setPosition( Vector( 0, 0, 0 ) );
	//wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
	//grassSkin.getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
	//grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
	//grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
	//grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
	//grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
	//wo->setLabel( "Grass" );
	//worldLst->push_back( wo );

	////Create the infinite grass plane that uses the Open Dynamics Engine (ODE)
	//wo = WOStatic::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
	//((WOStatic*)wo)->setODEPrimType( ODE_PRIM_TYPE::PLANE );
	//wo->setPosition( Vector(0,0,0) );
	//wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );
	//wo->setLabel( "Grass" );
	//worldLst->push_back( wo );

	////Create the infinite grass plane that uses NVIDIAPhysX(the floor)
	//wo = Module6WOP::New(p, scene, grass, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT, true, PxVec3{ 0, 0, 0 });
	//wo->setPosition(Vector(0, 0, 0));
	//wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats(5.0f);
	//wo->setLabel("Grass");
	//worldLst->push_back(wo);

	createFloor();

	////Create the infinite grass plane (the floor)
	//wo = WONVPhysX::New( shinyRedPlasticCube, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
	//wo->setPosition( Vector(0,0,50.0f) );
	//wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//wo->setLabel( "Grass" );
	//worldLst->push_back( wo );

	//wo = WONVPhysX::New( shinyRedPlasticCube, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
	//wo->setPosition( Vector(0,0.5f,75.0f) );
	//wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//wo->setLabel( "Grass" );
	//worldLst->push_back( wo );

	//wo = WONVDynSphere::New( ManagerEnvironmentConfiguration::getVariableValue("sharedmultimediapath") + "/models/sphereRp5.wrl", Vector(1.0f, 1.0f, 1.0f), mstSMOOTH );
	//wo->setPosition( 0,0,100.0f );
	//wo->setLabel( "Sphere" );
	//this->worldLst->push_back( wo );

	//wo = WOHumanCal3DPaladin::New( Vector( .5, 1, 1 ), 100 );
	//((WOHumanCal3DPaladin*)wo)->rayIsDrawn = false; //hide the "leg ray"
	//((WOHumanCal3DPaladin*)wo)->isVisible = false; //hide the Bounding Shell
	//wo->setPosition( Vector(20,20,20) );
	//wo->setLabel( "Paladin" );
	//worldLst->push_back( wo );
	//actorLst->push_back( wo );
	//netLst->push_back( wo );
	//this->setActor( wo );
	//
	//wo = WOHumanCyborg::New( Vector( .5, 1.25, 1 ), 100 );
	//wo->setPosition( Vector(20,10,20) );
	//wo->isVisible = false; //hide the WOHuman's bounding box
	//((WOHuman*)wo)->rayIsDrawn = false; //show the 'leg' ray
	//wo->setLabel( "Human Cyborg" );
	//worldLst->push_back( wo );
	//actorLst->push_back( wo ); //Push the WOHuman as an actor
	//netLst->push_back( wo );
	//this->setActor( wo ); //Start module where human is the actor

	////Create and insert the WOWheeledVehicle
	//std::vector< std::string > wheels;
	//std::string wheelStr( "../../../shared/mm/models/WOCar1970sBeaterTire.wrl" );
	//wheels.push_back( wheelStr );
	//wheels.push_back( wheelStr );
	//wheels.push_back( wheelStr );
	//wheels.push_back( wheelStr );
	//wo = WOCar1970sBeater::New( "../../../shared/mm/models/WOCar1970sBeater.wrl", wheels );
	//wo->setPosition( Vector( 5, -15, 20 ) );
	//wo->setLabel( "Car 1970s Beater" );
	//((WOODE*)wo)->mass = 200;
	//worldLst->push_back( wo );
	//actorLst->push_back( wo );
	//this->setActor( wo );
	//netLst->push_back( wo );

	createModule_7WayPoints();
}


void GLViewModule_7::createModule_7WayPoints()
{
	// Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
	WayPointParametersBase params(this);
	params.frequency = 5000;
	params.useCamera = true;
	params.visible = true;
	WOWayPointSpherical* wayPt = WOWP1::New(params, 3);
	wayPt->setPosition(Vector(50, 0, 3));
	worldLst->push_back(wayPt);
}
