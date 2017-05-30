#include "stdafx.h"
#include "Box.h"

void Box::setRelativeVariables(CState* s, int idX, int idY, double targetX, double targetY)
{
	btTransform bodyTrans;
	getBody()->getMotionState()->getWorldTransform(bodyTrans);

	{
		s->set(idX, fabs(targetX - bodyTrans.getOrigin().getX()));
		s->set(idY, fabs(targetY - bodyTrans.getOrigin().getZ()));
	}
}