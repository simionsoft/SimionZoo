#include "transform2d.h"

Transform2D::Transform2D()
{
	m_translation = Vector2D(0.0, 0.0);
	m_rotation = 0.0;
	m_scale = Vector2D(1.0, 1.0);
	m_depth = 0.0;
}


double* Transform2D::getOpenGLMatrix()
{

	double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = 0.0;  y2 = 0.0;  z2 = m_rotation + m_rotation;
	xx = 0.0;       xy = 0.0;       xz = 0.0;
	yy = 0.0;       yz = 0.0;       zz = m_rotation * z2;
	wx = 0.0;       wy = 0.0;       wz = z2;

	m_matrix44[0] = (1.0 - (yy + zz))*m_scale.x();
	m_matrix44[1] = (xy - wz)*m_scale.y();
	m_matrix44[2] = (xz + wy);
	m_matrix44[3] = 0.0;
	m_matrix44[4] = (xy + wz)*m_scale.x();
	m_matrix44[5] = (1.0 - (xx + zz))*m_scale.y();
	m_matrix44[6] = (yz - wx);
	m_matrix44[7] = 0.0;
	m_matrix44[8] = (xz - wy)*m_scale.x();
	m_matrix44[9] = (yz + wx)*m_scale.y();
	m_matrix44[10] = (1.0 - (xx + yy));
	m_matrix44[11] = 0.0;
	m_matrix44[12] = m_translation.x();
	m_matrix44[13] = m_translation.y();
	m_matrix44[14] = 0.0;
	m_matrix44[15] = 1.0;
	return m_matrix44;
}


