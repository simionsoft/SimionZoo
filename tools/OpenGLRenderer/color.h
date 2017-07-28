#pragma once

namespace tinyxml2 { class XMLElement; }

class Color
{
	float _values[4];
public:
	Color(float r, float g, float b, float a)
	{
		_values[0] = r; _values[1] = g; _values[2] = b; _values[3] = a;
	}
	Color(double r, double g, double b, double a)
		:Color((float)r, (float)g, (float)b, (float)a) {}
	Color() :Color(1.0, 1.0, 1.0, 1.0) {}
	float r() const { return _values[0]; }
	float g() const { return _values[1]; }
	float b() const { return _values[2]; }
	float a() const { return _values[3]; }
	void setR(double r) { _values[0] = (float) r; }
	void setG(double g) { _values[1] = (float) g; }
	void setB(double b) { _values[2] = (float) b; }
	void setA(double a) { _values[3] = (float) a; }

	Color operator*(double scalar) const;
	Color operator+(Color& c) const;

	void set(float r, float g, float b, float a) { _values[0] = r; _values[1] = g; _values[2] = b; _values[3] = a; }
	float* rgba() { return _values; }
	Color lerp(Color& c, double u);
};
