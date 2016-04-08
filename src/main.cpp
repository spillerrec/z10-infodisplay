/* This file is part of z10-infodisplay, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "display/Screen.hpp"
#include "info/Audio.hpp"

#include <sys/time.h>
#include <unistd.h>

extern "C"{
	#include <glibtop.h>
	#include <glibtop/cpu.h>
	#include <glibtop/close.h>
}

#include <iostream>
#include <algorithm>
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

void drawBar( G15::Canvas& canvas, G15::Point p, bool mirror, bool rotate ){
	auto offset_m = mirror ? 7 : 1;
	auto offset_p = mirror ? 8 : 0;
	
	if( rotate ){
		canvas.line( {p.x+1, p.y+offset_p}, {p.x+7, p.y+offset_p} );
		canvas.line( {p.x+2, p.y+offset_m}, {p.x+6, p.y+offset_m} );
	}
	else{
		canvas.line( {p.x+offset_p, p.y+1}, {p.x+offset_p, p.y+7} );
		canvas.line( {p.x+offset_m, p.y+2}, {p.x+offset_m, p.y+6} );
	}
}

void drawNumber( G15::Canvas& canvas, G15::Point pos, int number ){
	G15::Point offset{ pos.x, pos.y+8 };
	auto leftHigh  = [&](){ drawBar( canvas, pos,    false, false ); };
	auto rightHigh = [&](){ drawBar( canvas, pos,    true , false ); };
	auto leftLow   = [&](){ drawBar( canvas, offset, false, false ); };
	auto rightLow  = [&](){ drawBar( canvas, offset, true , false ); };
	auto top       = [&](){ drawBar( canvas, pos,    false, true  ); };
	auto bottom    = [&](){ drawBar( canvas, offset, true , true  ); };
	auto middle    = [&](){
		drawBar( canvas, pos,    true , true  );
		drawBar( canvas, offset, false, true  );
	};
	
	switch( number ){
		case 0: leftHigh(); rightHigh(); leftLow(); rightLow(); top(); bottom(); break;
		case 1: rightHigh(); rightLow(); break;
		case 2: top(); middle(); bottom(); rightHigh(); leftLow(); break;
		case 3: top(); middle(); bottom(); rightHigh(); rightLow(); break;
		case 4: middle(); leftHigh(); rightHigh(); rightLow(); break;
		case 5: top(); middle(); bottom(); rightLow();  leftHigh(); break;
		case 6: leftHigh(); leftLow(); rightLow(); top(); bottom(); middle(); break;
		case 7: rightHigh(); rightLow(); top(); break;
		case 8: leftHigh(); rightHigh(); leftLow(); rightLow(); top(); bottom(); middle(); break;
		case 9: leftHigh(); rightHigh(); rightLow(); top(); bottom(); middle(); break;
		default: middle(); break;
	}
}

void drawNumberText( G15::Canvas& canvas, G15::Point pos, std::string text ){
	for( auto character : text ){
		drawNumber( canvas, pos, character-'0' );
		pos = {pos.x+12, pos.y};
	}
}

void drawCurrentTime( G15::Canvas& canvas, G15::Point pos ){
	auto t = time( nullptr );
	auto tm = localtime( &t );
	auto number = []( int num ) {
		auto str = std::to_string( num );
		if( str.size() > 1 )
			return str;
		return std::string( "00" ).replace( 2-str.size(), str.size(), str );
	};
	
	drawNumberText( canvas, pos, number( tm->tm_hour ) + ":" + number( tm->tm_min ) + ":" + number( tm->tm_sec ) );
}

int main( int argc, char* argv[] ){
	glibtop_init();
	
	G15::Screen screen;
	G15::Canvas canvas;
	Audio a;
	
	auto cores = 4; //TODO:
	auto width = std::max( 16/cores, 2 );
	auto height = canvas.height-1;
	Stat prev[cores];
	for( int i=0; i<cores; i++ )
		prev[i] = Stat( i );
	
//	for(int j=0; j<10; j++){
	while(true){
		canvas.fill();
		
		canvas.rectangle( {0,0}, cores*width+2, height );
		for( int i=0; i<cores; i++ ){
			Stat cpu( i );
			auto amount = int(cpu.cpuPercentage(prev[i]) * (height-3));
			canvas.rectangleFilled( {2+width*i, height-amount-1}, width-1, amount );
			
			prev[i] = cpu;
		}
		
		drawCurrentTime( canvas, {30,0} );
		
		screen.sendCanvas( canvas );
		a.update();
		
		usleep( 1000000 );
	}
	
	glibtop_close();
	return 0;
}
