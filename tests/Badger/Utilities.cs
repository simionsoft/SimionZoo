using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Herd;
namespace Utilities
{
    [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void Herd_Utils_removeDirectories()
        {
            string result;
            result= Utils.RemoveDirectories("C:/jander\\clander/more.txt", 2);
            Assert.AreEqual(result, "jander\\clander/more.txt");
            result = Utils.RemoveDirectories("C:/jander\\clander/more.txt", 1);
            Assert.AreEqual(result, "clander/more.txt");
            result = Utils.RemoveDirectories("C:/jander\\clander/more.txt");
            Assert.AreEqual(result, "more.txt");
            result = Utils.RemoveDirectories("C:/jander\\clander/more.txt",5);
            Assert.AreEqual(result, "C:/jander\\clander/more.txt");
            result = Utils.RemoveDirectories("more.txt",2);
            Assert.AreEqual(result, "more.txt");
        }
        [TestMethod]
        public void Herd_Utils_getDirectory()
        {
            string result;
            result = Utils.GetDirectory("C:/jander\\clander/more.txt");
            Assert.AreEqual(result, "C:/jander\\clander/");
            result = Utils.GetDirectory("more.txt");
            Assert.AreEqual(result, "");
        }
        [TestMethod]
        public void Herd_Utils_RemoveExtension()
        {
            string filename= "C:/jander\\clander/more.simion.exp";
            Assert.AreEqual("C:/jander\\clander/more.simion", Utils.RemoveExtension(filename, 1));
            Assert.AreEqual("C:/jander\\clander/more.simion", Utils.RemoveExtension(filename));
            Assert.AreEqual("C:/jander\\clander/more", Utils.RemoveExtension(filename, 2));
            filename = "jander/clander.exp/more.simion.exp";
            Assert.AreEqual("jander/clander.exp/more.simion", Utils.RemoveExtension(filename, 1));
            Assert.AreEqual("jander/clander.exp/more", Utils.RemoveExtension(filename, 2));
            Assert.AreEqual(filename, Utils.RemoveExtension(filename, 0));
            Assert.AreEqual(filename, Utils.RemoveExtension(filename,3));
        }
        [TestMethod]
        public void Herd_Utils_NumParts()
        {
            string s = ".simion.proj";
            Assert.AreEqual(2, Utils.NumParts(s, '.'));
            s = "a.simion.proj";
            Assert.AreEqual(3, Utils.NumParts(s, '.'));
        }
        [TestMethod]
        public void Herd_Utils_OxyplotMathNotation()
        {
            //α β γ δ ε ζ η θ ι κ λ μ ν ξ ο π ρ σ τ υ φ χ ψ ω
            //Α Β Γ Δ Ε Ζ Η Θ Ι Κ Λ Μ Ν Ξ Ο Π Ρ Σ Τ Υ Φ Χ Ψ Ω

            string original = "E_p";
            string res= Utils.OxyPlotMathNotation(original);
            Assert.AreEqual("E_{p}", res);

            original = "E_int_omega_r,E_p";
            res = Utils.OxyPlotMathNotation(original);
            Assert.AreEqual("E_{int_ω_r},E_{p}", res);

            original = "KP_alpha";
            res = Utils.OxyPlotMathNotation(original);
            Assert.AreEqual("KP_{α}", res);

            original = "Alpha_i";
            res = Utils.OxyPlotMathNotation(original);
            Assert.AreEqual("α_{i}", res);

            original = "TD(lambda)";
            res = Utils.OxyPlotMathNotation(original);
            Assert.AreEqual("TD(λ)", res);
        }
    }
}
