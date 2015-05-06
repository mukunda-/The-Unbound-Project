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

//-----------------------------------------------------------------------------
enum class Strata {
	INHERIT,         // inherit from parent

	BACKGROUND,      // for background art, etc

	BEHIND,          // allow normal elements to overlap
	NORMAL,          // for most elements
	FRONT,           // for elements that overlap normal
	                 // elements

	DIALOG_BEHIND,   // used for window decorations ?
	DIALOG,          // used for windows
	DIALOG_FRONT,    // used for 
	
	EFFECTS1,        // screen effects

	OVERLAY,         // screen overlays
	OVERLAY_FRONT,   // in front of screen overlays

	EFFECTS2,        // more screen effects

	TOOLTIP,         // mouse tooltips

	EFFECTS3,        // mega screen effects, should be potentially avoided.

	// computed strata is determined as follows:
	// `strata` * 65536 + `level` * 16 + `offset`

	// strata may either be inherited from a parent or redefined
	// when redefined, the `level` is reset to zero
	// each time the strata is inherited by a child, `level` is incremented.
	//
	// `offset` is a final adjustment specified by the user, offset may
	// be negative to allow positioning behind a parent without changing
	// the strata
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

/** ---------------------------------------------------------------------------
 * A region is an area on the screen. Regions are invisible by themselves
 * and rely on an implementation to display things.
 */
class Region : public Object {

	friend class Instance;
private:
	 
	struct Point {
		int   offset  = 0;		// offset in pixels
		float percent = 0.0f;	// offset in percent
		bool  set     = false;	// is set
	};

	// pixels are added with percents

	Point m_points[6];       // left,middle,right,top,middle(y),bottom
	Point m_size[2];         // size of the region

	// if a size is 0, then the size will be determined by the two points
	// if one of the two edge points aren't set, then the region is invalid.
	
	Anchor m_anchor_points[6]; // points on the anchor region our points are
	                           // mounted to
	
	//-------------------------------------------------------------------------
	// strata for this region
	Strata m_strata         = Strata::INHERIT;

	// adjustment to sorting order
	int    m_strata_offset  = 0;

	//-------------------------------------------------------------------------
	ivec4 m_computed_rect;	 // computed area in screen space 
	ivec2 m_computed_size;	 // (screen, the topmost region)

	int   m_computed_strata; // computed rendering order

	//------------------------------------------------------------------------- 
	bool m_clickable  = false; // intercept mouse events
	bool m_focusable  = false; // this object can take input focus by clicking 
	                           // on it or tabbing

	bool m_focused    = false; // this object has input focus
	
	bool m_mouseover  = false; // mouse is hovering over the object
	
	bool m_pressed    = false; // the mouse is pressed down on the region

	// *** held was set when the user "drags" the region
	// *** this should be implemented in a derived class that handles the drag events
//	bool m_held;		 // the mouse is holding this object (???)

	//-------------------------------------------------------------------------
	// region this is parented to, or "contained in". Parent regions will
	// share certain events with children.
	Region *m_parent = nullptr;

	// region this is anchored to, if null, then the parent is used
	//
	// this is separate from parent in the case that you want to mount 
	// a region to the outside of another, where you probably 
	// want the parent to match the anchor's parent.
	Region *m_anchor_to = nullptr;

	// list of regions that are anchored to this one and need to be
	// refreshed when this one is adjusted.
	std::vector<Region*> m_anchor_list;

	// regions this is the parent of
	std::vector<Region*> m_children;
	
public:

	Region();

	/** ----------------------------------------------------------------------
	 * Get the computed area of this region.
	 *
	 * @returns vector containing "left, top, right, bottom" in pixels.
	 *
	 * left, top are inclusive; right, bottom are exclusive
	 */
	const ivec4 &GetRect() const { return m_computed_rect; }

	/** ----------------------------------------------------------------------
	 * Get the position of a corner point.
	 *
	 * bottom/right points are exclusive.
	 */
	ivec2 GetTopLeft()     const { return ivec2( m_computed_rect[0], m_computed_rect[1] ); }
	ivec2 GetTopRight()    const { return ivec2( m_computed_rect[2], m_computed_rect[1] ); }
	ivec2 GetBottomLeft()  const { return ivec2( m_computed_rect[0], m_computed_rect[3] ); }
	ivec2 GetBottomRight() const { return ivec2( m_computed_rect[2], m_computed_rect[3] ); }

	/** ----------------------------------------------------------------------
	 * Get the computed size.
	 */
	const ivec2 GetSize() const { return m_computed_size; }
	
	/** ----------------------------------------------------------------------
	 * Get the computed rendering order.
	 */
	int GetComputedStrata() const { return m_computed_strata; }

	/** ----------------------------------------------------------------------
	 * Setup the points of this region using pixel offsets.
	 *
	 * If you use a center anchor, the edge anchors of that dimension will
	 * be `unset`, and vice versa.
	 *
	 * @param horiz_from The horizontal point on the region to set.
	 * @param horiz_to   The horizontal point on the anchor region to 
	 *                   mount to.
	 * @param vert_from/to The vertical point to mount.
	 *
	 * @param horiz_offset   Horizontal offset in pixel units.
	 * @param horiz_offset_p Horizontal offset in percent units.
	 *
	 * @param vert_offset,vert_offset_p Vertical offset.
	 */
	void SetPoints( Anchor horiz_from, Anchor horiz_to,
		            Anchor vert_from,  Anchor vert_to, 
				    int horiz_offset,  int vert_offset );
	
	void SetHorizontalPoint( Anchor from, Anchor to, int offset );
	void SetVerticalPoint  ( Anchor from, Anchor to, int offset );

	/** ----------------------------------------------------------------------
	 * Setup the points of this region using percent offsets.
	 *
	 * Percentage offsets are added to the pixel offset.
	 */
	void SetPointsPercent( Anchor horiz_from,  Anchor horiz_to,
		                   Anchor vert_from,   Anchor vert_to, 
				           float horiz_offset, float vert_offset );
	void SetHorizontalPointPercent( Anchor from, Anchor to, float offset );
	void SetVerticalPointPercent  ( Anchor from, Anchor to, float offset );

	/** ----------------------------------------------------------------------
	 * Unset points.
	 *
	 * When points are unset, they are computed using the other data present.
	 * e.g when you have the `bottom` and `left` points defined, then the
	 * `right` and `top` will be determined by the size.
	 */
	void UnsetPoints( Anchor horizontal, Anchor vertical );
	void UnsetHorizontalPoint( Anchor point );
	void UnsetVerticalPoint( Anchor point );
	
	/** ----------------------------------------------------------------------
	 * Percent units equal 1% of the width or height of the anchor region.
	 *
	 * Percent of width is used for horizontal related points, 
	 * percent of height is used for vertical related points.
	 *
	 * For example, with an offset of 5 and a percent offset of 5.5, the
	 * computed offset of a horizontal point will be:
	 *                    "5 + 5.5/100 * anchor.width"
	 */
	
	
	/** ----------------------------------------------------------------------
	 * Set all points to cover the anchor region.
	 *
	 * @param margin   Pixel margin to leave on the anchor. Positive amounts
	 *                 will make this region smaller than the anchor region.
	 *                 Negative amounts will make this region bigger than the
	 *                 anchor region.
	 *
	 * @param margin_p Percent margin to leave on the anchor. 
	 */
	void SetAllPoints( int margin = 0, float margin_p = 0.0f );

	/** ----------------------------------------------------------------------
	 * Set the size of the region in pixels.
	 *
	 * If a dimension size is set to zero, then the points will be used to 
	 * determine the size. Note that the percentage size is added to this
	 * size, and both must be 0 to be sized by points.
	 *
	 * Size for a dimension will be ignored if both edge points are set.
	 *
	 * @param width, height Size of region in pixels.
	 */
	void SetSize( int width, int height );
	
	/** ----------------------------------------------------------------------
	 * Set the size of the region in percent. 
	 *
	 * Percent size is added to pixel size.
	 */
	void SetSizePercent( float width, float height );

	void SetWidth( int width );
	void SetWidthPercent( float width );

	void SetHeight( int height );
	void SetHeightPercent( float height );

	/** -----------------------------------------------------------------------
	 * Set the parent of this region. If the parent is not set, the screen
	 * will be used as the parent.
	 */
	void SetParent( Region &parent );
	void ClearParent();
	
	/** -----------------------------------------------------------------------
	 * Set the anchor region. If the anchor region is not set, the parent will
	 * be used for anchor operations.
	 */
	void SetAnchor( Region &anchor );
	void ClearAnchor();

	/** -----------------------------------------------------------------------
	 * Check if a point lies within this region.
	 *
	 * @param position Position to check.
	 * @returns true if the point is within the region.
	 */
	bool Inside( const ivec2 &position ) {
		return position[0] >= m_computed_rect[0] 
			&& position[1] >= m_computed_rect[1] 
			&& position[0]  < m_computed_rect[2]
			&& position[1]  < m_computed_rect[3];
	}
	
	virtual void OnDraw();
};

}

