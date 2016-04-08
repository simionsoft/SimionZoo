#include "stdafx.h"
#include "globals.h"





ENUMERATION(Boolean, "False", "True");
ENUMERATION(Distribution, "linear", "quadratic", "cubic");
ENUMERATION(Interpolation, "linear", "quadratic", "cubic");
ENUMERATION(TimeReference, "episode", "experiment", "experimentTime");

//This is a trick to use a string directly from within an app's definition

