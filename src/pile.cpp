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

#include <pile.h>
#include <object.h>
#include <desktop.h>

//==============================================================================

namespace BombasticDesktop {

//==============================================================================

Ogre::Real const Pile::msc_item_hight_delim = 10.0f;

//------------------------------------------------------------------------------

Pile::Pile() 
	: m_wraped_up (false)
	, m_move_to_center (false)
{
}

//------------------------------------------------------------------------------

Pile::~Pile() {
}

//------------------------------------------------------------------------------

void Pile::move(const Ogre::Vector3 & pos) {
	Ogre::Vector3 p = pos;
	int i = 0;
	for (std::vector<Object *>::iterator itr = m_objects.begin()
		; itr != m_objects.end(); ++itr, ++i
	)
	{
		Object * obj = *itr;
		p.y = i * msc_item_hight_delim;
		obj->move(p);
	}
	
	m_center = pos;
}

//------------------------------------------------------------------------------

void Pile::insert(Object * obj) {
	m_objects.push_back(obj);
	obj->setPile(this);
	m_positions.push_back(obj->getNode()->_getDerivedPosition());
}

//------------------------------------------------------------------------------

void Pile::wrapUp() {
	if (m_positions.size() == 0)
		return;
	
	if (m_wraped_up)
		return;
	
	Ogre::Vector3 v;
	for (std::vector<Ogre::Vector3>::iterator itr = m_positions.begin()
		; itr != m_positions.end(); ++itr
	) 
		v += *itr;
		
	m_center = v / m_positions.size();
	
	for (std::vector<Ogre::Vector3>::iterator itr = m_positions.begin()
		; itr != m_positions.end(); ++itr
	) 
		*itr = *itr - m_center;
		
	m_wraped_up = true;

	int i = 1;
	for (std::vector<Object *>::iterator itr = m_objects.begin()
		; itr != m_objects.end(); ++itr, ++i
	)
	{
		Object * obj = *itr;
		Ogre::Vector3 cur_pos = obj->getNode()->_getDerivedPosition();
		Ogre::Vector3 delta = (m_center - cur_pos);
		Ogre::Vector3 new_pos = cur_pos + delta;
		new_pos.y = i * msc_item_hight_delim;
		if (!new_pos.isNaN())
			obj->move(new_pos);
	}
}

//------------------------------------------------------------------------------

void Pile::unpile()
{
	for (std::vector<Object *>::iterator itr = m_objects.begin()
		; itr != m_objects.end(); ++itr
	)
		(**itr).setPile(NULL);
	
	m_objects.clear();
}

//------------------------------------------------------------------------------

void Pile::grid()
{
	int dim = Ogre::Math::Ceil(Ogre::Math::Log2(m_objects.size()));
	
	int i = 0, j = 0;
	
	Ogre::Vector3 max_vec = m_center + Ogre::Vector3(dim * 200.0f, 0, dim * 200.0f);
	if (max_vec.x > Desktop::msc_room_width / 2.0f - 200.0f)
		m_center.x -= max_vec.x - Desktop::msc_room_width / 2.0f + 200.0f;
	if (max_vec.z > Desktop::msc_room_depth / 2.0f - 200.0f)
		m_center.z -= max_vec.z - Desktop::msc_room_depth / 2.0f + 200.0f;
	
	for (std::vector<Object *>::iterator itr = m_objects.begin()
		; itr != m_objects.end(); ++itr
	)
	{
		Object * obj = *itr;
		Ogre::Vector3 new_pos = m_center + Ogre::Vector3(i * 200.0f, 0, j * 200.0f);
		new_pos.y = msc_item_hight_delim;
		if (!new_pos.isNaN())
			obj->move(new_pos);
		
		obj->setPile(NULL);
		
		++i;
		if (i == dim)
		{
			i = 0;
			++j;
		}
	}
	
	m_objects.clear();
}

//------------------------------------------------------------------------------

void Pile::fanout()
{
	Ogre::Vector3 max_vec, min_vec;
	for (std::vector<Ogre::Vector3>::iterator itr = m_positions.begin()
		; itr != m_positions.end(); ++itr
	)
	{
		Ogre::Vector3 p = m_center + *itr;
		
		if (p.x > max_vec.x)
			max_vec.x = p.x;
		if (p.z > max_vec.z)
			max_vec.z = p.z;
		
		if (p.x < min_vec.x)
			min_vec.x = p.x;
		if (p.z < min_vec.z)
			min_vec.z = p.z;
	}
		
	if (max_vec.x > Desktop::msc_room_width / 2.0f - 200.0f)
		m_center.x -= max_vec.x - Desktop::msc_room_width / 2.0f + 200.0f;
	if (min_vec.x < - Desktop::msc_room_width / 2.0f + 200.0f)
		m_center.x += - min_vec.x - Desktop::msc_room_width / 2.0f + 200.0f;
	
	if (max_vec.z > Desktop::msc_room_depth / 2.0f - 200.0f)
		m_center.z -= max_vec.z - Desktop::msc_room_depth / 2.0f + 200.0f;
	if (min_vec.z < - Desktop::msc_room_depth / 2.0f + 200.0f)
		m_center.z += - min_vec.z - Desktop::msc_room_depth / 2.0f + 200.0f;
	
	int i = 0;
	for (std::vector<Object *>::iterator itr = m_objects.begin()
		; itr != m_objects.end(); ++itr, ++i
	)
	{
		Object * obj = *itr;
		Ogre::Vector3 p = m_center + m_positions.at(i);
		p.y = msc_item_hight_delim;
		obj->move(p);
		obj->setPile(NULL);
	}
	
	m_objects.clear();
}

//==============================================================================

} // namespace BombasticDesktop

//==============================================================================
