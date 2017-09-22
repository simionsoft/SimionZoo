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
            result= Utility.RemoveDirectories("C:/jander\\clander/more.txt", 2);
            Assert.AreEqual(result, "jander\\clander/more.txt");
            result = Utility.RemoveDirectories("C:/jander\\clander/more.txt", 1);
            Assert.AreEqual(result, "clander/more.txt");
            result = Utility.RemoveDirectories("C:/jander\\clander/more.txt");
            Assert.AreEqual(result, "more.txt");
            result = Utility.RemoveDirectories("C:/jander\\clander/more.txt",5);
            Assert.AreEqual(result, "C:/jander\\clander/more.txt");
            result = Utility.RemoveDirectories("more.txt",2);
            Assert.AreEqual(result, "more.txt");
        }
        [TestMethod]
        public void Badger_Utility_getDirectory()
        {
            string result;
            result = Utility.GetDirectory("C:/jander\\clander/more.txt");
            Assert.AreEqual(result, "C:/jander\\clander/");
            result = Utility.GetDirectory("more.txt");
            Assert.AreEqual(result, "");
        }
        [TestMethod]
        public void Badger_Utility_removeExtension()
        {
            string filename= "C:/jander\\clander/more.simion.exp";
            Assert.AreEqual("C:/jander\\clander/more.simion", Utility.RemoveExtension(filename, 1));
            Assert.AreEqual("C:/jander\\clander/more.simion", Utility.RemoveExtension(filename));
            Assert.AreEqual("C:/jander\\clander/more", Utility.RemoveExtension(filename, 2));
            filename = "jander/clander.exp/more.simion.exp";
            Assert.AreEqual("jander/clander.exp/more.simion", Utility.RemoveExtension(filename, 1));
            Assert.AreEqual("jander/clander.exp/more", Utility.RemoveExtension(filename, 2));
            Assert.AreEqual(filename, Utility.RemoveExtension(filename, 0));
            Assert.AreEqual(filename, Utility.RemoveExtension(filename,3));
        }
    }
}
