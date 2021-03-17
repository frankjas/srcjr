#ifndef _jr_math_h__
#define _jr_math_h__

#include "ezport.h"

extern double			jr_DegreesToRadians				PROTO((double degrees)) ;
extern double 			jr_RadiansToDegrees				PROTO((double radians)) ;
extern double			jr_DegreesToRadians				PROTO((double degrees)) ;
extern double			jr_PointAngleInDegrees			PROTO((
								double	x1,
								double	y1,
								double	x2,
								double	y2
						)) ;
extern double			jr_PointAngleInRadians			PROTO((
								double	x1,
								double	y1,
								double	x2,
								double	y2
						)) ;
extern double			jr_PointDistance				PROTO((
								double	x1,
								double	y1,
								double	x2,
								double	y2
						)) ;

extern jr_int			jr_PointQuadrant				PROTO((double x, double	y)) ;


#define jr_InterpolationChange(x, y, num_frames, frame_number)	\
		(((y) - (x)) * ((frame_number) / (float) (num_frames)))


#endif
