/* This file is part of z10-infodisplay, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#ifndef INFO_AUDIO_HPP
#define INFO_AUDIO_HPP

#include "pulse++.hpp"

#include <iostream>

struct CallbackTest{
	int stuff;
	
	void stateCallback( pulse::Context context );
	void sinkInfoListCallback( pulse::Context, const pa_sink_info *i, int eol );
};

class Audio{
	private:
		pulse::Mainloop loop;
		pulse::Connection connection;
		int level;
		CallbackTest callback;
		
	public:
		Audio();
		
		void update();
	
};

#endif
