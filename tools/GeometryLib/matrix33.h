#pragma once

#include "vector3d.h"
class Quaternion;

class Matrix33
{
protected:
	Vector3D m_x, m_y, m_z;
public:
	Vector3D& x() { return m_x; }
	Vector3D& y() { return m_y; }
	Vector3D& z() { return m_z; }

	void setIdentity()
	{
		m_x.setX(1.0); m_x.setY(0.0); m_x.setZ(0.0);
		m_y.setX(0.0); m_y.setY(1.0); m_y.setZ(0.0);
		m_z.setX(0.0); m_z.setY(0.0); m_z.setZ(1.0);
	}

	void setRotation(Quaternion& quat);

	void asArray33(double* outMatrix)
	{
		outMatrix[0] = m_x.x(); outMatrix[1] = m_x.y(); outMatrix[2] = m_x.z();
		outMatrix[3] = m_y.x(); outMatrix[4] = m_y.y(); outMatrix[5] = m_y.z();
		outMatrix[6] = m_z.x(); outMatrix[7] = m_z.y(); outMatrix[8] = m_z.z();
	}
	void asArray44(double* outMatrix)
	{
		outMatrix[0] = m_x.x(); outMatrix[1] = m_x.y(); outMatrix[2] = m_x.z(); outMatrix[3] = 0.0;
		outMatrix[4] = m_y.x(); outMatrix[5] = m_y.y(); outMatrix[6] = m_y.z(); outMatrix[7] = 0.0;
		outMatrix[8] = m_z.x(); outMatrix[9] = m_z.y(); outMatrix[10] = m_z.z(); outMatrix[11] = 0.0;
		outMatrix[12] = 0.0; outMatrix[13] = 0.0; outMatrix[14] = 0.0; outMatrix[15] = 1.0;
	}

	Vector3D operator*(Vector3D& vec);
};


