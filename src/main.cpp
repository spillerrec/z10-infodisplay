/* This file is part of z10-infodisplay, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "display/Screen.hpp"

#include <unistd.h>

extern "C"{
	#include <glibtop.h>
	#include <glibtop/cpu.h>
	#include <glibtop/close.h>
}

#include <iostream>
using namespace std;


struct Stat{
	//http://stackoverflow.com/a/23376195
	guint64 non_idle{ 0 };
	guint64 total{ 0 };
	
	Stat() = default;
	
	Stat( glibtop_cpu info, int id )
		:	non_idle( info.xcpu_user[id] + info.xcpu_nice[id] + info.xcpu_sys[id] + info.xcpu_irq[id] + info.xcpu_softirq[id] )
		,	total( info.xcpu_idle[id] + info.xcpu_iowait[id] + non_idle )
		{ }
	
	auto cpuPercentage( Stat prev ) const
		{ return (non_idle - prev.non_idle) / double(total - prev.total);  }
};

int main( int argc, char* argv[] ){
	glibtop_init();
	
	G15::Screen screen;
	G15::Canvas canvas;
	
	auto cores = 4; //TODO:
	Stat prev[cores];
	for(int j=0; j<20; j++){
		canvas.fill();
		
		canvas.rectangle( {0,0}, 102, cores*5+2 );
		for( int i=0; i<cores; i++ ){
			glibtop_cpu info;
			glibtop_get_cpu( &info );
			
			Stat cpu( info, i );
			canvas.rectangleFilled( {2, 2+5*i}, cpu.cpuPercentage(prev[i])*100, 4 );
			prev[i] = cpu;
		}
		
		screen.sendCanvas( canvas );
		
		usleep( 500000 );
	}
	
	glibtop_close();
	return 0;
}
