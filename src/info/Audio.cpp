/* This file is part of z10-infodisplay, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "Audio.hpp"


Audio::Audio() : connection( loop, "z10-infodisplay" ) {
	connection.setStateCallback( callback );
	connection.connect( nullptr, PA_CONTEXT_NOAUTOSPAWN );
}

void Audio::update(){
	int block=0, retval;
	if( loop.iterate( block, retval ) < 0 )
		;//TODO: on quit
}
