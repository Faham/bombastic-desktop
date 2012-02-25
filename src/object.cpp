/*
    Copyright (c) 2011 <copyright holder> <email>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT1
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

//==============================================================================

#include <../include/object.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>

//==============================================================================

namespace BombasticDesktop {

//==============================================================================

unsigned int Object::ms_count = 0;
unsigned int Object::ms_incrementor = 0;
const Ogre::Vector3 Object::msc_scale(1.7f, 0.05f, 1.7f);
const Ogre::Real Object::msc_mass = 10.0f;

const char * Object::msc_object_type_str [] = {
	"Unknown"
	, "Folder"
	, "Image"
	, "Video"
	, "Audio"
	, "Document"
	, "Text"
	, "PDF"
	, "MyComputer"
	, "RecycleBin"
};

//==============================================================================

Object::Object(Ogre::SceneManager * scene_mgr, OgreBulletDynamics::DynamicsWorld * dyn_world, const ObjectType & obj_type)
	: m_mass (msc_mass)
	, m_restitution (0.0f)
	, m_friction (3.0f)
	, mp_entity (NULL)
	, mp_node (NULL)
	, mp_body (NULL)
	, mp_world (NULL)
	, m_selected (false)
	, mp_pile (NULL)
	, m_freeze (false)
{
	mp_world = dyn_world;
	m_name = "object" + Ogre::StringConverter::toString(ms_incrementor);
	m_type = obj_type;
	
	mp_entity = scene_mgr->createEntity(m_name, "cube.mesh");
	
	mp_entity->setQueryFlags(OQF_DefaultSelectable);
	mp_entity->setUserAny(Ogre::Any(this));
	mp_entity->setCastShadows(true);

	highlightVisual(false);

	mp_node = scene_mgr->getRootSceneNode()->createChildSceneNode();
	mp_node->attachObject(mp_entity);
	mp_node->scale(msc_scale);
	mp_node->setPosition(
		mp_node->getPosition().x
		, mp_node->getScale().y * 100 / 2
		, mp_node->getPosition().z
	);
	
	resetPhysics();
	
	++ms_count;
	++ms_incrementor;
}

//------------------------------------------------------------------------------

Object::~Object()
{
	destroyPhysics();
	mp_node->detachAllObjects();
	--ms_count;
}

//------------------------------------------------------------------------------

void Object::destroyPhysics ()
{
	if (NULL != mp_body)
	{
		OgreBulletCollisions::BoxCollisionShape *shape = dynamic_cast<OgreBulletCollisions::BoxCollisionShape *>(mp_body->getShape());
		delete mp_body;
		delete shape;
		mp_body = NULL;
	}
}

//------------------------------------------------------------------------------

void Object::move(const Ogre::Vector3 & vec)
{
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(vec));
	mp_body->getBulletRigidBody()->setWorldTransform(transform);
}

//------------------------------------------------------------------------------

void Object::resetPhysics()
{
	destroyPhysics ();	

	mp_node->setPosition(
		mp_node->getPosition().x
		, mp_node->getScale().y * 100 / 2
		, mp_node->getPosition().z
	);
	
	Ogre::Vector3 size = mp_node->getScale() * 100 * 0.5f * 1.0f;
	OgreBulletCollisions::BoxCollisionShape * shape = new OgreBulletCollisions::BoxCollisionShape(size);

	m_mass = msc_mass 
		* (mp_node->getScale().x * mp_node->getScale().y * mp_node->getScale().z)
		/ (msc_scale.x * msc_scale.y * msc_scale.z)
	;
	
	mp_body = new OgreBulletDynamics::RigidBody(m_name, mp_world);
	mp_body->setShape(mp_node, shape, m_restitution, m_friction, m_mass, mp_node->_getDerivedPosition(), Ogre::Quaternion::IDENTITY);
	mp_body->setLinearVelocity(Ogre::Vector3::UNIT_Y * 20.0f);
	mp_body->setCastShadows(true);
	mp_body->disableDeactivation();
	mp_body->setDamping(0.2f, 0.5f);
}

//------------------------------------------------------------------------------

void Object::setScale(const Ogre::Vector3 & scl)
{
	mp_node->setScale(scl);
	resetPhysics();
}

//------------------------------------------------------------------------------

void Object::highlightVisual (bool b)
{
	if (b)
		mp_entity->setMaterialName("BombasticDesktop/Icon/" + std::string(objectTypeToStr(m_type)) + "/Highlight");
	else
		mp_entity->setMaterialName("BombasticDesktop/Icon/" + std::string(objectTypeToStr(m_type)));
}

//------------------------------------------------------------------------------

void Object::freeze (bool b)
{
	m_freeze = b;
	if (b)
	{
		mp_entity->setQueryFlags(OQF_NotSelectable);
		//mp_body->getBulletRigidBody()->setMassProps(m_mass, btVector3(0.0f, 1.0f, 0.0f));
		mp_entity->setMaterialName("BombasticDesktop/Icon/Freeze");
	}
	else
	{
		mp_entity->setQueryFlags(OQF_DefaultSelectable);
		//mp_body->getBulletRigidBody()->setMassProps(m_mass, btVector3(0.0f, -1.0f, 0.0f));
		mp_entity->setMaterialName("BombasticDesktop/Icon/" + std::string(objectTypeToStr(m_type)));
	}
}

//------------------------------------------------------------------------------

const char * Object::objectTypeToStr (const ObjectType & obj_t)
{
	if (obj_t >= OT_Count || obj_t < OT_Unknown)
		return msc_object_type_str[OT_Unknown];
	else 
		return msc_object_type_str[obj_t];
}

//==============================================================================

} // namespace BombasticDesktop

//==============================================================================
