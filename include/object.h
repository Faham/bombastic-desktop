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
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

//==============================================================================

#ifndef OBJECT_H
#define OBJECT_H

//==============================================================================

#include <Ogre.h>

//==============================================================================

namespace OgreBulletDynamics { class DynamicsWorld; class RigidBody; }

//==============================================================================

namespace BombasticDesktop {

//==============================================================================

class Pile;

enum ObjectQueryFlag {
	OQF_NotSelectable = 1 << 0
	, OQF_DefaultSelectable = 1 << 1
};

//==============================================================================

class Object : public Ogre::Any
{
public:
	enum ObjectType {
		OT_Unknown = 0
		, OT_Folder
		, OT_Image
		, OT_Video
		, OT_Audio
		, OT_Document
		, OT_Text
		, OT_PDF
		, OT_MyComputer
		, OT_RecycleBin
		, OT_Count
	};

public:
	Object(Ogre::SceneManager * scene_mgr, OgreBulletDynamics::DynamicsWorld * dyn_world, const ObjectType & obj_type = OT_Unknown);
	~Object();
	void destroyPhysics ();
	static const unsigned int & getCount () { return ms_count; }
	Ogre::Entity * getEntity () { return mp_entity; }
	Ogre::SceneNode * getNode () { return mp_node; }
	OgreBulletDynamics::RigidBody * getBody () { return mp_body; }
	const Ogre::String & getName () { return m_name; }
	void move(const Ogre::Vector3 & force_vec);
	void resetPhysics();
	void setScale(const Ogre::Vector3 & scl);
	const Ogre::Vector3 & getScale() { return mp_node->getScale(); }
	void highlightVisual (bool b);
	void freeze (bool b);
	void setSelected (bool b) { m_selected = b; highlightVisual(b); }
	bool isSelected () { return m_selected; }
	void setPile (Pile * p) { mp_pile = p; }
	Pile * getPile () { return mp_pile; }
	bool hasPile () { return mp_pile != NULL;}
	bool isSelectable () { return mp_pile == NULL && !m_freeze; }
	const ObjectType & getObjectType () { return m_type; }
	
	static const char * objectTypeToStr (const ObjectType & obj_t);
	
private:
	static unsigned int ms_count;
	static unsigned int ms_incrementor;
	static const Ogre::Vector3 msc_scale;
	static const Ogre::Real msc_mass;
	static const char * msc_object_type_str [];
	
	Ogre::String m_name;
	Ogre::Entity * mp_entity;
	Ogre::SceneNode * mp_node;
	OgreBulletDynamics::RigidBody * mp_body;
	ObjectType m_type;
	Ogre::Real m_mass;
	Ogre::Real m_restitution;
	Ogre::Real m_friction;
	OgreBulletDynamics::DynamicsWorld * mp_world;
	Pile * mp_pile;
	bool m_selected;
	bool m_freeze;
};

//==============================================================================

} // namespace BombasticDesktop

//==============================================================================

#endif // OBJECT_H
