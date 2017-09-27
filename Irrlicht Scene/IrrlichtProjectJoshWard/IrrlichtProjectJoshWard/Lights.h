#pragma once
#include <irrlicht.h>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class Light
{
public:
	// sets up two constructors, a default constructor and a constructor used to load the lights
	Light();
	Light(const char *light_pathname, const char *texture_pathname, core::vector3df position, core::vector3df rotation,
		core::vector3df scale, scene::ISceneManager *sceneManager, video::IVideoDriver *driver, bool lighting);
	
	// sets up a function to create collision with the model
	void CreateCollision(scene::ICameraSceneNode *camera, scene::ISceneManager *sceneManager,
		core::vector3df radius, core::vector3df gravity, core::vector3df translation);

private:
	// sets a private mesh variable for use in the constructor
	scene::IAnimatedMeshSceneNode *model;
	scene::ITriangleSelector *lightTriangleSelector;
	scene::ISceneNodeAnimator *lightAnim;
};

Light::Light()
{
}

// takes many passed in parameters to simplify the creation of lighting
Light::Light(const char *light_pathname, const char *texture_pathname, core::vector3df position, core::vector3df rotation,
	core::vector3df scale, scene::ISceneManager *sceneManager, video::IVideoDriver *driver, bool lighting)
{
	// Load the lamp model when passed in
	scene::IAnimatedMesh *mesh = sceneManager->getMesh(light_pathname);

	// set the models mesh and if it is to have lighting or not
	model = sceneManager->addAnimatedMeshSceneNode(mesh);
	model->setMaterialFlag(video::EMF_LIGHTING, lighting);

	// if no texture is supplied, do nothing, otherwise add a texture
	if (texture_pathname == nullptr)
	{

	}
	else
	{
		model->setMaterialTexture(0, driver->getTexture(texture_pathname));
	}

	// set the position, rotation and scale of the lamp
	model->setPosition(position);
	model->setRotation(rotation);
	model->setScale(scale);

	// sets the position of the light to be equal to the models position
	core::vector3df lightPos = position;
	// then set the lights y position to be above the model
	lightPos.Y = 1700.0f;

	// create a light above the lamp, with a while colour and no parent
	scene::ILightSceneNode *light = sceneManager->addLightSceneNode(0, lightPos,
		video::SColor(255.0f, 255.0f, 255.0f, 255.0f), 3000);
}

// sets up a function to create collision with the camera and the model
void Light::CreateCollision(scene::ICameraSceneNode *camera, scene::ISceneManager *sceneManager,
	core::vector3df radius, core::vector3df gravity, core::vector3df translation)
{
	// initialises the model triangle selector
	lightTriangleSelector = 0;

	// creates a triangle selector from the models mesh
	lightTriangleSelector = sceneManager->createTriangleSelector(model->getMesh(), model);

	// adds the triangle selector to the model scene node
	model->setTriangleSelector(lightTriangleSelector);

	// creates an animator to manage collision between the player and the model
	lightAnim = sceneManager->createCollisionResponseAnimator(lightTriangleSelector, camera, radius, gravity, translation);

	// drop the selector as it is no longer used
	lightTriangleSelector->drop();

	// add the animator to the camera to enable the collision
	camera->addAnimator(lightAnim);

	// drop the animator as it is no longer used
	lightAnim->drop();
}