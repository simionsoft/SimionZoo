#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../RLSimion-Lib/named-var-set.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CNamedVarSets
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(CNamedVarSet_Circularity)
		{
			double lowerLimit = -3.1415;
			double upperLimit = 3.1415;
			CDescriptor desc;
			desc.addVariable("var1", "rad", lowerLimit, upperLimit, true);
			desc.addVariable("var2", "rad", lowerLimit, upperLimit);
			CState* s= desc.getInstance();
			s->set("var1", lowerLimit-0.1);
			Assert::AreEqual(s->get("var1"),upperLimit-0.1);
			s->set("var2", -3.15);
			Assert::AreEqual(-3.1415, s->get("var2"));

		}

	};
}