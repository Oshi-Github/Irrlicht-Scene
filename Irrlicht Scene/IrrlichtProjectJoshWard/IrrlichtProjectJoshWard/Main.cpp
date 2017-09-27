#include <irrlicht.h>
#include <vector>
#include <irrKlang.h>
#include "driverChoice.h"
#include "Model.h"
#include "Lights.h"
#include "Billboard.h"
#include "EventManager.h"
// Includes the libraries needed to complete the project, such as Engine, Sound Engine and Data Types

using namespace irr;
using namespace gui;
using namespace irrklang;
// Using the libraries namespaces allows use of the functions without referencing the library 


// This allows the irrlicht library to be linked when on Windows
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

// This allows the irrklang library to be linked
#pragma comment(lib , "irrklang.lib")

int main()
{
	// This prompts the user to input whether they would like realtime shadows or not
	// The result is stored and used to either turn realtime lighting on or off in the Irrlicht Device
	char i;
	printf("Press 'y' if you want to use realtime shadows.\n");
	std::cin >> i;
	const bool shadows = (i == 'y');

	// This promts the user to input whether they would like dynamic lighting or not
	// The result is stored and used to either turn dynamic lighting on or off for all of the objects in the scene
	// As well as turning on lighting from the lamps throughout the scene
	char j;
	printf("Press 'y' if you want to use dynamic lighting.\n");
	std::cin >> j;
	const bool dynamicLighting = (j == 'y');

	// These are two bools used to calculate if the user has already provided input
	// They are used for calculation while the Device is running
	bool cameraSwitch = false;
	bool firedGun = false;

	// Gives the user a set of display drivers they can choose from to launch into the scene with
	// The result is stored and used when creating the Device
	video::E_DRIVER_TYPE driverType = driverChoiceConsole();

	// If the display driver cannot be created for any reason, then the program will exit
	if (driverType == video::EDT_COUNT)
	{
		return 1;
	}

	// This creates an instance of the EventManager class, used for Calculating whether there is input
	// Such as key presses or mouse presses
	MyEventReceiver eventManager;

	// This creates an Irrlicht Device, using the display driver chosen, a window size of 800 by 600
	// 16 bits per pixel, no fullscreen, realtime shadows as chosen, no vsync and a reference to the instance
	// of the event manager created
	IrrlichtDevice *device = createDevice(driverType, core::dimension2d<u32>(800, 600),
		16, false, shadows, false, &eventManager);

	// If the Irrlicht Device cannot be made for any reason, then the program will exit
	if (device == 0)
	{
		return 1;
	}

	// This stores references to the Video Driver, Scene Manager and GUI Environment
	// All used within the Irrlicht Engine, allowing them to be accessed via the pointer
	// Elsewhere in the solution
	video::IVideoDriver	*videoDriver = device->getVideoDriver();
	scene::ISceneManager *sceneManager = device->getSceneManager();
	gui::IGUIEnvironment *guiEnvironment = device->getGUIEnvironment();

	// This forces the textures to be always 16 bit reduce the amount of memory required
	// Therefore increases exection speed
	videoDriver->setTextureCreationFlag(video::ETCF_ALWAYS_16_BIT, true);

	// This stores a reference to another font, allowing use of the font via the pointer
	// Elsewhere in the solution
	IGUIFont *font = guiEnvironment->getFont("media/fontlucida.png");

	// This creates a Irrlicht SkeyMap array, that can be added as a parameter to the FPSCamera
	// It allows remapping of the movement keys without using the EventManager instance and also
	// Allows easy remapping of functions such as crouch and jump
	SKeyMap keyMapNew[6];
	keyMapNew[0].Action = EKA_MOVE_FORWARD;
	keyMapNew[0].KeyCode = KEY_KEY_W;
	keyMapNew[1].Action = EKA_MOVE_BACKWARD;
	keyMapNew[1].KeyCode = KEY_KEY_S;
	keyMapNew[2].Action = EKA_STRAFE_LEFT;
	keyMapNew[2].KeyCode = KEY_KEY_A;
	keyMapNew[3].Action = EKA_STRAFE_RIGHT;
	keyMapNew[3].KeyCode = KEY_KEY_D;
	keyMapNew[4].Action = EKA_CROUCH;
	keyMapNew[4].KeyCode = KEY_LCONTROL;
	keyMapNew[5].Action = EKA_JUMP_UP;
	keyMapNew[5].KeyCode = KEY_SPACE;


	// This creates a pointer to a FPS Camera, with 1.2f Move speed and allowance of vertical movement and jumping
	// This camera can then be referenced later in the solution using this pointer
	scene::ICameraSceneNode *camera = sceneManager->addCameraSceneNodeFPS(0, 100.0f, 1.2f, -1, keyMapNew, 6, false, 1.0f, false, true);

	// This creates a pointer to a Irrlicht SceneCollisionManager, which will allow collision between the camera and the terrain
	// This pointer will allow the collision manager to be referenced later in the solution
	scene::ISceneCollisionManager *collisionManager = sceneManager->getSceneCollisionManager();

	// This sets the position of the referenced FPS Camera when it is first created in the scene
	camera->setPosition(core::vector3df(10000, 900, 10000));

	// This sets the far value of the referenced FPS Camera when it is first created in the scene
	// This is how far the camera will be able to render, often saving FPS if you lower the value
	camera->setFarValue(10000.0f);

	// This uses the reference to the Irrlicht Device to disable visibility of the Mouse Cursor
	// Otherwise the mouse cursor would always be visible in the center of the viewport
	device->getCursorControl()->setVisible(false);

	// This uses the reference to the Scene Manager to set the global shadow colour in the scene
	// This is for setting the shadow colour to a darker black where there is less light
	sceneManager->setShadowColor(video::SColor(150, 0, 0, 0));

	// This adds a pointer to a Terrain Scene Node in the scene, it takes a heightmap to raise
	// or lower different parts of the terrain. It also takes other parameters, most important being
	// LOD, Patch Size and smooth factor, all of which can affect performance, Lowering Level of Detail
	// as well as the path size allowed increased FPS in the scene
	scene::ITerrainSceneNode *terrain = sceneManager->addTerrainSceneNode("media/terrain-heightmap.bmp",
		0, // parent node
		-1, // node id
		core::vector3df(0.0f, 0.0f, 0.0f), // position
		core::vector3df(0.0f, 0.0f, 0.0f), // rotation
		core::vector3df(160.0f, 20.0f, 160.0f), // scale
		video::SColor(255, 255, 255, 255), // vertex colour
		1, // max LOD
		scene::ETPS_9, // patch size
		4 // smooth factor
		);

	// This allows the terrain to be either full lit or not
	// This is used to allow the lights to light the scene
	terrain->setMaterialFlag(video::EMF_LIGHTING, dynamicLighting);

	// This sets the Texture and Detail Map for the terrain, providing more detail 
	// or an illusion of depth, while not costing as much performance
	terrain->setMaterialTexture(0, videoDriver->getTexture("media/terrain-texture.jpg"));
	terrain->setMaterialTexture(1, videoDriver->getTexture("media/detailmap3.jpg"));

	terrain->setMaterialType(video::EMT_DETAIL_MAP);

	// This scales the terrain up so that it is large enough for the fps camera to move on
	terrain->scaleTexture(1.0f, 20.0f);

	// This creates a triangle selector for the terrain, to be used for collision between player and terrain
	scene::ITriangleSelector *terrainTriangleSelector = sceneManager->createTerrainTriangleSelector(terrain, 0);

	// This sets the terrain triangle selector to the triangle selector made for the terrain
	terrain->setTriangleSelector(terrainTriangleSelector);



	// This adds collision between the terrain and the player, via use of an animator, also enables gravity
	scene::ISceneNodeAnimator *collisionResponseAnimator = sceneManager->createCollisionResponseAnimator(terrainTriangleSelector,
		camera, core::vector3df(30, 120, 30), core::vector3df(0, -2.0f, 0), core::vector3df(0, 100, 0));

	// This drops the terrain triangle selector as it is no longer in use
	terrainTriangleSelector->drop();

	// This adds the collision animator made above to the camera
	camera->addAnimator(collisionResponseAnimator);

	// This drops the collision animator as it is no longer in use
	collisionResponseAnimator->drop();

	// This creates a skybox using six textures got from the video driver
	videoDriver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

	scene::ISceneNode *node = sceneManager->addSkyBoxSceneNode(
		videoDriver->getTexture("media/poc_up.jpg"),
		videoDriver->getTexture("media/poc_dn.jpg"), 
		videoDriver->getTexture("media/poc_rt.jpg"), 
		videoDriver->getTexture("media/poc_lf.jpg"),
		videoDriver->getTexture("media/poc_ft.jpg"),
		videoDriver->getTexture("media/poc_bk.jpg"));

	videoDriver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	// This creates a vector of Building objects, made by the Model class
	// This is done to made creating collisions and cycling through objects more efficient
	// as well as being able to add or remove Buildings without changing size of data types
	std::vector<Model> Buildings;

		// This supplies file, texture, position, rotation, scale, scene manager, video driver and whether lighting is on to each Building

		Buildings.push_back(Model("Models/Obj_File/Church.obj", "Models/Texture_File/churcht.jpg", core::vector3df(17500, 450, 14800),
		core::vector3df(0, 0, 0), core::vector3df(20.0f), sceneManager, videoDriver, dynamicLighting));

		Buildings.push_back(Model("Models/Obj_File/Building1.obj", "Models/Texture_File/Building1t.jpg", core::vector3df(14000.0f, 450.0f, 20000.0f),
		core::vector3df(0, 0, 0), core::vector3df(35.0f, 20.0f, 35.0f), sceneManager, videoDriver, dynamicLighting));

		Buildings.push_back(Model("Models/Obj_File/Shop.obj", "Models/Texture_File/shopt.jpg", core::vector3df(14000.0f, 900, 27000.0f),
		core::vector3df(0, 0, 90), core::vector3df(25.0f), sceneManager, videoDriver, dynamicLighting));

		Buildings.push_back(Model("Models/Obj_File/Warehouse.obj", "Models/Texture_File/warehouset.jpg", core::vector3df(17500, 450, 27000.0f),
		core::vector3df(0, 0, 0), core::vector3df(35.0f, 40.0f, 35.0f), sceneManager, videoDriver, dynamicLighting));

		Buildings.push_back(Model("Models/Obj_File/Flats.obj", "Models/Texture_File/flatst.jpg", core::vector3df(18500, 450, 20000),
		core::vector3df(0, 0, 0), core::vector3df(30.0f, 30.0f, 30.0f), sceneManager, videoDriver, dynamicLighting));

		Buildings.push_back(Model("Models/3d-platformer-houses.obj", "none" , core::vector3df(30000, 450, 17000),
		core::vector3df(0, 0, 0), core::vector3df(140), sceneManager, videoDriver, dynamicLighting));

		// This loops through the vector applying collision between the player and the Building
	for (u8 i = 0; i < 6; i++)
	{
		Buildings[i].CreateCollision(camera, sceneManager, core::vector3df(30, 60, 30), core::vector3df(0, 0, 0), core::vector3df(0, 0, 0));
	}

	// This rotates one of the buildings so that it is facing the correct way in the scene
	Buildings[5].setRotation(core::vector3df(0, -90, 0));
	

	// This creates a vector of static objects, made by the Model class
	// This is done to made creating collisions and cycling through objects more efficient
	// as well as being able to add or remove Static Objects without changing size of data types
	std::vector<Model> StaticObjects;

		// This supplies file, texture, position, rotation, scale, scene manager, video driver and whether lighting is on to the Static Objects
		StaticObjects.push_back(Model("Models/Obj_File/car-coupe-blue.obj", "none" , core::vector3df(13000, 450, 14000),
		core::vector3df(0, 0, 0), core::vector3df(500), sceneManager, videoDriver, dynamicLighting));

		StaticObjects.push_back(Model("Models/Obj_File/Dumpster.obj", "Models/Texture_File/Dumpster.jpg", core::vector3df(18310, 450, 18250),
		core::vector3df(0, 0, 0), core::vector3df(5), sceneManager, videoDriver, dynamicLighting));

		StaticObjects.push_back(Model("Models/Obj_File/shroom1.obj", "Models/Texture_File/shroom_color.png", core::vector3df(25400, 450, 15970),
			core::vector3df(0, 0, 0), core::vector3df(500), sceneManager, videoDriver, dynamicLighting));

		StaticObjects.push_back(Model("Models/Obj_File/shroom2.obj", "Models/Texture_File/shroom_color2.png", core::vector3df(25400, 450, 16200),
			core::vector3df(0, 0, 0), core::vector3df(500), sceneManager, videoDriver, dynamicLighting));

		StaticObjects.push_back(Model("Models/Obj_File/shroom3.obj", "Models/Texture_File/shroom_color.png", core::vector3df(25500, 450, 16100),
			core::vector3df(0, 0, 0), core::vector3df(500), sceneManager, videoDriver, dynamicLighting));

		StaticObjects.push_back(Model("Models/Obj_File/Tent_01.obj", "none" , core::vector3df(27741, 450, 27000),
			core::vector3df(0, 0, 0), core::vector3df(300), sceneManager, videoDriver, dynamicLighting));

		StaticObjects.push_back(Model("Models/Obj_File/Campfire_01.obj", "none" , core::vector3df(27500, 450, 25500),
			core::vector3df(0, 0, 0), core::vector3df(500), sceneManager, videoDriver, dynamicLighting));

		StaticObjects.push_back(Model("Models/Obj_File/shoppingcart_0.obj", "Models/Texture_File/Dumpster.jpg", core::vector3df(20320, 830, 19500),
			core::vector3df(0, 0, 0), core::vector3df(100), sceneManager, videoDriver, dynamicLighting));

		StaticObjects.push_back(Model("Models/Obj_File/wall_concrete_1.obj", "Models/references/SeamlessMixedStone0009.jpg", core::vector3df(28850, 450, 17225),
			core::vector3df(0, 0, 0), core::vector3df(17.3f, 1.5f, 1.5f), sceneManager, videoDriver, dynamicLighting));

		StaticObjects.push_back(Model("Models/Obj_File/wall_concrete_1.obj", "Models/references/SeamlessMixedStone0009.jpg", core::vector3df(28850, 450, 12775),
			core::vector3df(0, 0, 0), core::vector3df(17.3f, 1.5f, 1.5f), sceneManager, videoDriver, dynamicLighting));

		StaticObjects.push_back(Model("Models/Obj_File/wall_concrete_1.obj", "Models/references/SeamlessMixedStone0009.jpg", core::vector3df(28810, 450, 12781),
			core::vector3df(0, 0, 0), core::vector3df(17.3f, 1.5f, 1.5f), sceneManager, videoDriver, dynamicLighting));

		StaticObjects.push_back(Model("Models/Obj_File/wall_concrete_1.obj", "Models/references/SeamlessMixedStone0009.jpg", core::vector3df(24200, 500, 12781),
			core::vector3df(0, 0, 0), core::vector3df(17.3f, 1.5f, 1.5f), sceneManager, videoDriver, dynamicLighting));

		// This sets the rotation of two objects so that they are facing the correct way in the scene
	StaticObjects[10].setRotation(core::vector3df(0, 90, 0));
	StaticObjects[11].setRotation(core::vector3df(90, 90, 0));

	// This loops through the vector applying collision between the player and the Static Objects
	for (u8 i = 0; i < 12; i++)
	{
		StaticObjects[i].CreateCollision(camera, sceneManager, core::vector3df(30, 60, 30), core::vector3df(0, 0, 0), core::vector3df(0, 0, 0));
	}

	// This creates a vector of Animated objects, made by the Model class
	// This is done to made creating collisions and cycling through objects more efficient
	// as well as being able to add or remove Animated Objects without changing size of data types
	std::vector<Model> AnimatedObjects;

	// This supplies file, texture, position, rotation, scale, scene manager, video driver and whether lighting is on to the Animated Objects
		AnimatedObjects.push_back(Model("Models/Md2_File/Swing.md2", "Models/Texture_File/Swing.png", core::vector3df(16000, 450, 19000),
		core::vector3df(0, 90, 0), core::vector3df(13.0f), sceneManager, videoDriver, dynamicLighting));

		AnimatedObjects.push_back(Model("Models/Md2_File/gun.md2", "Models/Texture_File/gun.jpg", core::vector3df(0, 0, 0),
		core::vector3df(0, -90, 0), core::vector3df(1), sceneManager, videoDriver, false));

		AnimatedObjects.push_back(Model("Models/Bird.ms3d", "Models/Texture_File/Bird.png", core::vector3df(23000, 1000, 13000),
		core::vector3df(0, 90, 0), core::vector3df(300.0f), sceneManager, videoDriver, dynamicLighting));

		AnimatedObjects.push_back(Model("Models/cerberus.ms3d", "Models/Texture_File/cerberus_texture.png", core::vector3df(23000, 650, 15000),
		core::vector3df(0, 90, 0), core::vector3df(30.0f), sceneManager, videoDriver, dynamicLighting));

		AnimatedObjects.push_back(Model("Models/diablous.blend.ms3d", "Models/Texture_File/diablous_texture.png", core::vector3df(19659, 600, 16584),
		core::vector3df(0, 90, 0), core::vector3df(30.0f), sceneManager, videoDriver, dynamicLighting));

		AnimatedObjects.push_back(Model("Models/mage.ms3d", "Models/Texture_File/Mage.png", core::vector3df(19536, 650, 17969),
		core::vector3df(0, 90, 0), core::vector3df(30.0f), sceneManager, videoDriver, dynamicLighting));

		AnimatedObjects.push_back(Model("Models/Spider.ms3d", "Models/Texture_File/Spider.png", core::vector3df(23000, 450, 21000),
		core::vector3df(0, 90, 0), core::vector3df(300.0f), sceneManager, videoDriver, dynamicLighting));

		AnimatedObjects.push_back(Model("media/sydney.md2", "media/sydney.bmp", core::vector3df(15608, 655, 21467),
		core::vector3df(0, 90, 0), core::vector3df(6.0f), sceneManager, videoDriver, dynamicLighting));

		AnimatedObjects.push_back(Model("media/yodan.mdl", "none", core::vector3df(22000, 655, 18000),
		core::vector3df(0, 90, 0), core::vector3df(6.0f), sceneManager, videoDriver, dynamicLighting));

		AnimatedObjects.push_back(Model("media/faerie.md2", "media/faerie2.bmp", core::vector3df(28106, 655, 20397),
		core::vector3df(0, 90, 0), core::vector3df(6.0f), sceneManager, videoDriver, dynamicLighting));

	// This sets the Parent of the gun object as the camera, so that it will follow the player around the scene
	AnimatedObjects[1].setParent(camera);

	// This sets the frame loops for the Animated Objects, so that they are all playing reasonable animations and not there full animation
	AnimatedObjects[1].setFrameLoop(13, 30);
	AnimatedObjects[3].setFrameLoop(0, 27);
	AnimatedObjects[4].setFrameLoop(0, 43);
	// This sets the rotation of some of the Animated Objects, so that they are facing the correct way in the scene
	AnimatedObjects[4].setRotation(core::vector3df(0, 90, 0));
	AnimatedObjects[5].setRotation(core::vector3df(0, -90, 0));
	AnimatedObjects[5].setFrameLoop(0, 6);
	AnimatedObjects[2].setFrameLoop(0, 27);
	AnimatedObjects[6].setFrameLoop(0, 17);
	AnimatedObjects[7].setFrameLoop(0, 90);
	AnimatedObjects[8].setFrameLoop(0, 30);
	AnimatedObjects[9].setFrameLoop(0, 30);

	// This creates a pointer to a Fly Straight Animator, which two positions set, and it will cause the object to go between both positions
	scene::ISceneNodeAnimator *flyStraightAnim = sceneManager->createFlyStraightAnimator(core::vector3df(23000, 650, 15000), core::vector3df(23000, 650, 20500),
		16000, true, true);

	// This applies the Fly Straight Animator to the cerberus, so that it will walk back and forth along the scene
	AnimatedObjects[3].setFlyAnimation(flyStraightAnim);

	// This loops through all of the Animated Objects and applies collision between them and the player
	for (u8 i = 0; i < 9; i++)
	{
		AnimatedObjects[i].CreateCollision(camera, sceneManager, core::vector3df(30, 60, 30), core::vector3df(0, 0, 0), core::vector3df(0, 0, 0));
	}
	
	
	// This creates a vector of Light objects, made by the Lights class
	// This is done to made creating collisions and cycling through objects more efficient
	// as well as being able to add or remove Light Objects without changing size of data types
	std::vector<Light> Lights;
	
		// supplies file, texture, position, rotation, scale, scene manager, video driver and whether lighting is on
		Lights.push_back(Light("Models/Obj_File/Lamp.obj", "Models/Texture_File/Dumpster.jpg", core::vector3df(14000, 450, 14000),
		core::vector3df(0, 0, 0), core::vector3df(150), sceneManager, videoDriver, dynamicLighting));

		Lights.push_back(Light("Models/Obj_File/Lamp.obj", "Models/Texture_File/Dumpster.jpg", core::vector3df(23868, 450, 20870),
		core::vector3df(0, 0, 0), core::vector3df(150), sceneManager, videoDriver, dynamicLighting));

		Lights.push_back(Light("Models/Obj_File/Lamp.obj", "Models/Texture_File/Dumpster.jpg", core::vector3df(21000, 450, 16000),
		core::vector3df(0, 0, 0), core::vector3df(150), sceneManager, videoDriver, dynamicLighting));

		Lights.push_back(Light("Models/Obj_File/Lamp.obj", "Models/Texture_File/Dumpster.jpg", core::vector3df(14000, 450, 25000),
		core::vector3df(0, 0, 0), core::vector3df(150), sceneManager, videoDriver, dynamicLighting));

		Lights.push_back(Light("Models/Obj_File/Lamp.obj", "Models/Texture_File/Dumpster.jpg", core::vector3df(16500, 450, 20500),
		core::vector3df(0, 0, 0), core::vector3df(150), sceneManager, videoDriver, dynamicLighting));

		Lights.push_back(Light("Models/Obj_File/Lamp.obj", "Models/Texture_File/Dumpster.jpg", core::vector3df(24000, 450, 15000),
			core::vector3df(0, 0, 0), core::vector3df(150), sceneManager, videoDriver, dynamicLighting));

		Lights.push_back(Light("Models/Obj_File/Lamp.obj", "Models/Texture_File/Dumpster.jpg", core::vector3df(28715, 450, 21671),
			core::vector3df(0, 0, 0), core::vector3df(150), sceneManager, videoDriver, dynamicLighting));
			
		// This cycles through the vector to apply collision between the player and the light objects
		for (u8 i = 0; i < 7; i++)
		{
			Lights[i].CreateCollision(camera, sceneManager, core::vector3df(30, 60, 30), core::vector3df(0, 0, 0), core::vector3df(0, 0, 0));
		}


	// This creates a vector of billboards, made by the Billboard class
	// This is done to remove the need to change data type sizes when adding or removing objects to the vector
	std::vector<Billboard> Billboards;

		// supplies texture, position, scale, scene manager, video driver and whether lighting is on
		Billboards.push_back(Billboard("Models/Texture_File/Tree01.png", core::dimension2d<f32>(2000.0f, 2000.0f),
		core::vector3df(13000, 1250, 13000), sceneManager, videoDriver, dynamicLighting));

		Billboards.push_back(Billboard("Models/Texture_File/Tree02.png", core::dimension2d<f32>(2000.0f, 2000.0f),
		core::vector3df(13000, 1250, 15000), sceneManager, videoDriver, dynamicLighting));

		Billboards.push_back(Billboard("Models/Texture_File/Tree03.png", core::dimension2d<f32>(2000.0f, 2000.0f),
		core::vector3df(13000, 1250, 17000), sceneManager, videoDriver, dynamicLighting));

		Billboards.push_back(Billboard("Models/Texture_File/Tree04.png", core::dimension2d<f32>(2000.0f, 2000.0f),
		core::vector3df(13000, 1250, 19000), sceneManager, videoDriver, dynamicLighting));

		Billboards.push_back(Billboard("Models/Texture_File/Tree05.png", core::dimension2d<f32>(2000.0f, 2000.0f),
		core::vector3df(13000, 1000, 21000), sceneManager, videoDriver, dynamicLighting));

		Billboards.push_back(Billboard("Models/Texture_File/Bush1.png", core::dimension2d<f32>(800.0f, 800.0f),
		core::vector3df(24500, 700, 21000), sceneManager, videoDriver, dynamicLighting));

		Billboards.push_back(Billboard("Models/Texture_File/Bush2.png", core::dimension2d<f32>(800.0f, 800.0f),
		core::vector3df(24500, 700, 19000), sceneManager, videoDriver, dynamicLighting));

		Billboards.push_back(Billboard("Models/Texture_File/Bush3.png", core::dimension2d<f32>(800.0f, 800.0f),
		core::vector3df(24500, 700, 17000), sceneManager, videoDriver, dynamicLighting));

		Billboards.push_back(Billboard("Models/Texture_File/Bush4.png", core::dimension2d<f32>(800.0f, 800.0f),
		core::vector3df(24500, 700, 15000), sceneManager, videoDriver, dynamicLighting));

		Billboards.push_back(Billboard("Models/Texture_File/Bush5.png", core::dimension2d<f32>(800.0f, 800.0f),
		core::vector3df(24500, 700, 13000), sceneManager, videoDriver, dynamicLighting));
		
	// This adds some text to the top left of the screen using the reference to the font pointer created earlier
	scene::ITextSceneNode *text = sceneManager->addTextSceneNode(font, L"Normal Text", video::SColor(255, 255, 255, 255),
		camera, core::vector3df(-25, 25, 40));

	// This adds animated water to the game scene using Hill Planes as a mesh
	// source: http://irrlicht.sourceforge.net/docu/example008.html

	// Creates a hill plane mesh to add the water to, it has no hills but large patch sizes and low counts, to maximize performance
	// While retaining some visual quality
	scene::IAnimatedMesh *hillMesh = sceneManager->addHillPlaneMesh("Hill", core::dimension2d<f32>(500, 500), core::dimension2d<u32>(9, 9), 0,
		0, core::dimension2d<f32>(0, 0), core::dimension2d<f32>(20, 20));

	// This adds the water to the hill plane mesh and then sets the position of the water in the scene
	node = sceneManager->addWaterSurfaceSceneNode(hillMesh->getMesh(0), 3.0f, 300.0f, 30.0f);
	node->setPosition(core::vector3df(26600, 490, 15000));

	// This applies two textures to the water, an underlayer of stones and an overlayer of the water
	node->setMaterialTexture(0, videoDriver->getTexture("media/stones.jpg"));
	node->setMaterialTexture(1, videoDriver->getTexture("media/water.jpg"));

	// This sets whether the water is going to be full lit of lit by lighting, as well as adding
	// a reflection layer, adding a white layer on top that looks like light relflecting of the surface
	node->setMaterialFlag(video::EMF_LIGHTING, dynamicLighting);
	node->setMaterialType(video::EMT_REFLECTION_2_LAYER);

	// This creates a reference to a created Irrklang Device, this is the sound engine
	ISoundEngine *soundEngine = createIrrKlangDevice();

	// If the sound engine cannot be made for whatever reason, then the program will exit
	if (soundEngine == NULL)
	{
		device->drop();

		return -2;
	}

	// This creates background music using the sound engine, providing a music file and whether it will be looped
	ISound *backgroundMusic = soundEngine->play2D("Models/Music/Court_and_Page.ogg", true, false);

	// This sets the gun sound to NULL, so that it can be created when a key press is active, then reset to NULL
	ISound *gunSound = NULL;

	// This initialises the fps counter
	int lastFPS = -1;

	// This allows framerate independent actions to be started
	// This gets the time at the start of the scene
	u32 then = device->getTimer()->getTime();

	// while the game is running
	while (device->run())
	{
		// work out the frame delta time, which is the time it has been since the last frame
		const u32 now = device->getTimer()->getTime();
		const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
		then = now;

		// This plays a gunshot sound when the user presses the E key and the sound is currently not playing
		if (eventManager.IsKeyDown(KEY_KEY_E) && gunSound == NULL)
		{
				gunSound = soundEngine->play2D("Models/Music/Gunshot_and_echo_long.ogg", false, false, true);
		}

		// When the sound is finished, and if the sound if stored in the pointer, drop the sound and reset the sound pointer
		if (gunSound != NULL && gunSound->isFinished())
		{
			gunSound->drop();

			gunSound = NULL;
		}

		// If the user presses the key R, Lower the far value and shorten the FOV
		if (eventManager.IsKeyDown(KEY_KEY_R))
		{
			camera->setFarValue(3000);
			camera->setFOV(70.0f);
		}

			//begin the scene
			videoDriver->beginScene(true, true, 0);

			// draw everything in the scene 
			sceneManager->drawAll();

			 // draw everything in the GUI
			guiEnvironment->drawAll();

			// end the scene
			videoDriver->endScene();

			// set the current fps
			int fps = videoDriver->getFPS();

			// check if the fps has changed since last update, if so do next part
			if (lastFPS != fps)
			{
				// sets the window caption only once
				bool SetCaption = false;
				if (SetCaption = false)
				{
					core::stringw windowTitle = L"Josh Ward - Irrlicht Project []";

					windowTitle += videoDriver->getName();

					device->setWindowCaption(windowTitle.c_str());

					SetCaption = true;
				}

				// sets up the gui for displaying FPS and position of camera
				core::stringw guiText = "FPS: ";

				guiText += fps;
				// adds the fps to the text as well as cast the position to a shorter integer
				guiText += " X:";
				guiText += (s16)camera->getAbsolutePosition().X;
				guiText += " Y:";
				guiText += (s16)camera->getAbsolutePosition().Y;
				guiText += " Z:";
				guiText += (s16)camera->getAbsolutePosition().Z;

				// sets the text to the gui static text
				text->setText(guiText.c_str());

				// because fps has changed, update the fps counter 
				lastFPS = fps;
			}
		//}

		

	}

	// the game has stopped running, so stop the sound engine
	soundEngine->drop();

	// the game has stopped running, stop the game
	device->drop();

	// end the program
	return 0;
}