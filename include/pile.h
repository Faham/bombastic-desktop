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

#ifndef PILE_H
#define PILE_H

//==============================================================================

#include <Ogre.h>

//==============================================================================

namespace BombasticDesktop {

//==============================================================================

class Object;

//==============================================================================

class Pile
{
public:
	Pile();
	~Pile();
	
	void move(const Ogre::Vector3 & force_vec);
	void insert(Object * obj);
	void wrapUp();
	void unpile();
	void grid();
	void fanout();
	const std::vector<Object *> & getObjects () { return m_objects; }
	
private:
	static Ogre::Real const msc_item_hight_delim;
	
	std::vector<Object *> m_objects;
	std::vector<Ogre::Vector3> m_positions;
	Ogre::Vector3 m_center;
	bool m_wraped_up;
	bool m_move_to_center;
};

//==============================================================================

} // namespace BombasticDesktop

//==============================================================================

#endif // PILE_H
