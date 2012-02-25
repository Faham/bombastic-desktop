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

//#define BULLET_DEBUT 1

//==============================================================================

#include <desktop.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <OgreBulletDynamicsRigidBody.h>
#include <object.h>
#include <pile.h>

//==============================================================================

namespace BombasticDesktop {

//==============================================================================

const Ogre::String Desktop::msc_scene_name = "BombasticDesktop";
const Ogre::String Desktop::msc_camera_name = "BombasticCamera";
const Ogre::String Desktop::msc_window_title = "Bombastic Desktop - Faham Negini - CMPT811 - UofS";
const Ogre::Radian Desktop::msc_camera_start_pitch = Ogre::Radian(Ogre::Degree(-50.0f));
const Ogre::Radian Desktop::msc_camera_min_pitch = Ogre::Radian(Ogre::Degree(-5.0f));
const Ogre::Radian Desktop::msc_camera_max_pitch = Ogre::Radian(Ogre::Degree(-85.0f));
const Ogre::Vector3 Desktop::msc_camera_target_start_pos(0.0f, 0.0f, 0.0f);
const Ogre::ColourValue Desktop::msc_background_color(0.0f, 0.0f, 0.0f);
const Ogre::ColourValue Desktop::msc_ambient_light(0.5f, 0.5f, 0.5f);
const Ogre::Real Desktop::msc_mouse_rotate_speed = 0.005f;
const Ogre::Real Desktop::msc_mouse_zoom_speed = 0.3f;
const Ogre::Real Desktop::msc_mouse_start_zoom = 1700.0f;
const Ogre::Real Desktop::msc_mouse_min_zoom = 10.0f;
const Ogre::Real Desktop::msc_mouse_max_zoom = 2000.0f;
const Ogre::Real Desktop::msc_room_width = 1700.0f;
const Ogre::Real Desktop::msc_room_height = 700.0f;
const Ogre::Real Desktop::msc_room_depth = 1500.0f;
const Ogre::Real Desktop::msc_floor_restitution = 0.0f;
const Ogre::Real Desktop::msc_floor_friction = 0.8f;
const Ogre::Real Desktop::msc_scale_xz_factor = 0.05f;
const Ogre::Real Desktop::msc_max_scale_xz = 5.0f;
const Ogre::Real Desktop::msc_min_scale_xz = 1.0f;
const Ogre::Real Desktop::msc_scale_y_factor = 0.01f;
const Ogre::Real Desktop::msc_max_scale_y = 2.0f;
const Ogre::Real Desktop::msc_min_scale_y = 0.05f;
const Ogre::Real Desktop::msc_item_translation_height = 5.0f;

//------------------------------------------------------------------------------

Desktop::Desktop()
	: mp_root (NULL)
	, mp_renderwindow (NULL)
	, mp_viewport (NULL)
	, mp_scene_manager (NULL)
	, mp_camera (NULL)
	, mp_input_mgr (NULL)
	, mp_keyboard (NULL)
	, mp_mouse (NULL)
	, mp_log (NULL)
	, m_quit (false)
	, mp_ray_scene_query (NULL)
	, mp_plane_bounded_vol_list_scene_query (NULL)
	, mp_nd_cam_target_yaw (NULL)
	, mp_nd_cam_target_pitch (NULL)
	, mp_nd_camera (NULL)
	, mp_world (NULL)
	, mp_selected (NULL)
	, m_gravity_vector (0, -9.81f, 0)
	, m_bounds (Ogre::Vector3 (-msc_room_width, -msc_room_height, -msc_room_depth), Ogre::Vector3 (msc_room_width, msc_room_height, msc_room_depth))
	, m_movement_force (15)
	, m_mouse_move_treshold (0.0f)
	, mp_last_selected (NULL)
	, mp_rectangle_overlay (NULL)
	, m_scale_xz (false)
	, m_scale_xz_factor (0.0f)
	, m_scale_y (false)
	, m_scale_y_factor (0.0f)
	, m_batch_selecting (false)
	, mp_cegui_renderer (NULL)
	, m_ui_item_selected (false)
	, mp_selected_pile (NULL)
	, mp_popup_asked_pile (NULL)
	, m_ui_enabled (false)
{
}

//------------------------------------------------------------------------------

Desktop::~Desktop()
{
	shutdown();
}

//------------------------------------------------------------------------------

bool Desktop::frameStarted(const Ogre::FrameEvent& evt)
{
	return true;
}

//------------------------------------------------------------------------------

bool Desktop::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	return true;
}

//------------------------------------------------------------------------------

bool Desktop::frameEnded(const Ogre::FrameEvent& evt)
{
	mp_keyboard->capture();
	mp_mouse->capture();
	mp_world->stepSimulation(evt.timeSinceLastFrame);	
	mp_world->stepSimulation(evt.timeSinceLastFrame);	
	mp_world->stepSimulation(evt.timeSinceLastFrame);	
	mp_world->stepSimulation(evt.timeSinceLastFrame);	
	mp_world->stepSimulation(evt.timeSinceLastFrame);	
	mp_world->stepSimulation(evt.timeSinceLastFrame);	
	mp_world->stepSimulation(evt.timeSinceLastFrame);	
	mp_world->stepSimulation(evt.timeSinceLastFrame);	
	mp_world->stepSimulation(evt.timeSinceLastFrame);
	
	if (CEGUI::WindowManager::getSingleton().getWindow("Root/BatchSelect/Popup")->isVisible())
		return true;
	
	if (mp_selected || mp_selected_pile)
	{
		Ogre::Ray ray = mp_camera->getCameraToViewportRay(
			Ogre::Real(mp_mouse->getMouseState().X.abs) / Ogre::Real(mp_viewport->getActualWidth())
			, Ogre::Real(mp_mouse->getMouseState().Y.abs) / Ogre::Real(mp_viewport->getActualHeight())
		);
		
		std::pair<bool, Ogre::Real> res = Ogre::Math::intersects(
			ray, Ogre::Plane(Ogre::Vector3::UNIT_Y, 0)
		);
		
		if (mp_selected)
		{
			if (res.first)
			{
				Ogre::Vector3 pos = ray.getPoint(res.second);
				pos.y = mp_selected->getNode()->getScale().y * 100 / 2 + msc_item_translation_height;
				Ogre::Vector3 delta_pos = pos - mp_selected->getNode()->_getDerivedPosition();
				if (pos.x + mp_selected->getNode()->getScale().x * 100 / 2.0 < msc_room_width / 2.0
					&& pos.x - mp_selected->getNode()->getScale().x * 100 / 2.0 > - msc_room_width / 2.0
					&& pos.z + mp_selected->getNode()->getScale().z * 100 / 2.0 < msc_room_depth / 2.0
					&& pos.z - mp_selected->getNode()->getScale().z * 100 / 2.0 > - msc_room_depth / 2.0
				)
					mp_selected->move(pos);
				if (m_selected_vector.size() > 0)
				{
					int i = 1;
					for (std::vector<Object *>::iterator itr = m_selected_vector.begin()
						; itr != m_selected_vector.end(); ++itr, ++i
					)
					{
						Object * obj = *itr;
						if (obj != mp_selected)
						{
							Ogre::Vector3 new_pos = obj->getNode()->_getDerivedPosition() + delta_pos;
							new_pos.y = obj->getNode()->getScale().y * 100 / 2 + msc_item_translation_height * i;
							if (new_pos.x + mp_selected->getNode()->getScale().x * 100 / 2.0 < msc_room_width / 2.0
								&& new_pos.x - mp_selected->getNode()->getScale().x * 100 / 2.0 > - msc_room_width / 2.0
								&& new_pos.z + mp_selected->getNode()->getScale().z * 100 / 2.0 < msc_room_depth / 2.0
								&& new_pos.z - mp_selected->getNode()->getScale().z * 100 / 2.0 > - msc_room_depth / 2.0
							)
								obj->move(new_pos);
						}
					}
				}
			}
			
			if (m_scale_xz)
			{
				Ogre::Vector3 vec = mp_selected->getScale();
				if (vec.x + m_scale_xz_factor < msc_max_scale_xz 
					&& vec.x + m_scale_xz_factor > msc_min_scale_xz)
				{
					vec.x += m_scale_xz_factor;
					vec.z += m_scale_xz_factor;
					mp_selected->setScale(vec);
				}
			}
			
			if (m_scale_y)
			{
				if (mp_selected->getScale().y + m_scale_y_factor < msc_max_scale_y
					&& mp_selected->getScale().y + m_scale_y_factor > msc_min_scale_y
				)
				{
					mp_selected->setScale(
						Ogre::Vector3(
							mp_selected->getScale().x
							, mp_selected->getScale().y + m_scale_y_factor
							, mp_selected->getScale().z
						)
					);
				}
			}
		}
		else if (mp_selected_pile)
		{
			Ogre::Vector3 pos = ray.getPoint(res.second);
			mp_selected_pile->move(pos);
		}
	}
	
	if (m_mouse_move_treshold > 0)
		m_mouse_move_treshold -=  evt.timeSinceLastFrame;
	
	if (NULL == mp_selected && NULL != mp_last_selected && m_mouse_move_treshold > 0)
	{
		Ogre::Vector3 _cam_forward = mp_camera->getDerivedUp().crossProduct(mp_camera->getDerivedRight());
		Ogre::Vector3 _cam_right = mp_camera->getDerivedRight();
		
		Ogre::Vector3 movement =
			+ m_last_mouse_move_state.X.rel 
			* Ogre::Vector3(_cam_right.x, 0, _cam_right.z)
			- m_last_mouse_move_state.Y.rel 
			* Ogre::Vector3(_cam_forward.x, 0, _cam_forward.z)
		;

		mp_last_selected->getBody()->applyImpulse(movement * m_movement_force * 5, Ogre::Vector3::ZERO);
		if (m_selected_vector.size() > 0)
			for (std::vector<Object *>::iterator itr = m_selected_vector.begin()
				; itr != m_selected_vector.end(); ++itr
			)
			{
				Object * obj = *itr;
				obj->getBody()->applyImpulse(movement * m_movement_force * 5, Ogre::Vector3::ZERO);
			}
			

		m_mouse_move_treshold = 0;
		mp_last_selected = NULL;
	}
		
	return true;
}

//------------------------------------------------------------------------------

void Desktop::windowResized(Ogre::RenderWindow* rw)
{
	rw->windowMovedOrResized();
	
	if (mp_viewport && mp_camera)
	{
		mp_camera->setAspectRatio(
			Ogre::Real(mp_viewport->getActualWidth()) 
			/ Ogre::Real(mp_viewport->getActualHeight())
		);
	}
}

//------------------------------------------------------------------------------

void Desktop::windowClosed(Ogre::RenderWindow* rw)
{
	m_quit = true;
}

//------------------------------------------------------------------------------

bool Desktop::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_ESCAPE)
	{
		m_quit = true;
	}

	if(arg.key == OIS::KC_B)
	{
		m_objects.push_back(
			new Object(
				mp_scene_manager
				, mp_world
				, Object::OT_Unknown
			)
		);
	}
	
	if (NULL != mp_selected)
	{
		if (arg.key == OIS::KC_DOWN || arg.key == OIS::KC_UP)
		{
			m_scale_xz = true;
		
			if (arg.key == OIS::KC_DOWN)
				m_scale_xz_factor = -msc_scale_xz_factor;
			else if (arg.key == OIS::KC_UP)
				m_scale_xz_factor = msc_scale_xz_factor;
		}
		
		if (arg.key == OIS::KC_LEFT || arg.key == OIS::KC_RIGHT)
		{
			m_scale_y = true;
			
			if (arg.key == OIS::KC_LEFT)
				m_scale_y_factor = -msc_scale_y_factor;
			else if (arg.key == OIS::KC_RIGHT)
				m_scale_y_factor = msc_scale_y_factor;
		}
	}

	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectKeyDown(arg.key);
	sys.injectChar(arg.text);
	
	return true;
}

//------------------------------------------------------------------------------

bool Desktop::keyReleased(const OIS::KeyEvent &arg)		
{
	if (arg.key == OIS::KC_DOWN || arg.key == OIS::KC_UP)
		m_scale_xz = false;
	
	if (arg.key == OIS::KC_LEFT || arg.key == OIS::KC_RIGHT)
		m_scale_y = false;
	
	CEGUI::System::getSingleton().injectKeyUp(arg.key);
	
	return true;
}

//------------------------------------------------------------------------------

bool Desktop::mouseMoved( const OIS::MouseEvent &arg )
{
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
	if (arg.state.Z.rel)
		sys.injectMouseWheelChange(arg.state.Z.rel / 120.0f);
	
	if (m_ui_enabled)
	{
		if (m_ui_item_selected)
		{
			m_ui_item_selected = false;
			m_ui_enabled = false;
		}
		
		return true;
	}
	
	if (arg.state.buttonDown(OIS::MB_Left))
	{
		if ( NULL != mp_selected && (arg.state.X.rel != 0 || arg.state.Y.rel != 0))
		{
			m_last_mouse_move_state = arg.state;
			m_mouse_move_treshold = 0.5f;
		}
		
		if (!mp_selected && !mp_selected_pile)
		{
			int x1 = std::min(m_last_mouse_left_press_state.X.abs, arg.state.X.abs);
			int y1 = std::min(m_last_mouse_left_press_state.Y.abs, arg.state.Y.abs);
			int x2 = std::max(m_last_mouse_left_press_state.X.abs, arg.state.X.abs);
			int y2 = std::max(m_last_mouse_left_press_state.Y.abs, arg.state.Y.abs);
			
			mp_rectangle_overlay->setTop(Ogre::Real(y1) / mp_viewport->getActualHeight());
			mp_rectangle_overlay->setLeft(Ogre::Real(x1) / mp_viewport->getActualWidth());
			mp_rectangle_overlay->setWidth(Ogre::Real(x2 - x1) / mp_viewport->getActualWidth());
			mp_rectangle_overlay->setHeight(Ogre::Real(y2 - y1) / mp_viewport->getActualHeight());
			
			Ogre::OverlayManager::getSingleton().getByName("BombasticDesktop/Rectangle")->show();
			m_batch_selecting = true;
		}
	}
	else
	{
		if (arg.state.buttonDown(OIS::MB_Middle))
		{
			mp_nd_cam_target_yaw->yaw(Ogre::Radian(- arg.state.X.rel * msc_mouse_rotate_speed));
			
			Ogre::Radian _p = Ogre::Radian(- arg.state.Y.rel * msc_mouse_rotate_speed);
		
			if (mp_nd_cam_target_pitch->getOrientation().getPitch() + _p
				< msc_camera_min_pitch
				&& mp_nd_cam_target_pitch->getOrientation().getPitch() + _p
				> msc_camera_max_pitch
			)
				mp_nd_cam_target_pitch->pitch(_p);
			
			log ("camera zoom operation");
		}
		if (arg.state.buttonDown(OIS::MB_Right))
		{
			Ogre::Vector3 _cam_forward = mp_camera->getDerivedUp().crossProduct(mp_camera->getDerivedRight());
			Ogre::Vector3 _cam_right = mp_camera->getDerivedRight();
			mp_nd_cam_target_yaw->translate(
				- arg.state.X.rel * Ogre::Vector3(_cam_right.x, 0, _cam_right.z)
				+ arg.state.Y.rel * Ogre::Vector3(_cam_forward.x, 0, _cam_forward.z)
			);
		}

		if (arg.state.Z.rel != 0)
		{
			Ogre::Real _z = arg.state.Z.rel * msc_mouse_zoom_speed;
			if (mp_nd_camera->getPosition().z + _z < msc_mouse_max_zoom
				&& mp_nd_camera->getPosition().z + _z > msc_mouse_min_zoom)
				mp_nd_camera->translate(0, 0, _z);
		}
	}
	
	return true;
}

//------------------------------------------------------------------------------

bool Desktop::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	CEGUI::System::getSingleton().injectMouseButtonDown(convertMouseButton(id));
	
	if (m_ui_enabled)
	{
		if (m_ui_item_selected)
		{
			m_ui_item_selected = false;
			m_ui_enabled = false;
		}

		/*
		CEGUI::WindowManager::getSingleton().getWindow("Root/Pile/Popup")->deactivate();
		CEGUI::WindowManager::getSingleton().getWindow("Root/BatchSelect/Popup")->deactivate();
		CEGUI::WindowManager::getSingleton().getWindow("Root/Pile/Popup")->hide();
		CEGUI::WindowManager::getSingleton().getWindow("Root/BatchSelect/Popup")->hide();
		 */
		
		return true;
	}
	
	if (OIS::MB_Left == id)
	{		
		mp_ray_scene_query->setRay(
			mp_camera->getCameraToViewportRay(
				Ogre::Real(arg.state.X.abs) / Ogre::Real(mp_viewport->getActualWidth())
				, Ogre::Real(arg.state.Y.abs) / Ogre::Real(mp_viewport->getActualHeight())
			)
		);

		Ogre::RaySceneQueryResult & res = mp_ray_scene_query->execute();
		
		if (res.size() > 0)
		{
			Object * obj = Ogre::any_cast<Object *>(
				res.begin()->movable->getUserAny()
			);
			
			if (obj->isSelectable())
			{
				if (false == obj->isSelected())
				{
					if (mp_keyboard->isKeyDown(OIS::KC_LCONTROL)
						|| mp_keyboard->isKeyDown(OIS::KC_RCONTROL)
					)
						m_selected_vector.push_back(obj);
					else
						unselectVector();
						
					if (m_selected_vector.size() == 0)
						m_selected_vector.push_back(obj);
					
					obj->setSelected(true);
					log ("selected 1 object by mouse click");
				}
				else if (mp_keyboard->isKeyDown(OIS::KC_LSHIFT)
					|| mp_keyboard->isKeyDown(OIS::KC_RSHIFT))
				{
					obj->setSelected(false);
					if (mp_selected == obj)
					{
						mp_last_selected = mp_selected;
						mp_selected = NULL;
					}
					
					for (std::vector<Object *>::iterator itr = m_selected_vector.begin()
						; itr != m_selected_vector.end(); ++itr
					)
						if (*itr == obj)
						{
							m_selected_vector.erase(itr);
							break;
						}
					
					log ("unselected 1 object by mouse click");
				}
			}
			else if (obj->hasPile())
			{
				mp_selected_pile = obj->getPile();
			}
			
			if (obj->isSelected() && mp_selected != obj)
				mp_selected = obj;
			
			if (mp_selected)
				log ("move operation for " 
					+ Ogre::StringConverter::toString(m_selected_vector.size()) 
					+ " number of objects"
				);
			else if (mp_selected_pile)
				log ("move operation for pile");
		}
		else if (false == mp_keyboard->isKeyDown(OIS::KC_LSHIFT)
			&& false == mp_keyboard->isKeyDown(OIS::KC_RSHIFT)
			&& false == mp_keyboard->isKeyDown(OIS::KC_RCONTROL)
			&& false == mp_keyboard->isKeyDown(OIS::KC_LCONTROL)
		)
		{
			unselectVector();
		}
	}
	else if (OIS::MB_Right == id)
	{
		mp_ray_scene_query->setRay(
			mp_camera->getCameraToViewportRay(
				Ogre::Real(arg.state.X.abs)
					/ Ogre::Real(mp_viewport->getActualWidth())
				, Ogre::Real(arg.state.Y.abs)
					/ Ogre::Real(mp_viewport->getActualHeight())
			)
		);

		Ogre::RaySceneQueryResult & res = mp_ray_scene_query->execute();
	
		if (res.size() > 0)
		{
			Object * obj = Ogre::any_cast<Object *>(
				res.begin()->movable->getUserAny()
			);
			
			float x = float(arg.state.X.abs) / mp_viewport->getActualWidth();
			float y = float(arg.state.Y.abs) / mp_viewport->getActualHeight();
			if (1.0f - y < 0.15f)
				y = 0.85f;
			
			if (obj->isSelected() && m_selected_vector.size() > 0)
			{
				log ("batch selection popup menu for " + Ogre::StringConverter::toString(m_selected_vector.size()) + " objects");
				
				CEGUI::WindowManager::getSingleton()
					.getWindow("Root/BatchSelect/Popup")->setPosition(
						CEGUI::UVector2(
							CEGUI::UDim(x, 0.0f)
							, CEGUI::UDim(y, 0.0f)
						)
					);
					
				CEGUI::WindowManager::getSingleton().getWindow("Root/BatchSelect/Popup")->show();
				m_ui_enabled = true;
			}
			else if (obj->hasPile())
			{
				log ("pile popup menu");
				
				mp_popup_asked_pile = obj->getPile();
				
				CEGUI::WindowManager::getSingleton()
					.getWindow("Root/Pile/Popup")->setPosition(
						CEGUI::UVector2(
							CEGUI::UDim(x, 0.0f)
							, CEGUI::UDim(y, 0.0f)
						)
					);
					
				CEGUI::WindowManager::getSingleton().getWindow("Root/Pile/Popup")->show();
				m_ui_enabled = true;
			}
		}
		else
			log ("camera pan operation");
	}
	else if (OIS::MB_Middle == id)
		log ("camera rotate operation");
	
	m_last_mouse_left_press_state = arg.state;
	
	return true;
}

//------------------------------------------------------------------------------

bool Desktop::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	CEGUI::System::getSingleton().injectMouseButtonUp(convertMouseButton(id));
	
	if (m_ui_enabled)
	{
		if (m_ui_item_selected)
		{
			m_ui_item_selected = false;
			m_ui_enabled = false;
		}
		
		return true;
	}
	
	
	if (OIS::MB_Left == id)
	{
		if (mp_selected)
		{
			if (m_selected_vector.size() == 0)
				mp_selected->setSelected(false);

			mp_last_selected = mp_selected;
			mp_selected = NULL;

			int freezed_objects = 0;
			for (std::vector<Object *>::iterator it1 = m_selected_vector.begin()
				; it1 != m_selected_vector.end();
			)
			{
				Object * obj = *it1;
				bool obj_removed = false;
				for (std::map<Ogre::String, Ogre::Entity *>::iterator itr = 
					m_cubes.begin(); itr != m_cubes.end(); ++itr
				)
				{
					if (itr->second->getParentSceneNode()->_getWorldAABB().intersects(obj->getNode()->_getWorldAABB())
						&& itr->first == Object::objectTypeToStr(obj->getObjectType())
					)
					{
						unselect(obj);
						for (std::deque<Object *>::iterator it2 = m_objects.begin()
							; it2 != m_objects.end(); ++it2
						)
							if (*it2 == obj)
							{
								m_objects.erase(it2);
								break;
							}
						
						obj->freeze(true);
						obj_removed = true;
						mp_last_selected = NULL;
						break;
					}
				}
				
				if (obj_removed)
					++freezed_objects;
				else
					++it1;
			}
			
			if (freezed_objects > 0)
				log (Ogre::StringConverter::toString(freezed_objects) + " number of objects freezed");
			
			if (m_objects.size() == 0)
				m_quit = true;
		}
		else if (mp_selected_pile)
			mp_selected_pile = NULL;
	}

	if (m_batch_selecting)
	{
		m_batch_selecting = false;
		
		Ogre::OverlayManager::getSingleton().getByName("BombasticDesktop/Rectangle")->hide();
		
		int x1 = std::min(m_last_mouse_left_press_state.X.abs, arg.state.X.abs);
		int y1 = std::min(m_last_mouse_left_press_state.Y.abs, arg.state.Y.abs);
		int x2 = std::max(m_last_mouse_left_press_state.X.abs, arg.state.X.abs);
		int y2 = std::max(m_last_mouse_left_press_state.Y.abs, arg.state.Y.abs);
		
		mp_rectangle_overlay->setTop(Ogre::Real(y1) / mp_viewport->getActualHeight());
		mp_rectangle_overlay->setLeft(Ogre::Real(x1) / mp_viewport->getActualWidth());
		mp_rectangle_overlay->setWidth(Ogre::Real(x2 - x1) / mp_viewport->getActualWidth());
		mp_rectangle_overlay->setHeight(Ogre::Real(y2 - y1) / mp_viewport->getActualHeight());
		selectObjectsInRectangle(
			Ogre::Real(x1) / mp_viewport->getActualWidth()
			, Ogre::Real(y1) / mp_viewport->getActualHeight()
			, Ogre::Real(x2) / mp_viewport->getActualWidth()
			, Ogre::Real(y2) / mp_viewport->getActualHeight()
		);
	}
	
	return true;
}

//------------------------------------------------------------------------------

void Desktop::go()
{
	while( !m_quit )
	{
		Ogre::WindowEventUtilities::messagePump();
		
		if (!mp_root->renderOneFrame())
			break;
	}
}

//------------------------------------------------------------------------------

void Desktop::locateResources()
{
	Ogre::ConfigFile cf;
	cf.load("resources.cfg");
	
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	Ogre::String sec, type, arch;
	
	while (seci.hasMoreElements())
	{
		sec = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		
		for (i = settings->begin(); i != settings->end(); i++)
		{
			type = i->first;
			arch = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch, type, sec);
		}
	}
}

//------------------------------------------------------------------------------

bool Desktop::setup()
{
	mp_root = new Ogre::Root();
	
	if (!mp_root)
		return false;
		
	if (!mp_root->restoreConfig() && !mp_root->showConfigDialog()) 
		return false;
	
	mp_renderwindow = mp_root->initialise(true, msc_window_title);
	
	locateResources();

	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Essential");
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Popular");
	//Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	
	setupInput();
	
	mp_scene_manager = mp_root->createSceneManager(Ogre::ST_GENERIC, msc_scene_name);
	mp_scene_manager->setAmbientLight(msc_ambient_light);
	mp_scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
	mp_scene_manager->setShadowTextureSelfShadow(true);
	mp_scene_manager->setShadowTextureSize(2048);
	
	mp_camera = mp_scene_manager->createCamera(msc_camera_name);
	setupCamera();

	mp_viewport = mp_renderwindow->addViewport(mp_camera);
	mp_viewport->setBackgroundColour(msc_background_color);
	
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	
	mp_root->addFrameListener(this);
	Ogre::WindowEventUtilities::addWindowEventListener(mp_renderwindow, this);

	mp_log = Ogre::LogManager::getSingleton().createLog("BombasticDesktop.log");
	mp_log->setTimeStampEnabled(true);
	
	setupBullet();
	setupEntities();	
	setupLights();
	setupCEGUI();
	setupScene();
	
	mp_ray_scene_query = mp_scene_manager->createRayQuery(Ogre::Ray());
	mp_ray_scene_query->setSortByDistance(true, 1);
	mp_ray_scene_query->setQueryMask(OQF_DefaultSelectable);

	mp_plane_bounded_vol_list_scene_query = mp_scene_manager->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());
	mp_plane_bounded_vol_list_scene_query->setQueryTypeMask(OQF_DefaultSelectable);
	
	mp_rectangle_overlay = dynamic_cast<Ogre::OverlayContainer *>(
		Ogre::OverlayManager::getSingleton().getOverlayElement(
			"BombasticDesktop/Rectangle/BorderPanel"
		)
	);
}

//------------------------------------------------------------------------------

void Desktop::setupInput()
{
	OIS::ParamList pl;
	size_t whdl = 0;
	std::ostringstream whdl_str;
	mp_renderwindow->getCustomAttribute("WINDOW", &whdl);
	whdl_str << whdl;
	
	pl.insert(std::make_pair("WINDOW", whdl_str.str()));
	pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
//	pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
//	pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
//	pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
	
	mp_input_mgr = OIS::InputManager::createInputSystem(pl);
	mp_keyboard = static_cast<OIS::Keyboard*>(mp_input_mgr->createInputObject(OIS::OISKeyboard, true));
	mp_mouse = static_cast<OIS::Mouse*>(mp_input_mgr->createInputObject(OIS::OISMouse, true));
	mp_keyboard->setEventCallback(this);
	mp_mouse->setEventCallback(this);
}

//------------------------------------------------------------------------------

void Desktop::setupCamera()
{
	mp_camera->setQueryFlags(OQF_NotSelectable);
	mp_nd_cam_target_yaw = mp_scene_manager->getRootSceneNode()->createChildSceneNode("scenenode camera target yaw", msc_camera_target_start_pos);
	mp_nd_cam_target_pitch = mp_nd_cam_target_yaw->createChildSceneNode("scenenode camera target pitch");
	mp_nd_camera = mp_nd_cam_target_pitch->createChildSceneNode("scenenode camera", Ogre::Vector3(0, 0, msc_mouse_start_zoom));
	mp_nd_cam_target_pitch->pitch(msc_camera_start_pitch);
	mp_nd_camera->attachObject(mp_camera);
	mp_camera->lookAt(0, 0, 0);
	mp_camera->setNearClipDistance(5);
}

//------------------------------------------------------------------------------

void Desktop::setupLights()
{
	Ogre::Light* pointLight = mp_scene_manager->createLight("pointLight");
	pointLight->setType(Ogre::Light::LT_POINT);
	pointLight->setPosition(Ogre::Vector3(0, 350, 1000));
	
	pointLight->setDiffuseColour(1.0, 1.0, 1.0);
	pointLight->setSpecularColour(1.0, 1.0, 1.0);
	
	Ogre::Light* directionalLight = mp_scene_manager->createLight("directionalLight");
	directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
	directionalLight->setDiffuseColour(Ogre::ColourValue(.25, .25, 0));
	directionalLight->setSpecularColour(Ogre::ColourValue(.25, .25, 0));
	pointLight->setCastShadows(false);
	directionalLight->setDirection(Ogre::Vector3( 0, -3, 3 ));
}

//------------------------------------------------------------------------------

void Desktop::setupEntities()
{
	Ogre::SceneNode * room_node = mp_scene_manager->getRootSceneNode()->createChildSceneNode("room");
	
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
		, plane, msc_room_width, msc_room_depth
		, 20, 20, true, 1, 1, 1, Ogre::Vector3::UNIT_Z
	);

	Ogre::MeshManager::getSingleton().createPlane(
		"side-wall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
		, plane, msc_room_depth, msc_room_height
		, 20, 20, true, 1, 1, 1, Ogre::Vector3::UNIT_Z
	);

	Ogre::MeshManager::getSingleton().createPlane(
		"front-wall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
		, plane, msc_room_width, msc_room_height
		, 20, 20, true, 1, 1, 1, Ogre::Vector3::UNIT_Z
	);
	
	// Ground Plane
	Ogre::Entity * ent = mp_scene_manager->createEntity("floor", "ground");
	ent->setMaterialName("room/ground");
	ent->setCastShadows(false);
	ent->setQueryFlags(OQF_NotSelectable);
	Ogre::SceneNode * node = room_node->createChildSceneNode("ground-node");
	node->attachObject(ent);
	node->yaw(Ogre::Degree(180));
	
	OgreBulletCollisions::CollisionShape *shape
		= new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3::UNIT_Y, 0);
		OgreBulletDynamics::RigidBody *rb = new OgreBulletDynamics::RigidBody("floor", mp_world);
	rb->setStaticShape(shape, 0.1, 0.8);

	// Ceil Plane
	shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3::NEGATIVE_UNIT_Y, - msc_room_height);
	rb = new OgreBulletDynamics::RigidBody("ceil", mp_world);
	rb->setStaticShape(shape, msc_floor_restitution, msc_floor_friction);
	
	// Left Plane
	ent = mp_scene_manager->createEntity("left", "side-wall");
	ent->setMaterialName("room/wall");
	ent->setCastShadows(false);
	ent->setQueryFlags(OQF_NotSelectable);
	node = room_node->createChildSceneNode("left-node");
	node->attachObject(ent);
	node->setPosition(- msc_room_width / 2, msc_room_height / 2, 0);
	node->roll(Ogre::Degree(-90));
	node->yaw(Ogre::Degree(-90));
	
	shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3::UNIT_X, - msc_room_width / 2);
	rb = new OgreBulletDynamics::RigidBody("left", mp_world);
	rb->setStaticShape(shape, msc_floor_restitution, msc_floor_friction);

	// Right Plane
	ent = mp_scene_manager->createEntity("right", "side-wall");
	ent->setMaterialName("room/wall");
	ent->setCastShadows(false);
	ent->setQueryFlags(OQF_NotSelectable);
	node = room_node->createChildSceneNode("right-node");
	node->attachObject(ent);
	node->setPosition(msc_room_width / 2, msc_room_height / 2, 0);
	node->roll(Ogre::Degree(90));
	node->yaw(Ogre::Degree(90));
	
	shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3::NEGATIVE_UNIT_X, - msc_room_width / 2);
	rb = new OgreBulletDynamics::RigidBody("right", mp_world);
	rb->setStaticShape(shape, msc_floor_restitution, msc_floor_friction);

	// Front Plane
	ent = mp_scene_manager->createEntity("front", "front-wall");
	ent->setMaterialName("room/wall");
	ent->setCastShadows(false);
	ent->setQueryFlags(OQF_NotSelectable);
	node = room_node->createChildSceneNode("front-node");
	node->attachObject(ent);
	node->setPosition(0, msc_room_height / 2, - msc_room_depth / 2);
	node->pitch(Ogre::Degree(90));
	node->yaw(Ogre::Degree(180));
	
	shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3::UNIT_Z, - msc_room_depth / 2);
	rb = new OgreBulletDynamics::RigidBody("front", mp_world);
	rb->setStaticShape(shape, msc_floor_restitution, msc_floor_friction);
	
	// Rear Plane
	ent = mp_scene_manager->createEntity("rear", "front-wall");
	ent->setMaterialName("room/wall");
	ent->setCastShadows(false);
	ent->setQueryFlags(OQF_NotSelectable);
	node = room_node->createChildSceneNode("rear-node");
	node->attachObject(ent);
	node->setPosition(0, msc_room_height / 2, msc_room_depth / 2);
	node->pitch(Ogre::Degree(-90));
	
	shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3::NEGATIVE_UNIT_Z, - msc_room_depth / 2);
	rb = new OgreBulletDynamics::RigidBody("rear", mp_world);
	rb->setStaticShape(shape, msc_floor_restitution, msc_floor_friction);
}

//------------------------------------------------------------------------------

void Desktop::setupBullet()
{
	mp_world = new OgreBulletDynamics::DynamicsWorld(mp_scene_manager, m_bounds, m_gravity_vector);
#if defined(BULLET_DEBUT)
	mp_debug_drawer = new OgreBulletCollisions::DebugDrawer();
	mp_debug_drawer->setDrawWireframe(true);
	mp_world->setDebugDrawer(mp_debug_drawer);
	mp_world->setShowDebugShapes(true);
	Ogre::SceneNode * node = mp_scene_manager->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
	node->attachObject(static_cast <Ogre::SimpleRenderable *> (mp_debug_drawer));
#endif
}

//------------------------------------------------------------------------------

void Desktop::setupCEGUI()
{
	mp_cegui_renderer = &CEGUI::OgreRenderer::bootstrapSystem();
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
	CEGUI::MouseCursor::getSingleton().setImage( CEGUI::System::getSingleton().getDefaultMouseCursor());
	CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(0.0f, 0.0f));

	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();

	CEGUI::Window* root_sheet = wmgr.loadWindowLayout("root.layout");
	CEGUI::System::getSingleton().setGUISheet(root_sheet);

	wmgr.getWindow("Root/BatchSelect/Popup/Pile")->subscribeEvent(CEGUI::Window::EventMouseButtonUp,	CEGUI::Event::Subscriber(&Desktop::onPopupPile, this));
	wmgr.getWindow("Root/BatchSelect/Popup")->hide();

	wmgr.getWindow("Root/Pile/Popup/Unpile")->subscribeEvent(CEGUI::Window::EventMouseButtonUp,	CEGUI::Event::Subscriber(&Desktop::onPopupUnpile, this));
	wmgr.getWindow("Root/Pile/Popup/Grid")->subscribeEvent(CEGUI::Window::EventMouseButtonUp,	CEGUI::Event::Subscriber(&Desktop::onPopupGrid, this));
	wmgr.getWindow("Root/Pile/Popup/FanOut")->subscribeEvent(CEGUI::Window::EventMouseButtonUp,	CEGUI::Event::Subscriber(&Desktop::onPopupFanOut, this));
	wmgr.getWindow("Root/Pile/Popup")->hide();
}

//------------------------------------------------------------------------------

void Desktop::setupScene()
{
	for (int i = 10; i < 60; ++i)
	{
		Object * obj = new Object(
			mp_scene_manager
			, mp_world
			, Object::ObjectType(i / 10)
		);
		
		m_objects.push_back(obj);
		
		obj->move(Ogre::Vector3(
			Ogre::Math::RangeRandom(- msc_room_width / 2.0 + obj->getScale().x * 100/ 2, msc_room_width / 2.0 - obj->getScale().x * 100/ 2) 
			, Ogre::Math::RangeRandom(obj->getNode()->getScale().y * 100 / 2 + msc_item_translation_height, obj->getNode()->getScale().y * 100 / 2 + msc_item_translation_height * 5)
			, Ogre::Math::RangeRandom(- msc_room_depth / 2.0 + obj->getScale().z * 100/ 2, msc_room_depth / 2.0 - obj->getScale().z * 100/ 2)
		));
	}
	
	log ("Number of created objects: " + Ogre::StringConverter::toString(m_objects.size()));
	
	Ogre::String name_arr[5] = {
		"Folder"
		, "Image"
		, "Video"
		, "Audio"
		, "Document"
	};
	
	for (int i = 0; i < 5; ++i)
	{
		Ogre::String name = "cube" + Ogre::StringConverter::toString(i);
		Ogre::Entity * ent = mp_scene_manager->createEntity(name, "cube.mesh");
		ent->setMaterialName("BombasticDesktop/Target/" + name_arr[i]);
		ent->setQueryFlags(OQF_NotSelectable);
		ent->setCastShadows(false);
		Ogre::SceneNode * node = mp_scene_manager->getRootSceneNode()->createChildSceneNode();
		node->attachObject(ent);
		Ogre::Vector3 scale(0.30f, 5.0f, 0.30f);
		node->scale(scale);
		node->setPosition(Ogre::Vector3(
			Ogre::Math::RangeRandom(- msc_room_width / 2.0 + scale.x * 100/ 2, msc_room_width / 2.0 - scale.x * 100/ 2) 
			, 0.0f
			, Ogre::Math::RangeRandom(- msc_room_depth / 2.0 + scale.z * 100/ 2, msc_room_depth / 2.0 - scale.z * 100/ 2)
		));
		
		m_cubes[name_arr[i]] = ent;
	}
}

//------------------------------------------------------------------------------

void Desktop::shutdownBullet()
{	
	delete mp_world;
}

//------------------------------------------------------------------------------

void Desktop::shutdown()
{
	if (mp_root)
	{
		Ogre::WindowEventUtilities::removeWindowEventListener(mp_renderwindow, this);
		mp_root->removeFrameListener(this);

		mp_renderwindow->removeAllViewports();
		mp_viewport = NULL;
		
		mp_scene_manager->destroyQuery(mp_ray_scene_query);
		mp_ray_scene_query = NULL;

		mp_root->destroySceneManager(mp_scene_manager);
		mp_scene_manager = NULL;
		
		mp_root->shutdown();
		delete mp_root;
		mp_root = NULL;
	}

	shutdownInput();
	shutdownBullet();
}

//------------------------------------------------------------------------------

void Desktop::shutdownInput()
{
	if (mp_input_mgr)
	{
		mp_input_mgr->destroyInputObject(mp_keyboard);
		mp_keyboard = NULL;

		mp_input_mgr->destroyInputObject(mp_mouse);
		mp_mouse = NULL;
		
		OIS::InputManager::destroyInputSystem(mp_input_mgr);
		mp_input_mgr = NULL;
	}
}

//------------------------------------------------------------------------------

void Desktop::selectObjectsInRectangle(
	const Ogre::Real & x1, const Ogre::Real & y1
	, const Ogre::Real & x2, const Ogre::Real & y2
)
{
	Ogre::PlaneBoundedVolumeList volume_list;
	volume_list.resize(1);
	mp_camera->getCameraToViewportBoxVolume(x1, y1, x2, y2, &(volume_list[0]));
	
	mp_plane_bounded_vol_list_scene_query->setVolumes(volume_list);

	Ogre::SceneQueryResult result = mp_plane_bounded_vol_list_scene_query->execute();

	if (false == mp_keyboard->isKeyDown(OIS::KC_LSHIFT)
		&& false == mp_keyboard->isKeyDown(OIS::KC_RSHIFT)
		&& false == mp_keyboard->isKeyDown(OIS::KC_RCONTROL)
		&& false == mp_keyboard->isKeyDown(OIS::KC_LCONTROL)
	)
		unselectVector();
	
	if (false == mp_keyboard->isKeyDown(OIS::KC_LSHIFT)
		&& false == mp_keyboard->isKeyDown(OIS::KC_RSHIFT)
	)
	{
		for (Ogre::SceneQueryResultMovableList::iterator itr = result.movables.begin()
			; itr != result.movables.end(); ++itr
		)
		{
			Ogre::MovableObject* obj = *itr;

			for (std::deque<Object *>::iterator itr2 = m_objects.begin(); itr2 != m_objects.end(); ++itr2)
			{
				if ((**itr2).getName() == obj->getName())
				{
					if ((**itr2).isSelectable())
					{
						(**itr2).setSelected(true);
						m_selected_vector.push_back(*itr2);
					}
				}
			}
		}
		
		log ("Rectangular selection of " + Ogre::StringConverter::toString(m_selected_vector.size()));
	}
	else
	{
		int i = 0;
		for (Ogre::SceneQueryResultMovableList::iterator itr = result.movables.begin()
			; itr != result.movables.end(); ++itr
		)
		{
			bool item_is_selected = false;
			for (std::vector<Object *>::iterator itr2 = m_selected_vector.begin()
				; itr2 != m_selected_vector.end(); ++itr2
			)
			{
				Ogre::MovableObject* obj = *itr;
				if ((**itr2).getName() == obj->getName())
				{
					item_is_selected = true;
					(**itr2).setSelected(false);
					m_selected_vector.erase(itr2);
					++i;
					itr2 = m_selected_vector.begin();
					break;
				}
			}
		}
		
		log ("Rectangular unselection of " + Ogre::StringConverter::toString(i));
	}
	
	log ("Num of selected obj: " + Ogre::StringConverter::toString(m_selected_vector.size()));
}
	 
//------------------------------------------------------------------------------

void Desktop::unselectVector()
{
	for (std::vector<Object *>::iterator itr = m_selected_vector.begin(); itr != m_selected_vector.end(); ++itr)
		(*itr)->setSelected(false);
	
	m_selected_vector.clear();
}

//------------------------------------------------------------------------------

void Desktop::unselect (Object * obj)
{
	if (NULL == obj)
		return;
	
	obj->setSelected(false);
	
	for (std::vector<Object *>::iterator itr = m_selected_vector.begin()
		; itr != m_selected_vector.end(); ++itr
	)
		if (*itr == obj)
		{
			m_selected_vector.erase(itr);
			break;
		}
		
}

//------------------------------------------------------------------------------

bool Desktop::onQuit(const CEGUI::EventArgs &e)
{
	m_quit = true;
	return true;
}

//------------------------------------------------------------------------------

bool Desktop::onPopupUnpile(const CEGUI::EventArgs &e)
{
	log ("Unpile operation occured");
	unselectVector();
	std::vector<Object *> objs = mp_popup_asked_pile->getObjects();
	mp_popup_asked_pile->unpile();
	
	for (std::vector<Object *>::iterator itr = objs.begin(); itr != objs.end(); ++itr)
	{
		(**itr).setSelected(true);
		m_selected_vector.push_back(*itr);
	}
	
	for (std::vector<Pile *>::iterator itr = m_piles.begin(); itr != m_piles.end(); ++itr)
		if (*itr == mp_popup_asked_pile)
		{
			m_piles.erase(itr);
			break;
		}
		
	CEGUI::WindowManager::getSingleton().getWindow("Root/Pile/Popup")->hide();
	mp_popup_asked_pile = NULL;
	m_ui_item_selected = true;
	
	log ("Number of selected objects " + Ogre::StringConverter::toString(m_selected_vector.size()));
	return true;
}

//------------------------------------------------------------------------------

bool Desktop::onPopupPile(const CEGUI::EventArgs &e)
{
	log ("Pile operation occured");
	if (m_selected_vector.size() == 0)
		return true;
	
	Pile * p = new Pile();
	m_piles.push_back(p);
	
	for (std::vector<Object *>::iterator itr = m_selected_vector.begin()
		; itr != m_selected_vector.end(); ++itr
	)
		p->insert(*itr);

	unselectVector();
	p->wrapUp();
	CEGUI::WindowManager::getSingleton().getWindow("Root/BatchSelect/Popup")->hide();
	m_ui_item_selected = true;
	return true;
}

//------------------------------------------------------------------------------

bool Desktop::onPopupGrid(const CEGUI::EventArgs &e)
{
	log ("Grid operation occured");
	unselectVector();
	std::vector<Object *> objs = mp_popup_asked_pile->getObjects();
	mp_popup_asked_pile->grid();
	
	for (std::vector<Object *>::iterator itr = objs.begin(); itr != objs.end(); ++itr)
	{
		(**itr).setSelected(true);
		m_selected_vector.push_back(*itr);
	}
	
	for (std::vector<Pile *>::iterator itr = m_piles.begin(); itr != m_piles.end(); ++itr)
		if (*itr == mp_popup_asked_pile)
		{
			m_piles.erase(itr);
			break;
		}
	
	CEGUI::WindowManager::getSingleton().getWindow("Root/Pile/Popup")->hide();
	mp_popup_asked_pile = NULL;
	m_ui_item_selected = true;
	log ("Number of selected objects " + Ogre::StringConverter::toString(m_selected_vector.size()));
	return true;
}


//------------------------------------------------------------------------------

bool Desktop::onPopupFanOut(const CEGUI::EventArgs &e)
{
	log ("Fanout operation occured");
	unselectVector();
	std::vector<Object *> objs = mp_popup_asked_pile->getObjects();
	mp_popup_asked_pile->fanout();

	for (std::vector<Object *>::iterator itr = objs.begin(); itr != objs.end(); ++itr)
	{
		(**itr).setSelected(true);
		m_selected_vector.push_back(*itr);
	}
	
	for (std::vector<Pile *>::iterator itr = m_piles.begin(); itr != m_piles.end(); ++itr)
		if (*itr == mp_popup_asked_pile)
		{
			m_piles.erase(itr);
			break;
		}
		
	CEGUI::WindowManager::getSingleton().getWindow("Root/Pile/Popup")->hide();
	mp_popup_asked_pile = NULL;
	m_ui_item_selected = true;
	log ("Number of selected objects " + Ogre::StringConverter::toString(m_selected_vector.size()));
	return true;
}

//------------------------------------------------------------------------------

CEGUI::MouseButton Desktop::convertMouseButton(OIS::MouseButtonID buttonID)
{
	switch (buttonID)
	{
		case OIS::MB_Left:
			return CEGUI::LeftButton;
			
		case OIS::MB_Right:
			return CEGUI::RightButton;
			
		case OIS::MB_Middle:
			return CEGUI::MiddleButton;
			
		default:
			return CEGUI::LeftButton;
	}
}

//------------------------------------------------------------------------------

void Desktop::log (const Ogre::String & message)
{
	mp_log->logMessage(message);
}

//==============================================================================
	
} // namespace BombasticDesktop

//==============================================================================
