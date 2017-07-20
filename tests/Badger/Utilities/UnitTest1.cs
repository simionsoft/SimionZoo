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
        [TestMethod]
        public void Badger_Utility_removeExtension()
        {
            string filename= "C:/jander\\clander/more.simion.exp";
            Assert.AreEqual("C:/jander\\clander/more.simion", Utility.removeExtension(filename, 1));
            Assert.AreEqual("C:/jander\\clander/more.simion", Utility.removeExtension(filename));
            Assert.AreEqual("C:/jander\\clander/more", Utility.removeExtension(filename, 2));
            filename = "jander/clander.exp/more.simion.exp";
            Assert.AreEqual("jander/clander.exp/more.simion", Utility.removeExtension(filename, 1));
            Assert.AreEqual("jander/clander.exp/more", Utility.removeExtension(filename, 2));
            Assert.AreEqual(filename, Utility.removeExtension(filename, 0));
            Assert.AreEqual(filename, Utility.removeExtension(filename,3));
        }
    }
}
