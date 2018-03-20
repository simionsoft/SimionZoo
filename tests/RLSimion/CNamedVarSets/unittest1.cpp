#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../RLSimion/named-var-set.h"

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
			Descriptor desc;
			desc.addVariable("var1", "rad", lowerLimit, upperLimit, true);
			desc.addVariable("var2", "rad", lowerLimit, upperLimit);
			State* s= desc.getInstance();

			//lower limit
			s->set("var1", lowerLimit - 0.1);
			Assert::AreEqual(s->get("var1"),upperLimit-0.1);
			s->set("var2", lowerLimit - 0.1);
			Assert::AreEqual(lowerLimit, s->get("var2"));

			//upper limit
			s->set("var1", upperLimit + 0.1);
			Assert::AreEqual(lowerLimit + 0.1, s->get("var1"));
			s->set("var2", upperLimit + 0.1);
			Assert::AreEqual(upperLimit, s->get("var2"));
		}

	};
}