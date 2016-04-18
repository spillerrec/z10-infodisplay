/* This file is part of z10-infodisplay, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "Canvas.hpp"

#include <cmath>

using namespace G15;

static char doMerge( char value, Color color, Merge merge ){
	auto wanted = static_cast<char>( color );
	switch( merge ){
		case Merge::COPY: return wanted;
		case Merge::AND : return wanted & value;
		case Merge::OR  : return wanted | value;
		case Merge::XOR : return wanted ^ value;
		default: exit(-1);
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
	int x = top_left.x, y = top_left.y;
	if( height > 1 && width > 1 ){ //Avoid the case were some of the lines would overlap
		line( {x      , y       },  {x+width, y         }, color, merge );
		line( {x      , y+height},  {x+width, y+height  }, color, merge );
		
		line( {x      , y+1     },  {x      , y+height-1}, color, merge );
		line( {x+width, y+1     },  {x+width, y+height-1}, color, merge );
	}
	else //Draw a filled rectangle as it doesn't contain any whitespace
		rectangleFilled( top_left, width, height, color, merge );
}

void Canvas::rectangleFilled( Point top_left, int width, int height, Color color, Merge merge ){
	for( int iy=0; iy<height; iy++ )
		line( {top_left.x, iy+top_left.y}, {top_left.x+width-1, iy+top_left.y}, color, merge );
}

void Canvas::elipse( Point center, int horizontal_radius, int vertical_radius, Color color, Merge merge ){
	
	if( horizontal_radius == 0 ){
		if( vertical_radius == 0 )
			point( center, color, merge );
		else
			line( {center.x, center.y-vertical_radius}, {center.x, center.y+vertical_radius}, color, merge );
	}
	else if( vertical_radius == 0 ){
		line( {center.x-horizontal_radius, center.y}, {center.x+horizontal_radius, center.y}, color, merge );
	}
	else{
		//Find the point (P) where the tangent is at a 45 deg angle
		double v = atan( (double)vertical_radius / horizontal_radius );
		int Px = horizontal_radius * cos( v ) + 0.5;
		int Py_start = vertical_radius * sin( acos( (double)Px/horizontal_radius ) ) + 0.5;
		int Py;// = vertical_radius * sin( v ) + 0.5;
		
		//Draw ellipse path from left to P iterating with x-coordinates
		for( int i=0; i<=Px; i++ ){
			double angle = acos( (double)i/horizontal_radius );
			Py = vertical_radius * sin( angle ) + 0.5;
			
				if( Py ){
					if( i ){
						point( {center.x+i, center.y+Py}, color, merge );
						point( {center.x-i, center.y+Py}, color, merge );
						point( {center.x+i, center.y-Py}, color, merge );
						point( {center.x-i, center.y-Py}, color, merge );
					}
					else{
						point( {center.x, center.y+Py}, color, merge );
						point( {center.x, center.y-Py}, color, merge );
					}
				}
				else{
					point( {center.x-i, center.y}, color, merge );
					point( {center.x+i, center.y}, color, merge );
				}
		}
		
		
		//*Draw ellipse path from P and down iterating with y-coordinates
		for( int i=Py_start-1; i>=0; i-- ){ //This should be Py-1 to prevent overlap
			double angle = asin( (double)i/vertical_radius );
			int dx = horizontal_radius * cos( angle ) + 0.5;
			
				if( dx ){
					if( i ){
						point( {center.x+dx, center.y+i}, color, merge );
						point( {center.x+dx, center.y-i}, color, merge );
						point( {center.x-dx, center.y+i}, color, merge );
						point( {center.x-dx, center.y-i}, color, merge );
					}
					else{
						point( {center.x+dx, center.y}, color, merge );
						point( {center.x-dx, center.y}, color, merge );
					}
				}
				else{
					point( {center.x, center.y+i}, color, merge );
					point( {center.x, center.y-i}, color, merge );
				}
		}
	}
}

void Canvas::elipseFilled( Point center, int horizontal_radius, int vertical_radius, Color color, Merge merge ){
	
}

void Canvas::text( Point baseline, std::string content, Color color, Merge merge ){
	
}

void Canvas::polygon( std::vector<Point> positions, Color color, Merge merge ){
	
}

void Canvas::polygonFilled( std::vector<Point> positions, Color color, Merge merge ){
	
}
