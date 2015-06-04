//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Ui { namespace Render {

/** ---------------------------------------------------------------------------
 * Render text onto the screen.
 *
 * @param font   Font to use.
 * @param sort   Z order.
 * @param height Height of font to use.
 * @param x      X pixel offset.
 * @param y      Y pixel offset.
 * @param text   Text to render.
 */
void Text( Graphics::FontMaterial &font, int sort, int height, 
	       int x, int y, const Stref &text );

}}
