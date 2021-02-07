#include "ezport.h"

#include <math.h>
#include <stdio.h>

#include "jr/misc.h"
#include "jr/math.h"

/*
 -225deg      |                Q1
  135deg      |
 (-10, 10)    |
      *       |       * (10, 10) 45deg counter-clockwise
              |                -265deg clockwise
Q2            |
              |
-----------------------------
Q3            |
              |
              |
      *       |       * (10, -10) -45deg
(-10, -10)    |                   265deg
   -135deg    |
    225deg    |                Q4
*/

int jr_PointQuadrant(x, y)
	double	x ;
	double	y ;
{
	if (y > 0) {
		if (x > 0)	return(1) ;
		else		return(2) ;
	}
	else {
		if (x > 0)	return(4) ;
		else		return(3) ;
	}
}

double jr_PointDistance(x1, y1, x2, y2)
	double	x1 ;
	double	y1 ;
	double	x2 ;
	double	y2 ;
{
	double		distance ;

	distance =	sqrt(
					((x1 - x2) * (x1 - x2)) +
					((y1 - y2) * (y1 - y2))
				) ;
	
	return(distance) ;
}

double jr_PointAngleInRadians(x1, y1, x2, y2)
	double	x1 ;
	double	y1 ;
	double	x2 ;
	double	y2 ;
{
    double	angle_in_radians ;
	double	x, y ;
	jr_int		quadrant ;

	x = fabs(x2 - x1) ;
	y = fabs(y2 - y1) ;

	angle_in_radians = atan2(y, x) ;

	quadrant = jr_PointQuadrant(x2 - x1, y2 - y1) ;

	switch(quadrant) {
		case 1 : {
			/* angle_in_radians ; unchanged */
			break ;
		}
		case 2 : {
			angle_in_radians = jr_PI - angle_in_radians ;
			break ;
		}
		case 3 : {
			angle_in_radians = jr_PI + angle_in_radians ;
			break ;
		}
		case 4 : {
			angle_in_radians = 2 * jr_PI - angle_in_radians ;
			break ;
		}
	}

	return(angle_in_radians) ;
}

double jr_PointAngleInDegrees(x1, y1, x2, y2)
	double	x1 ;
	double	y1 ;
	double	x2 ;
	double	y2 ;
{
	double	angle_in_radians ;

	angle_in_radians = jr_PointAngleInRadians(x1,y1,x2,y2) ;

	return(jr_RadiansToDegrees(angle_in_radians)) ;
}

double jr_DegreesToRadians(degrees)
	double degrees ;
{
	double	radians ;

	radians = jr_PI / 180 * degrees ;

	return (radians);
}

double jr_RadiansToDegrees(radians)
	double radians ;
{
	double	degrees ;

	degrees = 180 / jr_PI * radians ;

	return(degrees) ;
}

/*
void main()
{
	double angle_in_radians ;
	double angle_in_degrees ;

	angle_in_radians = jr_PointAngleRadians(0.0, 0.0, 3.0, 4.0) ;
	angle_in_degrees = jr_RadiansToDegrees(angle_in_radians) ;
	fprintf(stdout, "Q0 angle(0,0,3,4) == %lg\n", angle_in_degrees) ;

	angle_in_radians = jr_PointAngleRadians(0.0, 0.0, -3.0, 4.0) ;
	angle_in_degrees = jr_RadiansToDegrees(angle_in_radians) ;
	fprintf(stdout, "Q1 angle(0,0,-3,4) == %lg\n", angle_in_degrees) ;

	angle_in_radians = jr_PointAngleRadians(0.0, 0.0, -3.0, -4.0) ;
	angle_in_degrees = jr_RadiansToDegrees(angle_in_radians) ;
	fprintf(stdout, "Q2 angle(0,0,-3,-4) == %lg\n", angle_in_degrees) ;

	angle_in_radians = jr_PointAngleRadians(0.0, 0.0, 3.0, -4.0) ;
	angle_in_degrees = jr_RadiansToDegrees(angle_in_radians) ;
	fprintf(stdout, "Q3 angle(0,0,3,-4) == %lg\n", angle_in_degrees) ;
}   
*/
