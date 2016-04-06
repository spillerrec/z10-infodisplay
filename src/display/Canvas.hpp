/* This file is part of z10-infodisplay, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#ifndef DISPLAY_CANVAS_HPP
#define DISPLAY_CANVAS_HPP

#include <g15daemon_client.h>

#include <string>
#include <vector>
#include <cassert>

namespace G15{

constexpr char BLACK = 1;
constexpr char WHITE = 0;

enum class Merge{
	COPY,
	AND,
	OR,
	XOR
};

template<int MAX, int MIN=0>
struct Limited{
	int val;
	
	void validate()
#ifdef NDEBUG
		{}
#else
		{ assert( val >=0 && val < MAX ); }
#endif
	
	Limited( int value ) : val(value) { validate(); }
	
	operator int() const{ return val; }
	Limited& operator=( const Limited& other ){
		val = other.val;
		return *this;
	}
	Limited& operator=( int value ) { return *this = Limited( value ); }
};

class Point{
	public:
		Limited<G15_WIDTH > x;
		Limited<G15_HEIGHT> y;
		
	public:
		Point( int x, int y ) : x(x), y(y) {}
};

enum class Color: char{
	WHITE = 0,
	BLACK = 1
};

class Canvas{
	public:
		char canvas[G15_WIDTH*G15_HEIGHT];
		
	public:
		auto operator[](int iy){ return canvas + G15_WIDTH*iy; }
		auto& operator[](Point pos){ return (*this)[pos.y][pos.x]; }
		
		static constexpr auto width  = G15_WIDTH;
		static constexpr auto height = G15_HEIGHT;
		
		void fill( Color color=Color::WHITE );
		void point( Point position, Color color=Color::BLACK, Merge merge=Merge::COPY );
		void line( Point from, Point to, Color color=Color::BLACK, Merge merge=Merge::COPY );
		void rectangle( Point top_left, int width, int height, Color color=Color::BLACK, Merge merge=Merge::COPY );
		void rectangleFilled( Point top_left, int width, int height, Color color=Color::BLACK, Merge merge=Merge::COPY );
		void elipse( Point center, int horizontal_radius, int vertical_radius, Color color=Color::BLACK, Merge merge=Merge::COPY );
		void elipseFilled( Point center, int horizontal_radius, int vertical_radius, Color color=Color::BLACK, Merge merge=Merge::COPY );
		void text( Point baseline, std::string content, Color color=Color::BLACK, Merge merge=Merge::COPY );
		void polygon( std::vector<Point> positions, Color color=Color::BLACK, Merge merge=Merge::COPY );
		void polygonFilled( std::vector<Point> positions, Color color=Color::BLACK, Merge merge=Merge::COPY );
		
};


}
#endif
