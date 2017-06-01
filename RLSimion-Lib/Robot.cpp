#include "stdafx.h"
#include "Robot.h"

void Robot::setRelativeVariables(CState* s, int idX, int idY, double valX, double valY)
{
	btTransform bodyTrans;
	getBody()->getMotionState()->getWorldTransform(bodyTrans);

	{
		s->set(idX, fabs(bodyTrans.getOrigin().getX() - valX));
		s->set(idY, fabs(bodyTrans.getOrigin().getZ() - valY));
	}
}