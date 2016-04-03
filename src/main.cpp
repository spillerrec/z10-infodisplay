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
	Stat( int id ){
		glibtop_cpu info;
		glibtop_get_cpu( &info );
		*this = Stat( info, id );
	}
	
	auto cpuPercentage( Stat prev ) const{
		auto total_diff = total - prev.total;
		return total_diff == 0 ? 0.0 : (non_idle - prev.non_idle) / double(total_diff);
	}
};

int main( int argc, char* argv[] ){
	glibtop_init();
	
	G15::Screen screen;
	G15::Canvas canvas;
	
	auto cores = 4; //TODO:
	auto width = std::max( 16/cores, 2 );
	auto height = canvas.height-1;
	Stat prev[cores];
	for( int i=0; i<cores; i++ )
		prev[i] = Stat( i );
	
	for(int j=0; j<10; j++){
		canvas.fill();
		
		canvas.rectangle( {0,0}, cores*width+2, height );
		for( int i=0; i<cores; i++ ){
			Stat cpu( i );
			auto amount = int(cpu.cpuPercentage(prev[i]) * (height-3));
			canvas.rectangleFilled( {2+width*i, height-amount-1}, width-1, amount );
			
			prev[i] = cpu;
		}
		
		screen.sendCanvas( canvas );
		
		usleep( 500000 );
	}
	
	glibtop_close();
	return 0;
}
