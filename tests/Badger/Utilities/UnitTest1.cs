using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Badger.Data;
namespace Utilities
{
    [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void Badger_Utility_removeDirectories()
        {
            string result;
            result= Utility.removeDirectories("C:/jander\\clander/more.txt", 2);
            Assert.AreEqual(result, "jander\\clander/more.txt");
            result = Utility.removeDirectories("C:/jander\\clander/more.txt", 1);
            Assert.AreEqual(result, "clander/more.txt");
            result = Utility.removeDirectories("C:/jander\\clander/more.txt");
            Assert.AreEqual(result, "more.txt");
            result = Utility.removeDirectories("C:/jander\\clander/more.txt",5);
            Assert.AreEqual(result, "C:/jander\\clander/more.txt");
            result = Utility.removeDirectories("more.txt",2);
            Assert.AreEqual(result, "more.txt");
        }
        [TestMethod]
        public void Badger_Utility_getDirectory()
        {
            string result;
            result = Utility.getDirectory("C:/jander\\clander/more.txt");
            Assert.AreEqual(result, "C:/jander\\clander/");
            result = Utility.getDirectory("more.txt");
            Assert.AreEqual(result, "");
        }
        }
}
