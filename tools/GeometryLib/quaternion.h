#pragma once

class Matrix33;


class Quaternion
{
protected:
	double m_x, m_y, m_z, m_w;
	//these are defined to be able to bind regular orientations to quaternions
	//they should only be set by the bindings read from the config file
	double m_yaw= 0.0, m_pitch= 0.0, m_roll= 0.0;
	bool m_bOrientationsSet = false;
public:
	Quaternion(double x, double y, double z, double w) :m_x(x), m_y(y), m_z(z), m_w(w) {}
	Quaternion(double yaw, double pitch, double roll);
	Quaternion() { setIdentity(); }

	//if any of the orientation angles has been set, we should use them instead of the quaternion
	bool bUseOrientations() const { return m_bOrientationsSet; }
	void fromOrientations(double yaw, double pitch, double roll);
	void fromOrientations();


	static double xFromOrientations (double yaw, double pitch, double roll);
	static double yFromOrientations (double yaw, double pitch, double roll);
	static double zFromOrientations (double yaw, double pitch, double roll);
	static double wFromOrientations (double yaw, double pitch, double roll);

	double x() const;
	double y() const;
	double z() const;
	double w() const;

	double yaw() const;
	double pitch() const;
	double roll() const;

	void setX(const double x);
	void setY(const double y);
	void setZ(const double z);
	void setW(const double w);

	void setYaw(const double yaw);
	void setPitch(const double pitch);
	void setRoll(const double roll);

	void setIdentity() { m_x = 0.0; m_y = 0.0; m_z = 0.0; m_w = 1.0; }

	void asMatrix33(Matrix33& matrix) const;
	Quaternion inverse();

	Quaternion operator+(const Quaternion quat);
	Quaternion operator-(const Quaternion quat);
	Quaternion operator*(const Quaternion quat);
	void operator+=(const Quaternion quat);
	void operator-=(const Quaternion quat);
	void operator*=(const Quaternion quat);
};


