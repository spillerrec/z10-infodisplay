
#include "Canvas.hpp"

using namespace G15;

static char doMerge( char value, Color color, Merge merge ){
	auto wanted = static_cast<char>( color );
	switch( merge ){
		case Merge::COPY: return wanted;
		case Merge::AND : return wanted & value;
		case Merge::OR  : return wanted | value;
		case Merge::XOR : return wanted ^ value;
	};
}

static int line_x( int x1, int y1, int dx, int dy, int y ){
	return (int)( (double)( dx * ( y - y1 )) / (double)dy + x1 + 0.5 );
}
static int line_y( int x1, int y1, int dx, int dy, int x ){
	return line_x( y1, x1, dy, dx, x );
}

void Canvas::fill( Color color ){
	for( auto& pixel : canvas )
		pixel = static_cast<char>( color );
}


void Canvas::point( Point position, Color color, Merge merge ){
	(*this)[position] = doMerge( (*this)[position], color, merge );
}

void Canvas::line( Point from, Point to, Color color, Merge merge ){
	int dx = to.x - from.x;
	int dy = to.y - from.y;
	int dir_x = dx > 0 ? 1 : -1;
	int dir_y = dy > 0 ? 1 : -1;
	
	//Start drawing
	point( to, color, merge );	//Always draw the last point
	
	if( from.y == to.y ){	//Horizontal line
		for( int ix = from.x; ix != to.x; ix += dir_x )
			point( {ix, from.y}, color, merge );
	}
	else if( from.x == to.x ){	//Vertical line
		for( int iy = from.y; iy != to.y; iy += dir_y )
			point( {from.x, iy}, color, merge );
	}
	else{	//tilting line
		if( abs(dy) > abs(dx) ){	//iterate over y values
			for( int iy = from.y; iy != to.y; iy+=dir_y )
				point( {line_x( from.x, from.y, dx, dy, iy ), iy}, color, merge );
		}
		else{	//iterate over x values
			for( int ix = from.x; ix != to.x; ix+=dir_x )
				point( {ix, line_y( from.x, from.y, dx, dy, ix )}, color, merge );
		}
	}
}

void Canvas::rectangle( Point top_left, int width, int height, Color color, Merge merge ){
	
}

void Canvas::rectangleFilled( Point top_left, int width, int height, Color color, Merge merge ){
	
}

void Canvas::elipse( Point center, int horizontal_radius, int vertical_radius, Color color, Merge merge ){
	
}

void Canvas::elipseFilled( Point center, int horizontal_radius, int vertical_radius, Color color, Merge merge ){
	
}

void Canvas::text( Point baseline, std::string content, Color color, Merge merge ){
	
}

void Canvas::polygon( std::vector<Point> positions, Color color, Merge merge ){
	
}

void Canvas::polygonFilled( std::vector<Point> positions, Color color, Merge merge ){
	
}
