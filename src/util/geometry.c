/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#include "taisei.h"

#include "geometry.h"

#include <string.h>

bool point_in_ellipse(complex p, Ellipse e) {
	double Xp = creal(p);
	double Yp = cimag(p);
	double Xe = creal(e.origin);
	double Ye = cimag(e.origin);
	double a = e.angle;

	return (
		pow(cos(a) * (Xp - Xe) + sin(a) * (Yp - Ye), 2) / pow(creal(e.axes)/2, 2) +
		pow(sin(a) * (Xp - Xe) - cos(a) * (Yp - Ye), 2) / pow(cimag(e.axes)/2, 2)
	) <= 1;
}

// Is the point of shortest distance between the line through a and b
// and a point c between a and b and closer than r?
// if yes, return f so that a+f*(b-a) is that point.
// otherwise return -1.
double lineseg_circle_intersect(LineSegment seg, Circle c) {
	complex m, v;
	double projection, lv, lm, distance;

	m = seg.b - seg.a; // vector pointing along the line
	v = seg.a - c.origin; // vector from circle to point A

	lv = cabs(v);
	lm = cabs(m);

	if(lv < c.radius) {
		return 0;
	}

	if(lm == 0) {
		return -1;
	}

	projection = -creal(v*conj(m)) / lm; // project v onto the line

	// now the distance can be calculated by Pythagoras
	distance = sqrt(pow(lv, 2) - pow(projection, 2));

	if(distance <= c.radius) {
		double f = projection/lm;

		if(f >= 0 && f <= 1) { // it’s on the line!
			return f;
		}
	}

	return -1;
}

bool lineseg_ellipse_intersect(LineSegment seg, Ellipse e) {
	// Transform the coordinate system so that the ellipse becomes a circle
	// with origin at (0, 0) and diameter equal to its X axis. Then we can
	// calculate the segment-circle intersection.

	double ratio = creal(e.axes) / cimag(e.axes);
	complex rotation = cexp(I * -e.angle);
	seg.a *= rotation;
	seg.b *= rotation;
	seg.a = creal(seg.a) + I * ratio * cimag(seg.a);
	seg.b = creal(seg.b) + I * ratio * cimag(seg.b);

	Circle c = { .radius = creal(e.axes) / 2 };
	return lineseg_circle_intersect(seg, c) >= 0;
}

bool rect_in_rect(Rect inner, Rect outer) {
	return
		rect_left(inner)   >= rect_left(outer)  &&
		rect_right(inner)  <= rect_right(outer) &&
		rect_top(inner)    >= rect_top(outer)   &&
		rect_bottom(inner) <= rect_bottom(outer);
}

bool rect_rect_intersect(Rect r1, Rect r2, bool edges) {
	if(
		rect_bottom(r1) < rect_top(r2)    ||
		rect_top(r1)    > rect_bottom(r2) ||
		rect_left(r1)   > rect_right(r2)  ||
		rect_right(r1)  < rect_left(r2)
	) {
		// Not even touching
		return false;
	}

	if(!edges && (
		rect_bottom(r1) == rect_top(r2)    ||
		rect_top(r1)    == rect_bottom(r2) ||
		rect_left(r1)   == rect_right(r2)  ||
		rect_right(r1)  == rect_left(r2)
	)) {
		// Discard edge intersects
		return false;
	}

	if(
		(rect_left(r1) == rect_right(r2) && rect_bottom(r1) == rect_top(r2)) ||
		(rect_left(r1) == rect_right(r2) && rect_bottom(r2) == rect_top(r1)) ||
		(rect_left(r2) == rect_right(r1) && rect_bottom(r1) == rect_top(r2)) ||
		(rect_left(r2) == rect_right(r1) && rect_bottom(r2) == rect_top(r1))
	) {
		// Discard corner intersects
		return false;
	}

	return true;
}

bool rect_rect_intersection(Rect r1, Rect r2, bool edges, Rect *out) {
	if(!rect_rect_intersect(r1, r2, edges)) {
		return false;
	}

	out->top_left = CMPLX(
    fmax(rect_left(r1), rect_left(r2)),
		fmax(rect_top(r1), rect_top(r2))
	);

	out->bottom_right = CMPLX(
    fmin(rect_right(r1), rect_right(r2)),
		fmin(rect_bottom(r1), rect_bottom(r2))
	);

	return true;
}

bool rect_join(Rect *r1, Rect r2) {
	if(rect_in_rect(r2, *r1)) {
		return true;
	}

	if(rect_in_rect(*r1, r2)) {
		memcpy(r1, &r2, sizeof(r2));
		return true;
	}

	if(!rect_rect_intersect(*r1, r2, true)) {
		return false;
	}

	if(rect_left(*r1) == rect_left(r2) && rect_right(*r1) == rect_right(r2)) {
		// r2 is directly above/below r1
		double y_min = fmin(rect_top(*r1), rect_top(r2));
		double y_max = fmax(rect_bottom(*r1), rect_bottom(r2));

		r1->top_left = CMPLX(creal(r1->top_left), y_min);
		r1->bottom_right = CMPLX(creal(r1->bottom_right), y_max);

		return true;
	}

	if(rect_top(*r1) == rect_top(r2) && rect_bottom(*r1) == rect_bottom(r2)) {
		// r2 is directly left/right to r1
		double x_min = fmin(rect_left(*r1), rect_left(r2));
		double x_max = fmax(rect_right(*r1), rect_right(r2));

		r1->top_left = CMPLX(x_min, cimag(r1->top_left));
		r1->bottom_right = CMPLX(x_max, cimag(r1->bottom_right));

		return true;
	}

	return false;
}

void rect_set_xywh(Rect *rect, double x, double y, double w, double h) {
	rect->top_left = CMPLX(x, y);
	rect->bottom_right = CMPLX(w, h) + rect->top_left;
}
