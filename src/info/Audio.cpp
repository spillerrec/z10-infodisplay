/* This file is part of z10-infodisplay, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "Audio.hpp"

void CallbackTest::stateCallback( pulse::Context context ){
	std::cout << "Something happened!\n";
	//std::cout << text << std::endl;
	
	context.setSinkInfoListCallback( *this );
}

void CallbackTest::sinkInfoListCallback( pulse::Context, const pa_sink_info *i, int eol ){
	
	std::cout << eol << std::endl;
	if( i ){
		auto volume = i->volume;
		uint64_t avg = volume.values[0];
		for( int i=1; i<volume.channels; i++ )
			avg += volume.values[i];
		avg /= volume.channels;
		std::cout << i->name << std::endl;
		std::cout << "Volume: " << double(avg) / PA_VOLUME_NORM * 100 << std::endl;
	}
}

Audio::Audio() : connection( loop, "z10-infodisplay" ) {
	connection.setStateCallback( callback );
	connection.connect( nullptr, PA_CONTEXT_NOAUTOSPAWN );
}

void Audio::update(){
	int block=0, retval;
	if( loop.iterate( block, retval ) < 0 )
		;//TODO: on quit
}
