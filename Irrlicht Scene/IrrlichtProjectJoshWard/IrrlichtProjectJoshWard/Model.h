#pragma once
#include <irrlicht.h>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class Model
{
public:
	// sets up two constructors, a default contructor and a constructor used to load models
	Model();

	Model(const char *model_pathname, const char *texture_pathname, core::vector3df position, core::vector3df rotation, 
		core::vector3df scale, scene::ISceneManager *sceneManager, video::IVideoDriver *driver, bool lighting);

	// sets up a function to create collision with the model
	void CreateCollision(scene::ICameraSceneNode *camera, scene::ISceneManager *sceneManager,
		core::vector3df radius, core::vector3df gravity, core::vector3df translation);

	// This sets up encapsulated functions, so that you can only access specific node related functions
	void setRotation(core::vector3df rotation);

	void setParent(scene::ISceneNode *newParent);

	void setFrameLoop(s32 begin, s32 end);

	void setFlyAnimation(scene::ISceneNodeAnimator *animation);

private:
	// sets up private variables for the model, triangle selector, and animator, used in the constructor
	scene::IAnimatedMeshSceneNode *model;
	scene::ITriangleSelector *modelTriangleSelector;
	scene::ISceneNodeAnimator *modelAnim;
};

Model::Model()
{

}

// takes many passed in parameters to simplify the creation of models
Model::Model(const char *model_pathname, const char *texture_pathname, core::vector3df position, core::vector3df rotation,
	core::vector3df scale, scene::ISceneManager *sceneManager, video::IVideoDriver *driver, bool lighting)
{

	// Load the mesh from the file
	scene::IAnimatedMesh *mesh = sceneManager->getMesh(model_pathname);

	// add the mesh to the model variable
	model = sceneManager->addAnimatedMeshSceneNode(mesh);

	// set the lighting to be enabled or disables via passed in value
	model->setMaterialFlag(video::EMF_LIGHTING, lighting);

	// if there is no texture do nothing otherwise set the texture of the model
	if (texture_pathname == nullptr || texture_pathname == "none")
	{
		
	}
	else {
		model->setMaterialTexture(0, driver->getTexture(texture_pathname));
	}
	// set the position, rotation and scale of the model 
	model->setPosition(position);
	model->setRotation(rotation);
	model->setScale(scale);

	// normalise the models normals so that lighting works properly when scaled
	model->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);

	// add a shadow when realistic shadows is chosen by the user
	model->addShadowVolumeSceneNode();
	
}

// sets up a function to create collision with the camera and the model
void Model::CreateCollision(scene::ICameraSceneNode *camera, scene::ISceneManager *sceneManager,
	core::vector3df radius, core::vector3df gravity, core::vector3df translation)
{
	// initialises the model triangle selector
	modelTriangleSelector = 0;

	// creates a triangle selector from the models mesh
	modelTriangleSelector = sceneManager->createTriangleSelector(model->getMesh(), model);

	// adds the triangle selector to the model scene node
	model->setTriangleSelector(modelTriangleSelector);

	// creates an animator to manage collision between the player and the model
	modelAnim = sceneManager->createCollisionResponseAnimator(modelTriangleSelector, camera, radius, gravity, translation);

	// drop the selector as it is no longer used
	modelTriangleSelector->drop();

	// add the animator to the camera to enable the collision
	camera->addAnimator(modelAnim);

	// drop the animator as it is no longer used
	modelAnim->drop();
}


// Allows the rotation of model to be set without referencing the node in the main
void Model::setRotation(core::vector3df rotation)
{
	model->setRotation(rotation);
}

// Allows the parent of model to be set without referencing the node in the main
void Model::setParent(scene::ISceneNode *newParent)
{
	model->setParent(newParent);
}

// Allows the frame loop of model to be set without referencing the node in the main
void Model::setFrameLoop(s32 begin, s32 end)
{
	model->setFrameLoop(begin, end);
}

// Allows the Animation of model to be set without referencing the node in the main
void Model::setFlyAnimation(scene::ISceneNodeAnimator *animation)
{
	model->addAnimator(animation);
	animation->drop();
}
