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
	guint64 total  { 0 };
	guint64 user   { 0 };
	guint64 nice   { 0 };
	guint64 sys    { 0 };
	guint64 idle   { 0 };
	guint64 iowait { 0 };
	guint64 irq    { 0 };
	guint64 softirq{ 0 };
	
	Stat() = default;
	Stat( glibtop_cpu info )
		:	total(info.total), user(info.user), nice(info.user), sys(info.sys), idle(info.idle)
		,	iowait(info.iowait), irq(info.irq), softirq(info.softirq)
		{ }
	
	Stat( glibtop_cpu info, int id )
		:	total(info.xcpu_total[id]), user(info.xcpu_user[id]), nice(info.xcpu_user[id])
		,	sys(info.xcpu_sys[id]), idle(info.xcpu_idle[id]) , iowait(info.xcpu_iowait[id])
		,	irq(info.xcpu_irq[id]), softirq(info.xcpu_softirq[id])
		{ }
	
	//http://stackoverflow.com/a/23376195
	auto idle2()   const { return idle + iowait; }
	auto nonIdle() const { return user + nice + sys + irq + softirq; }
	auto total2()  const { return idle2() + nonIdle(); }
	
	auto cpuPercentage( Stat prev ) const
		{ return (nonIdle() - prev.nonIdle()) / double(total2() - prev.total2());  }
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
