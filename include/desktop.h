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

#ifndef __DESKTOP_H__
#define __DESKTOP_H__

//==============================================================================

#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
#include <OgreBulletCollisions.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>

//==============================================================================

namespace OgreBulletDynamics { class DynamicsWorld; }

//==============================================================================

namespace BombasticDesktop {

//==============================================================================

	class Object;
	class Pile;
	
//==============================================================================

class Desktop : public Ogre::FrameListener
	, public Ogre::WindowEventListener
	, public OIS::KeyListener
	, public OIS::MouseListener
{
public:
	Desktop();
	~Desktop();
	void go();
	bool setup();

	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool frameEnded(const Ogre::FrameEvent& evt);
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);		
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	static const Ogre::String msc_scene_name;
	static const Ogre::String msc_camera_name;
	static const Ogre::String msc_window_title;
	static const Ogre::Radian msc_camera_start_pitch;
	static const Ogre::Radian msc_camera_min_pitch;
	static const Ogre::Radian msc_camera_max_pitch;
	static const Ogre::Vector3 msc_camera_target_start_pos;
	static const Ogre::ColourValue msc_background_color;
	static const Ogre::ColourValue msc_ambient_light;
	static const Ogre::Real msc_mouse_rotate_speed;
	static const Ogre::Real msc_mouse_zoom_speed;
	static const Ogre::Real msc_mouse_start_zoom;
	static const Ogre::Real msc_mouse_min_zoom;
	static const Ogre::Real msc_mouse_max_zoom;
	static const Ogre::Real msc_room_width;
	static const Ogre::Real msc_room_height;
	static const Ogre::Real msc_room_depth;
	static const Ogre::Real msc_floor_restitution;
	static const Ogre::Real msc_floor_friction;
	static const Ogre::Real msc_scale_xz_factor;
	static const Ogre::Real msc_max_scale_xz;
	static const Ogre::Real msc_min_scale_xz;
	static const Ogre::Real msc_scale_y_factor;
	static const Ogre::Real msc_max_scale_y;
	static const Ogre::Real msc_min_scale_y;
	static const Ogre::Real msc_item_translation_height;

private:
	void locateResources();
	void setupInput();
	void setupCamera();
	void setupLights();
	void setupEntities();
	void setupBullet();
	void setupCEGUI();
	void setupScene();
	void shutdownBullet();
	void shutdown();
	void shutdownInput();
	void selectObjectsInRectangle(
		const Ogre::Real & x1, const Ogre::Real & y1
		, const Ogre::Real & x2, const Ogre::Real & y2
	);
	void unselectVector();
	void unselect (Object * obj);
	
	bool onQuit(const CEGUI::EventArgs &e);
	bool onPopupUnpile(const CEGUI::EventArgs &e);
	bool onPopupPile(const CEGUI::EventArgs &e);
	bool onPopupGrid(const CEGUI::EventArgs &e);
	bool onPopupFanOut(const CEGUI::EventArgs &e);
	CEGUI::MouseButton convertMouseButton(OIS::MouseButtonID buttonID);
	
	void log (const Ogre::String & message);
	
	Ogre::Root* mp_root;
	Ogre::RenderWindow* mp_renderwindow;
	Ogre::Viewport* mp_viewport;
	Ogre::SceneManager* mp_scene_manager;
	Ogre::Camera* mp_camera;
	OIS::InputManager* mp_input_mgr;
	OIS::Keyboard* mp_keyboard;
	OIS::Mouse* mp_mouse;
	Ogre::Log * mp_log;
	bool m_quit;
	Ogre::RaySceneQuery *mp_ray_scene_query;
	Ogre::PlaneBoundedVolumeListSceneQuery *mp_plane_bounded_vol_list_scene_query;
	Ogre::SceneNode * mp_nd_cam_target_yaw;
	Ogre::SceneNode * mp_nd_cam_target_pitch;
	Ogre::SceneNode * mp_nd_camera;
	
	OgreBulletDynamics::DynamicsWorld * mp_world;
	std::deque<Object *> m_objects;
	Object * mp_selected;
	Pile * mp_popup_asked_pile;
	Pile * mp_selected_pile;
	std::vector<Object *> m_selected_vector;
	Ogre::Vector3 m_gravity_vector;
	Ogre::AxisAlignedBox m_bounds;

	Ogre::Real m_movement_force;
	
#if defined(BULLET_DEBUT)
	OgreBulletCollisions::DebugDrawer * mp_debug_drawer;
#endif
	
	OIS::MouseState m_last_mouse_move_state;
	OIS::MouseState m_last_mouse_left_press_state;
	Ogre::Real m_mouse_move_treshold;
	Object * mp_last_selected;
	Ogre::OverlayContainer * mp_rectangle_overlay;
	
	bool m_scale_xz;
	Ogre::Real m_scale_xz_factor;
	bool m_scale_y;
	Ogre::Real m_scale_y_factor;
	
	bool m_batch_selecting;
	
	CEGUI::OgreRenderer* mp_cegui_renderer;
	bool m_ui_item_selected;
	bool m_ui_enabled;
	
	std::vector<Pile *> m_piles;
	std::map<Ogre::String, Ogre::Entity *> m_cubes;
};

//==============================================================================

} // namespace BombasticDesktop

//==============================================================================

#endif // __DESKTOP_H__

//==============================================================================
