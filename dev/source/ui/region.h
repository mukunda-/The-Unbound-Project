//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "object.h"

//-----------------------------------------------------------------------------
namespace Ui {
	
//-----------------------------------------------------------------------------
enum class Anchor { 
	LEFT, MIDDLE, RIGHT, // left edge, center, right edge

	TOP = LEFT, BOTTOM = RIGHT,

	NONE = -1
};

 /*
//-----------------------------------------------------------------------------
struct AnchorSet {
	AnchorPoint from_x; // horizontal point on the region to anchor to the parent
	AnchorPoint to_x;   // horizontal point on the parent to anchor to
	AnchorPoint from_y; // vertical point on the region to anchor to the parent
	AnchorPoint to_y;   // vertical point on the parent to anchor to

	// "parent" meaning "anchor region", and not the "parent region"!
};*/

//-----------------------------------------------------------------------------
struct Rect {
	ivec2 pos1; // left, top
	ivec2 pos2; // right, bottom

	// percentages are expanded into integers, 100% = 2^16
};

struct Prect : public Rect {
	fvec2 percent1; // left, top
	fvec2 percent2; // right, bottom

	// abs is computed parent + percent * width + pos
}; 

struct Point {
	int   offset;	// offset in pixels
	float percent;	// offset in percent
	bool  set;		// is set
};

/** ---------------------------------------------------------------------------
 * A region is an area on the screen. Regions are invisible by themselves
 * and rely on an implementation to display things.
 */
class Region : public Object {

	friend class Instance;
protected:

	// pixels are added with percents

	Point m_points[6];       // left,middle,right,top,middle(y),bottom
	Point m_size[2];         // size of the region, if using a fixed size. 

	// if a size is 0, then the size will be determined by the two points
	// if one of the two edge points are not set, then the region is invalid.
	
	Anchor m_anchor_points[6]; // points on the anchor region our points are
	                           // mounted to
	
	ivec4 m_computed_rect;	// computed area in screen space 
	ivec2 m_computed_size;	// (screen, the topmost region)
	 
	bool m_rect_percent[4]; // use percentages instead of offsets.
	
	bool m_clickable;	 // intercept mouse events
	bool m_focusable;	 // this object can take input focus by clicking 
	                     // on it or tabbing
	bool m_focused;		 // this object has input focus
	
	bool m_mouseover;	 // mouse is hovering over the object
	
	bool m_pressed;		 // the mouse is pressed down on the region

	// *** held was set when the user "drags" the region
	// *** this should be implemented in a derived class that handles the drag events
//	bool m_held;		 // the mouse is holding this object (???)

	// region this is parented to, or "contained in". Parent regions will
	// share certain events with children.
	Region *m_parent;

	// region this is anchored to, if null, then the parent is used
	//
	// this is separate from parent in the case that you want to mount 
	// a region to the outside of another, where you probably 
	// want the parent to match the anchor's parent.
	Region *m_anchor_to;

	// list of regions that are anchored to this one and need to be
	// refreshed when this one is adjusted.
	std::vector<Region*> m_anchor_list;

	// regions this is the parent of
	std::vector<Region*> m_children;
	
	int  m_sort;            // rendering order
	bool m_sort_relative;   // sort relative to parent, or absolute
	
public:

	Region();

	/** ----------------------------------------------------------------------
	 * Read the location and/or size of this region, relative to its anchor.
	 *
	 * The vector contains left,top,right,bottom.
	 */
	const ivec4 &GetRect() const { return m_computed_rect; }
	ivec2 GetTopLeft()     const { return ivec2( m_computed_rect[0], m_computed_rect[1] ); }
	ivec2 GetTopRight()    const { return ivec2( m_computed_rect[2], m_computed_rect[1] ); }
	ivec2 GetBottomLeft()  const { return ivec2( m_computed_rect[0], m_computed_rect[3] ); }
	ivec2 GetBottomRight() const { return ivec2( m_computed_rect[2], m_computed_rect[3] ); }
	const ivec2 GetSize()  const { return m_computed_size; }

	/** ----------------------------------------------------------------------
	 * Set the position of the region, relative to its anchor.
	 */
	void SetPoint( Anchor horizontal, Anchor vertical, Anchor ivec2 position );
	void SetSize( ivec2 size );

	/** ----------------------------------------------------------------------
	 * Read the location and size of this region in absolute coordinates.
	 */
	const Rect &GetAbsRect() { return m_abs_rect; }
	const ivec2 &GetAbsPos() { return m_abs_rect.pos; }

	/** -----------------------------------------------------------------------
	 * Set the parent of this region.
	 */
	void SetParent( Region &parent );
	void ClearParent();

	void SetAnchorRegion( Region &anchor );
	void SetAnchorPoints( Anchor 

	/** -----------------------------------------------------------------------
	 * Check if a point lies within this region.
	 *
	 * @param position Position to check.
	 * @returns true if the point is within the region.
	 */
	bool Inside( const ivec2 &position );
	
	virtual void OnDraw();
};

}

