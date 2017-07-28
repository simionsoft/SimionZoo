#include "stdafx.h"
#include "color.h"
#include "../GeometryLib/vector2d.h"

Color Color::operator*(double scalar) const
{
	Range limits(0.0,1.0);
	return Color((float)limits.clamp(r()*scalar), (float)limits.clamp(g()*scalar)
		, (float)limits.clamp(b()*scalar), (float)limits.clamp(a()*scalar));
}

Color Color::operator+(Color& c) const
{
	Range limits(0.0, 1.0);
	return Color((float)limits.clamp(r()+c.r()), (float) limits.clamp(g()+c.g())
		, (float)limits.clamp(b()+c.b()), (float)limits.clamp(a()+c.a()));
}

Color Color::lerp(Color& c, double u)
{
	return (*this)*(1 - u) + c*u;
}