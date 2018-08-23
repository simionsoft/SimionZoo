using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Badger.Simion;
using System.Collections.Generic;

namespace TrackBeauty
{
    public class SeriesComparer : IComparer<Series>
    {
        bool m_bAsc;
        bool m_bUseBeauty;

        public SeriesComparer(bool asc, bool bUseBeauty)
        {
            m_bAsc = asc;
            m_bUseBeauty = bUseBeauty;
        }
        public int Compare(Series x, Series y)
        {

            if (!m_bUseBeauty)
            {
                if ((m_bAsc && x.Stats.avg >= y.Stats.avg)
                    || (!m_bAsc && x.Stats.avg <= y.Stats.avg))
                    return 1;
                return -1;
            }
            else
            {
                if ((m_bAsc && x.Stats.ascBeauty >= y.Stats.ascBeauty)
                    || (!m_bAsc && x.Stats.dscBeauty >= y.Stats.dscBeauty))
                    return 1;
                return -1;
            }
        }
    }
        [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void AscBeauty()
        {
            Report report = new Report("test",Badger.Data.ReportType.EvaluationAverages, "None");

            Series aSeries = new Series();
            Series bSeries = new Series();
            Series cSeries = new Series();

            aSeries.AddValue(0.0, 0.0);
            aSeries.AddValue(1.0, 3.0);
            aSeries.AddValue(2.0, 4.0);
            aSeries.AddValue(3.0, 3.0);
            aSeries.AddValue(4.0, 4.0);
            aSeries.AddValue(5.0, 5.0);
            aSeries.AddValue(6.0, 5.0);
            aSeries.CalculateStats(report);

            bSeries.AddValue(0.0, 0.0);
            bSeries.AddValue(1.0, 3.0);
            bSeries.AddValue(2.0, 4.0);
            bSeries.AddValue(3.0, 1.0);
            bSeries.AddValue(4.0, 4.0);
            bSeries.AddValue(5.0, 6.0);
            bSeries.AddValue(6.0, 5.0);
            bSeries.CalculateStats(report);

            cSeries.AddValue(0.0, 0.0);
            cSeries.AddValue(1.0, 13.0);
            cSeries.AddValue(2.0, 14.0);
            cSeries.AddValue(3.0, 1.0);
            cSeries.AddValue(4.0, 24.0);
            cSeries.AddValue(5.0, 6.0);
            cSeries.AddValue(6.0, 35.0);
            cSeries.CalculateStats(report);

            Assert.IsTrue(aSeries.Stats.ascBeauty > bSeries.Stats.ascBeauty);
            Assert.IsTrue(bSeries.Stats.ascBeauty > cSeries.Stats.ascBeauty);
        }


        [TestMethod]
        public void AscBeauty2()
        {
            Report report = new Report("test", Badger.Data.ReportType.EvaluationAverages, "None");

            Series aSeries = new Series();
            Series bSeries = new Series();
            Series cSeries = new Series();

            aSeries.AddValue(0.0, 0.0);
            aSeries.AddValue(1.0, 1.0);
            aSeries.AddValue(2.0, 2.0);
            aSeries.AddValue(3.0, 3.0);
            aSeries.AddValue(4.0, 4.0);
            aSeries.AddValue(5.0, 5.0);
            aSeries.AddValue(6.0, 6.0);
            aSeries.CalculateStats(report);

            bSeries.AddValue(0.0, 0.0);
            bSeries.AddValue(1.0, 2.0);
            bSeries.AddValue(2.0, 3.0);
            bSeries.AddValue(3.0, 0.0);
            bSeries.AddValue(4.0, 5.0);
            bSeries.AddValue(5.0, 4.0);
            bSeries.AddValue(6.0, 6.0);
            bSeries.CalculateStats(report);

            cSeries.AddValue(0.0, 0.0);
            cSeries.AddValue(1.0, 1.0);
            cSeries.AddValue(2.0, 2.0);
            cSeries.AddValue(3.0, 1.0);
            cSeries.AddValue(4.0, 4.0);
            cSeries.AddValue(5.0, 4.0);
            cSeries.AddValue(6.0, 5.0);
            cSeries.CalculateStats(report);

            Assert.IsTrue(aSeries.Stats.ascBeauty > bSeries.Stats.ascBeauty);
            Assert.IsTrue(aSeries.Stats.ascBeauty > cSeries.Stats.ascBeauty);
        }
        [TestMethod]
        public void AscBeauty3()
        {
            Report report = new Report("test", Badger.Data.ReportType.EvaluationAverages, "None");

            Series aSeries = new Series();
            Series bSeries = new Series();
            Series cSeries = new Series();

            aSeries.AddValue(0.0, 0.0);
            aSeries.AddValue(1.0, 2.0);
            aSeries.AddValue(2.0, 4.0);
            aSeries.AddValue(3.0, 5.0);
            aSeries.AddValue(4.0, 5.0);
            aSeries.AddValue(5.0, 5.0);
            aSeries.AddValue(6.0, 5.0);
            aSeries.CalculateStats(report);

            bSeries.AddValue(0.0, 0.0);
            bSeries.AddValue(1.0, 2.0);
            bSeries.AddValue(2.0, 4.0);
            bSeries.AddValue(3.0, 6.0);
            bSeries.AddValue(4.0, 5.0);
            bSeries.AddValue(5.0, 6.0);
            bSeries.AddValue(6.0, 7.0);
            bSeries.CalculateStats(report);

            cSeries.AddValue(0.0, 0.0);
            cSeries.AddValue(1.0, 5.0);
            cSeries.AddValue(2.0, 8.0);
            cSeries.AddValue(3.0, 4.0);
            cSeries.AddValue(4.0, 8.0);
            cSeries.AddValue(5.0, 4.0);
            cSeries.AddValue(6.0, 9.0);
            cSeries.CalculateStats(report);

            Assert.IsTrue(aSeries.Stats.ascBeauty > bSeries.Stats.ascBeauty);
            Assert.IsTrue(aSeries.Stats.ascBeauty > cSeries.Stats.ascBeauty);
        }
        [TestMethod]
        public void AscBeautySorting()
        {
            Report report = new Report("test", Badger.Data.ReportType.EvaluationAverages, "None");

            Series aSeries = new Series();
            Series bSeries = new Series();
            Series cSeries = new Series();

            aSeries.AddValue(0.0, 0.0);
            aSeries.AddValue(1.0, 3.0);
            aSeries.AddValue(2.0, 4.0);
            aSeries.AddValue(3.0, 3.0);
            aSeries.AddValue(4.0, 4.0);
            aSeries.AddValue(5.0, 5.0);
            aSeries.AddValue(6.0, 5.0);
            aSeries.CalculateStats(report);

            bSeries.AddValue(0.0, 0.0);
            bSeries.AddValue(1.0, 3.0);
            bSeries.AddValue(2.0, 4.0);
            bSeries.AddValue(3.0, 1.0);
            bSeries.AddValue(4.0, 4.0);
            bSeries.AddValue(5.0, 6.0);
            bSeries.AddValue(6.0, 5.0);
            bSeries.CalculateStats(report);

            cSeries.AddValue(0.0, 0.0);
            cSeries.AddValue(1.0, 13.0);
            cSeries.AddValue(2.0, 14.0);
            cSeries.AddValue(3.0, 1.0);
            cSeries.AddValue(4.0, 24.0);
            cSeries.AddValue(5.0, 6.0);
            cSeries.AddValue(6.0, 35.0);
            cSeries.CalculateStats(report);

            List<Series> tracks = new List<Series>();
            tracks.Add(aSeries);
            tracks.Add(bSeries);
            tracks.Add(cSeries);
            tracks.Sort(new SeriesComparer(true,true));

            Assert.IsTrue(tracks[0].Stats.ascBeauty < tracks[1].Stats.ascBeauty
                && tracks[1].Stats.ascBeauty < tracks[2].Stats.ascBeauty);
        }
        [TestMethod]
        public void DscBeauty()
        {
            Report report = new Report("test", Badger.Data.ReportType.EvaluationAverages, "None");

            Series aSeries = new Series();
            Series bSeries = new Series();
            Series cSeries = new Series();

            aSeries.AddValue(0.0, 0.0);
            aSeries.AddValue(1.0, -3.0);
            aSeries.AddValue(2.0, -4.0);
            aSeries.AddValue(3.0, -3.0);
            aSeries.AddValue(4.0, -4.0);
            aSeries.AddValue(5.0, -5.0);
            aSeries.AddValue(6.0, -5.0);
            aSeries.CalculateStats(report);

            bSeries.AddValue(0.0, 0.0);
            bSeries.AddValue(1.0, -3.0);
            bSeries.AddValue(2.0, -4.0);
            bSeries.AddValue(3.0, -1.0);
            bSeries.AddValue(4.0, -4.0);
            bSeries.AddValue(5.0, -6.0);
            bSeries.AddValue(6.0, -5.0);
            bSeries.CalculateStats(report);

            cSeries.AddValue(0.0, 0.0);
            cSeries.AddValue(1.0, -13.0);
            cSeries.AddValue(2.0, -14.0);
            cSeries.AddValue(3.0, -1.0);
            cSeries.AddValue(4.0, -24.0);
            cSeries.AddValue(5.0, -6.0);
            cSeries.AddValue(6.0, -35.0);
            cSeries.CalculateStats(report);

            Assert.IsTrue(aSeries.Stats.dscBeauty > bSeries.Stats.dscBeauty);
            Assert.IsTrue(bSeries.Stats.dscBeauty > cSeries.Stats.dscBeauty);
        }
        [TestMethod]
        public void DscBeauty2()
        {
            Report report = new Report("test", Badger.Data.ReportType.EvaluationAverages, "None");

            Series aSeries = new Series();
            Series bSeries = new Series();
            Series cSeries = new Series();

            aSeries.AddValue(0.0, 10.0);
            aSeries.AddValue(1.0, 6.0);
            aSeries.AddValue(2.0, 4.0);
            aSeries.AddValue(3.0, 3.0);
            aSeries.AddValue(4.0, 4.0);
            aSeries.AddValue(5.0, 5.0);
            aSeries.AddValue(6.0, 4.0);
            aSeries.CalculateStats(report);

            bSeries.AddValue(0.0, 10.0);
            bSeries.AddValue(1.0, 3.0);
            bSeries.AddValue(2.0, 4.0);
            bSeries.AddValue(3.0, 1.0);
            bSeries.AddValue(4.0, 4.0);
            bSeries.AddValue(5.0, 6.0);
            bSeries.AddValue(6.0, 5.0);
            bSeries.CalculateStats(report);

            cSeries.AddValue(0.0, 0.0);
            cSeries.AddValue(1.0, -13.0);
            cSeries.AddValue(2.0, 14.0);
            cSeries.AddValue(3.0, -1.0);
            cSeries.AddValue(4.0, -24.0);
            cSeries.AddValue(5.0, 6.0);
            cSeries.AddValue(6.0, -35.0);
            cSeries.CalculateStats(report);

            Assert.IsTrue(aSeries.Stats.dscBeauty > bSeries.Stats.dscBeauty);
            Assert.IsTrue(bSeries.Stats.dscBeauty > cSeries.Stats.dscBeauty);
        }
    }
}
