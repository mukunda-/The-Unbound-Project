//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace Ui {
	
//-----------------------------------------------------------------------------
enum class Anchor { 
	LEFT,MIDDLE,RIGHT,
	TOP=LEFT,BOTTOM=RIGHT
};

//-----------------------------------------------------------------------------
struct AnchorSet {
	Anchor from_x;
	Anchor to_x;
	Anchor from_y;
	Anchor to_y;
};

//-----------------------------------------------------------------------------
class Region : public Object {

	friend struct Ui;
protected:
	 
	Eigen::Vector4i m_rect;     // location relative to parent, and size
	Eigen::Vector4i m_abs_rect; // computed area in screen space 
	                            // after applying hierarchy
	
	bool m_active;			// should appear on screen and accept events
	bool m_held;			// the mouse is holding this object
	bool m_mouseover;		// mouse is hovering over the object

	bool m_focusable;		// this object can take input focus by clicking on it or tabbing
	bool m_focused;			// this object has input focus
	
	bool m_pressed;			// the mouse is pressed and is in the object area
	 
	Region   *m_parent;	 // region this is parented to.
	Region   *m_anchor;  // region this is anchored to.
	AnchorSet m_anchors; // anchor point with anchor region

	// child regions, this is the parent of them
	std::vector<Region*> m_children; 
	
	//int m_z_order;		// ordering of Widgets with their siblings TODO
							// negative values will appear behind the parent.

	EventHandler m_event_handler;

public:

	Region();

	bool Picked( const Eigen::Vector2i &position );
	 
	// handle SDL event
	bool FireEvent( const Event &event );

	// find an Widget at this point
	//Widget *PickWidget( const Eigen::Vector2i &position );

	virtual void OnDraw();
};

}
