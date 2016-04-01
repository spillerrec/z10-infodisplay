/* This file is part of z10-infodisplay, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */


#include <g15daemon_client.h>
#include <unistd.h>

extern "C"{
	#include <glibtop.h>
	#include <glibtop/cpu.h>
	#include <glibtop/close.h>
}

#include <iostream>
using namespace std;

namespace G15{
	
	constexpr char BLACK = 1;
	constexpr char WHITE = 0;

class Canvas{
	public:
		char canvas[G15_WIDTH*G15_HEIGHT];
		
	public:
		auto operator[](int iy){ return canvas + G15_WIDTH*iy; }
		
		static constexpr auto width  = G15_WIDTH;
		static constexpr auto height = G15_HEIGHT;
		
		void lineHor( int y, int x, int width, char color=BLACK ){
			for( int i=0; i<width; i++ )
				(*this)[y][x+i] = color;
		}
		void rectangle( int y, int x, int height, int width, char color=BLACK ){
			for( int i=0; i<height; i++ )
				lineHor( y+i, x, width, color );
		}
		
		void fill( char color=WHITE ){
			for( auto& pix : canvas )
				pix = color;
		}
};

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
		
		for( int i=0; i<cores; i++ ){
			glibtop_cpu info;
			glibtop_get_cpu( &info );
			
			Stat cpu( info, i );
			canvas.rectangle( 10+5*i, 0, 5, int(cpu.cpuPercentage(prev[i])*100) );
			prev[i] = cpu;
		}
		
		screen.sendCanvas( canvas );
		
		usleep( 500000 );
	}
	
	glibtop_close();
	return 0;
}
