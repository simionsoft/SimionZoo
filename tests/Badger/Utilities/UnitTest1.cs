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
        public void Badger_Utility_RemoveExtension()
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
        [TestMethod]
        public void Badger_Utility_NumParts()
        {
            string s = ".simion.proj";
            Assert.AreEqual(2, Utility.NumParts(s, '.'));
            s = "a.simion.proj";
            Assert.AreEqual(3, Utility.NumParts(s, '.'));
        }
        [TestMethod]
        public void Badger_Utility_OxyplotMathNotation()
        {
            //α β γ δ ε ζ η θ ι κ λ μ ν ξ ο π ρ σ τ υ φ χ ψ ω
            //Α Β Γ Δ Ε Ζ Η Θ Ι Κ Λ Μ Ν Ξ Ο Π Ρ Σ Τ Υ Φ Χ Ψ Ω

            string original = "E_p";
            string res= Utility.OxyPlotMathNotation(original);
            Assert.AreEqual("E_{p}", res);

            original = "E_int_omega_r,E_p";
            res = Utility.OxyPlotMathNotation(original);
            Assert.AreEqual("E_{int_ω_r},E_{p}", res);

            original = "KP_alpha";
            res = Utility.OxyPlotMathNotation(original);
            Assert.AreEqual("KP_{α}", res);

            original = "Alpha_i";
            res = Utility.OxyPlotMathNotation(original);
            Assert.AreEqual("α_{i}", res);

            original = "TD(lambda)";
            res = Utility.OxyPlotMathNotation(original);
            Assert.AreEqual("TD(λ)", res);
        }
    }
}
