#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../RLSimion/Lib/sample-file.h"
#include "../../tools/System/CrossPlatform.h"
#include "../../RLSimion/Common/named-var-set.h"
#include <string>
using namespace std;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SampleFilesTests
{
	TEST_CLASS(SampleFileTest)
	{
		//These tests check the functionality of the Experiment class
	public:
		string descriptor1 = "<SampleFileDescriptor BinaryDataFile=\"";
		string descriptor2 = "\" NumSamples=\"";
		string descriptor3 = "\"><State-variable>x</State-variable><Action-variable>u</Action-variable><Reward-variable>r</Reward-variable></SampleFileDescriptor>";
		int numVariables = 4;
		string createSampleFileDescriptor(string name, int numSamples)
		{
			string binFileName = name + ".bin";
			FILE* file;
			CrossPlatform::Fopen_s(&file, name.c_str(), "w");
			CrossPlatform::Fprintf_s(file, "%s%s%s%d%s", descriptor1.c_str(), binFileName.c_str(), descriptor2.c_str(), numSamples, descriptor3.c_str());
			fclose(file);
			return binFileName;
		}

		void createSampleFileBin(string name, int numSamples, int numVariables)
		{
			FILE* file;
			CrossPlatform::Fopen_s(&file, name.c_str(), "wb");
			double value = 0;
			for (int i = 0; i < numSamples; i++)
			{
				for (int var = 0; var < numVariables; var++)
				{
					fwrite((void*)&value, sizeof(double), 1, file);
					value += 0.1;
				}
			}
			fclose(file);
		}

		void checkSampleFile(string name, int numSamples)
		{
			SampleFile sampleFile(name);
			Descriptor sDesc, aDesc, rDesc;
			sDesc.addVariable("x", "m", 0.0, 1000000);
			aDesc.addVariable("u", "N", 0.0, 1000000);

			State* s = new State(sDesc);
			State* s_p = new State(sDesc);
			Action* a = new Action(aDesc);

			double reward;
			double expected = 0.0;
			for (size_t sample = 0; sample < numSamples; sample++)
			{
				sampleFile.drawRandomSample(s, a, s_p, reward);
				Assert::AreEqual(expected, s->get((size_t)0)); expected += 0.1;
				Assert::AreEqual(expected, a->get((size_t)0)); expected += 0.1;
				Assert::AreEqual(expected, s_p->get((size_t)0)); expected += 0.1;
				Assert::AreEqual(expected, reward); expected += 0.1;
			}
			//check we are back to the beginning
			sampleFile.drawRandomSample(s, a, s_p, reward);
			expected = 0.0;
			Assert::AreEqual(expected, s->get((size_t)0)); expected += 0.1;
			Assert::AreEqual(expected, a->get((size_t)0)); expected += 0.1;
			Assert::AreEqual(expected, s_p->get((size_t)0)); expected += 0.1;
			Assert::AreEqual(expected, reward);
		}

		TEST_METHOD(SampleFile_Small)
		{
			//Create sample file
			int numSamples = 987;
			string sampleFilename = "test1.simion.samples";
			string binaryFilename = createSampleFileDescriptor(sampleFilename, numSamples);
			createSampleFileBin(binaryFilename, numSamples, numVariables);
			checkSampleFile(sampleFilename, numSamples);
		}
		TEST_METHOD(SampleFile_Medium)
		{
			//Create sample file
			int numSamples = 12014;
			string sampleFilename = "test2.simion.samples";
			string binaryFilename = createSampleFileDescriptor(sampleFilename, numSamples);
			createSampleFileBin(binaryFilename, numSamples, numVariables);
			checkSampleFile(sampleFilename, numSamples);
		}
		TEST_METHOD(SampleFile_Larger)
		{
			//Create sample file
			int numSamples = 35214;
			string sampleFilename = "test3.simion.samples";
			string binaryFilename = createSampleFileDescriptor(sampleFilename, numSamples);
			createSampleFileBin(binaryFilename, numSamples, numVariables);
			checkSampleFile(sampleFilename, numSamples);
		}
	};
}