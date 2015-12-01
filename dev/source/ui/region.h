//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "object.h"

//-----------------------------------------------------------------------------
namespace Ui {
	
//-----------------------------------------------------------------------------
enum class Anchor { 
	LEFT = 0, MIDDLE = 1, RIGHT = 2, // left edge, center, right edge

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
	// `strata` * 10000 + `level` * 100 + `offset`

	// strata may either be inherited from a parent or redefined
	// when redefined, the `level` is reset to zero
	// each time the strata is inherited by a child, `level` is incremented.
	//
	// `offset` is a final adjustment specified by the user, offset may
	// be negative to allow positioning behind a parent without changing
	// the strata
};
  
/** ---------------------------------------------------------------------------
 * A region is an area on the screen. Regions are invisible by themselves
 * and rely on an implementation to display things.
 */
class Region : public Object {

	friend class Instance;
private:
	 
	struct Point {
		int   offset  = 0;		// offset in pixels
		float percent = 0.0f;	// offset in percent, 
		                        // actually stored as 100% = 1.0
		bool  set     = false;	// is set

		void Set( int o ) {
			offset = o;
			set = true;
		}

		void SetPercent( float p ) {
			percent = p;
			set = true;
		}

		void Unset() {
			set = false;
		}
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
	ivec4 m_computed_rect;	     // computed area in screen space 
	ivec2 m_computed_size;       // (screen, the topmost region)
	bool  m_computed_valid;      // if the computed area is valid.

	int   m_computed_strata = 0; // computed rendering order

	//------------------------------------------------------------------------- 
	bool m_clickable  = false; // intercept mouse events
	bool m_focusable  = false; // this object can take input focus by clicking 
	                           // on it or tabbing

	bool m_focused    = false; // this object has input focus
	
	bool m_mouseover_last = false; // last mouseover state, used during
	                               // movement processing

	bool m_mouseover  = false; // mouse is over the region
	
	bool m_pressed    = false; // the mouse is pressed down on the region
	 
	//-------------------------------------------------------------------------
	// region this is parented to, or "contained in". Parent regions will
	// share certain events with children.
	Region *m_parent = nullptr;

	// region this is anchored to, will never be null
	//
	// this is separate from parent in the case that you want to mount 
	// a region to the outside of another, where you probably 
	// want the parent to match the anchor's parent.
	//
	// anchor_to_parent means the anchor will be updated if the parent
	// is changed.
	Region *m_anchor_to        = nullptr;
	bool    m_anchor_to_parent = false;

	// list of regions that are anchored to this one and need to be
	// refreshed when this one is adjusted.
	std::vector<Region*> m_anchor_list;

	// regions this is the parent of
	std::vector<Region*> m_children;

	//-------------------------------------------------------------------------
	void SetPointsThing( Anchor from, Anchor to, int index, 
		                 bool set_offset, int offset, 
						 bool set_percent, float percent );

	void ComputeWith( Region &anchor );
	static int GetAnchorPos( Region &anchor, Anchor pos, int set );
	static int ComputePoint( Region &anchor, Anchor pos, 
		                     int set, Point point );
	int ComputeSize( Region &anchor, int set );
	void ComputeDimension( Region &anchor, int set );

	void SetupScreen( int width, int height );

	void AddChild( Region &child );
	void RemoveChild( Region &child );

	void AddAnchored( Region &region );
	void RemoveAnchored( Region &region );
	
protected:

	Region( const Stref &name );

	/** ----------------------------------------------------------------------
	 * Render this region.
	 */
	virtual void Draw();

public:
	~Region();

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
	 * e.g. when you have the `bottom` and `left` points defined, then the
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

	void SetWidth        ( int   pixels  );
	void SetWidthPercent ( float percent );
	void SetHeight       ( int   pixels  );
	void SetHeightPercent( float percent );

	void UnsetSize();
	void UnsetWidth();
	void UnsetHeight();

	/** -----------------------------------------------------------------------
	 * Set the strata for this region.
	 *
	 * @param strata See Strata enum.
	 * @param offset The offset added to the computed strata.
	 */
	void SetStrata      ( Strata strata );
	void SetStrataOffset( int offset    );

	Strata GetStrata();
	int    GetStrataOffset();
	

	/** -----------------------------------------------------------------------
	 * Set the parent of this region. 
	 *
	 * @param parent  Pointer to parent, or nullptr to use the screen.
	 * @param compute Recompute the region area.
	 */
	void SetParent( Region *parent, bool compute = true ); 
	
	/** -----------------------------------------------------------------------
	 * Set the anchor region.
	 *
	 * @param anchor Anchor region. may be nullptr to reference the screen.
	 * @param compute Recompute the region area.
	 */
	void SetAnchor( Region *anchor, bool compute = true );

	/** -----------------------------------------------------------------------
	 * Clear the anchor region.
	 *
	 * This makes the region use the parent as the anchor.
	 * 
	 * @param compute Recompute the region area.
	 */
	void ClearAnchor( bool compute = true );

	/** -----------------------------------------------------------------------
	 * Set if this region may be focused for input.
	 */
	void SetFocusable( bool focusable );
	bool IsFocusable() const { return m_focusable; }

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

	/** -----------------------------------------------------------------------
	 * Recompute the region area.
	 *
	 * Normally not needed to be called directly.
	 */
	void Compute();

	/** -----------------------------------------------------------------------
	 * Returns true if the computed area is valid.
	 */
	bool Valid() { return m_computed_valid; }
};

}

