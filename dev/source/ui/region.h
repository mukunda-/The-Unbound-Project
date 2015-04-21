//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace Ui {
	
//-----------------------------------------------------------------------------
enum class Anchor { 
	LEFT, MIDDLE, RIGHT, // left edge, center, right edge

	TOP = LEFT, BOTTOM = RIGHT
};

//-----------------------------------------------------------------------------
struct AnchorSet {
	Anchor from_x; // horizontal point on the region to anchor to the parent
	Anchor to_x;   // horizontal point on the parent to anchor to
	Anchor from_y; // vertical point on the region to anchor to the parent
	Anchor to_y;   // vertical point on the parent to anchor to

	// "parent" meaning "anchor region", and not the "parent region"!
};

//-----------------------------------------------------------------------------
struct Rect {
	Eigen::Vector2i pos;
	Eigen::Vector2i size;
};

/** ---------------------------------------------------------------------------
 * A region is an area on the screen. Regions are invisible by themselves
 * and rely on an implementation to display things.
 */
class Region : public Object {

	friend struct Ui;
protected:
	 
	Rect m_rect;     // area relative to parent 
	Rect m_abs_rect; // computed area in screen space 
	                 // after applying hierarchy
	
	// interecept mouse events
	bool m_clickable;
	bool m_focusable;	 // this object can take input focus by clicking 
	                     // on it or tabbing
	bool m_focused;		 // this object has input focus
	
	bool m_mouseover;	 // mouse is hovering over the object
	
	bool m_pressed;		 // the mouse is pressed down on the region

	// held was set when the user "drags" the region
	// this should be implemented in a derived class that handles the drag events
//	bool m_held;		 // the mouse is holding this object (???)

	// region this is parented to, or "contained in". Parent regions will
	// share certain events with children.
	Region   *m_parent;

	// region this is anchored to, this may or may not be the parent.
	// usually it will match the parent if it's contained in the parent's
	// region, but not if it's anchored to the outside of a region, in which
	// case you probably want the parent to match the anchor's parent.
	Region   *m_anchor;  
 
	// anchor points
	AnchorSet m_anchors; 

	// child regions, this is the parent of them
	std::vector<Region*> m_children;
	
	int  m_sort;            // rendering order
	bool m_sort_relative;   // sort relative to parent, or absolute
	
public:

	Region();

	/** ----------------------------------------------------------------------
	 * Read the location and size of this region, relative to its parent.
	 *
	 * The rect is defined as [x, y, width, height]
	 */
	const Rect &GetRect()    { return m_rect;     }

	/** ----------------------------------------------------------------------
	 * Read the location and size of this region in absolute coordinates.
	 */
	const Rect &GetAbsRect() { return m_abs_rect; }

	void SetParent() {

	}

	/** -----------------------------------------------------------------------
	 * Check if a point lies within this region.
	 *
	 * @param position Position to check.
	 * @returns true if the point is within the region.
	 */
	bool Picked( const Eigen::Vector2i &position );
	
	virtual void OnDraw();
};

}
