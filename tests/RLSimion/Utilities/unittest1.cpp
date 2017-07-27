#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../tools/WindowsUtils/FileUtils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Utilities
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(RLSimion_Utilities_getDirectory)
		{
			string result;
			result = getDirectory("C:/jander\\clander/more.txt");
			Assert::AreEqual(string("C:/jander\\clander/"), result);
			result = getDirectory("more.txt");
			Assert::AreEqual(string(""), result);
		}
		TEST_METHOD(RLSimion_Utilities_getFilename)
		{
			string path = "C:/jander/clander\\more.txt";
			Assert::AreEqual(string("more.txt"), getFilename(path));
		}
		TEST_METHOD(RLSimion_Utilities_removeExtension)
		{
			string filename = "C:/jander\\clander/more.simion.exp";
			Assert::AreEqual(string("C:/jander\\clander/more.simion"), removeExtension(filename, 1));
			Assert::AreEqual(string("C:/jander\\clander/more.simion"), removeExtension(filename));
			Assert::AreEqual(string("C:/jander\\clander/more"), removeExtension(filename, 2));
			filename = "jander/clander.exp/more.simion.exp";
			Assert::AreEqual(string("jander/clander.exp/more.simion"), removeExtension(filename, 1));
			Assert::AreEqual(string("jander/clander.exp/more"), removeExtension(filename, 2));
			Assert::AreEqual(filename, removeExtension(filename, 0));
			Assert::AreEqual(filename, removeExtension(filename, 3));
			filename = "jander\\clander\\harl.simion.exp";
			Assert::AreEqual(string("jander\\clander\\harl.simion"), removeExtension(filename));
			Assert::AreEqual(string("jander\\clander\\harl"), removeExtension(filename,2));
		}
		TEST_METHOD(RLSimion_Utilities_getLastBarPos)
		{
			Assert::AreEqual(size_t(7), getLastBarPos(string("a/s/d\\f/4.txt")));
			Assert::AreEqual(size_t(0), getLastBarPos(string("zeinek-daki-zer")));
		}
	};
}