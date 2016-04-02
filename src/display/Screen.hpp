/* This file is part of z10-infodisplay, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#ifndef DISPLAY_SCREEN_HPP
#define DISPLAY_SCREEN_HPP

#include <g15daemon_client.h>

#include "Canvas.hpp"

namespace G15{

class Screen{
	private:
		int handle;
		
	public:
		Screen() : handle( new_g15_screen( G15_PIXELBUF ) ) { }
		Screen( const Screen& ) = delete;
		~Screen(){ g15_close_screen( handle ); }
		
		void sendCanvas( Canvas& c )
			{ g15_send( handle, c.canvas, G15_WIDTH*G15_HEIGHT ); }
};

}
#endif
