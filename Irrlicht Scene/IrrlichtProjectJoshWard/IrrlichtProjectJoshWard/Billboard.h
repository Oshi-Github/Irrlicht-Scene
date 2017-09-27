#pragma once
#include <irrlicht.h>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

class Billboard
{
public:
	// sets up two constructors, a default constructor and a contructor used to load billbiards
	Billboard();
	Billboard(const char *texture_pathname, core::dimension2d<f32>(scale), core::vector3df postion,
		scene::ISceneManager *sceneManager, video::IVideoDriver *driver, bool lighting);

private:
	// sets a private billboard variable up for use in the constructor
	scene::IBillboardSceneNode *billboard;
};

Billboard::Billboard()
{
}

// takes many passed in paramaters to simply the creation of billboards
Billboard::Billboard(const char *texture_pathname, core::dimension2d<f32>(scale), core::vector3df postion,
	scene::ISceneManager *sceneManager, video::IVideoDriver *driver, bool lighting)
{
	// adds a billboard to the scene, with no parent, and a passed in scale and position
	billboard = sceneManager->addBillboardSceneNode(0, scale, postion);

	// sets the lighting of the billboard to be users choice
	billboard->setMaterialFlag(video::EMF_LIGHTING, lighting);
	
	// sets the texture of the billboard to a passed in value
	billboard->setMaterialTexture(0, driver->getTexture(texture_pathname));

	// sets the billboard to have an alpha layer so it looks realistic
	billboard->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);

	// normalises the billboards normal so that lighting works properly when scaled
	billboard->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
}