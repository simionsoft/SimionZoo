using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Badger.ViewModels;
using System.Collections.Generic;
using Badger.Converters;
using Badger.Data;
using System.Globalization;
using System.Windows.Data;

namespace LogQueryCopyTest
{
    [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void TestMethod1()
        {
            LogQueryViewModel query1, query2;

            query1 = new LogQueryViewModel();
            query2 = new LogQueryViewModel();

            List<string> variableList = new List<string>() { "E_p", "v", "omega_r", "omega_g" };

            //init query1 before copying it
            query1.AddLogVariables(variableList);
            query1.VariablesVM[0].IsSelected = true;
            query1.VariablesVM[2].IsSelected = true;
            query1.LimitTracks = true;
            query1.MaxNumTracks = 3;
            query1.AddGroupByFork("fork-1");
            query1.InGroupSelectionVariable = "omega_g";
            query1.InGroupSelectionFunction = LogQueryViewModel.FunctionMax;
            query1.OrderByFunction = LogQueryViewModel.FunctionDscBeauty;
            query1.OrderByVariable = "E_p";
            Badger.Converters.EnumDescriptionConverter conv = new Badger.Converters.EnumDescriptionConverter();
            
            query1.OrderByReportType = ReportType.AllTrainingEpisodes.ToString();
            query1.AfterExperimentBatchLoad();

            Assert.IsTrue(query1.CanGenerateReports);


            //The deep copy itself
            query2.DeepCopy(query1);


            //check query2 is a deep copy of query1
            variableList.Remove("omega_g"); //check the list is a deep copy of the original
            foreach (string variable in variableList)
                Assert.IsTrue(query2.LogVariableExists(variable));
            Assert.IsTrue(query2.LogVariableExists("omega_g"));

            Assert.IsTrue(query2.LimitTracks);
            Assert.IsTrue(query2.MaxNumTracks == 3);

            Assert.IsTrue(query2.IsForkUsedToGroup("fork-1"));

            Assert.AreEqual(query1.InGroupSelectionVariable, query2.InGroupSelectionVariable);

            Assert.AreEqual(query1.InGroupSelectionFunction, query2.InGroupSelectionFunction);

            Assert.AreEqual(query2.OrderByFunction, LogQueryViewModel.FunctionDscBeauty);

            Assert.AreEqual(query1.OrderByReportType, query2.OrderByReportType);

            Assert.IsTrue(query2.CanGenerateReports);

            Assert.AreNotEqual(query1.QueryId, query2.QueryId);
        }
    }
}
