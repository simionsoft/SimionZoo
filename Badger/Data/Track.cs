using System;
using System.Collections.Generic;
using Badger.Data;

namespace Badger.Simion
{
    public class ProcessFunc
    {
        public const string None = "None";
        public const string Abs = "Abs";

        public static double Get(string func, double value)
        {
            switch (func)
            {
                case Abs: return Math.Abs(value);
                default:
                case None: return value;
            }
        }
    };
    public class Stats
    {
        public double min { get; set; }
        public double max { get; set; }
        public double avg { get; set; }
        public double stdDev { get; set; }
    }
    public class Report
    {
        public string Variable;
        public ReportType Type;
        public string ProcessFunc;
        public bool Resample = false;
        public int NumSamples= 0;
        public Report(string varName, ReportType type, string processFunc)
        {
            Type = type;
            ProcessFunc = processFunc;
            Variable = varName;
        }
        public string Name { get { return Type.ToString() + "-" + Variable;  } }
    }
    public class XYValue
    {
        public double X;
        public double Y;
        public XYValue(double x, double y)
        {
            X = x;
            Y = y;
        }
    }

    /// <summary>
    /// Single series of data. These series may belong to a series a values in an episode or a set of
    /// per-episode-values. In this case, an Id will be used to distinguish them
    /// </summary>
    public class Series
    {
        public string Id { get; set; } = null;
        public List<XYValue> Values { get; set; } = new List<XYValue>();

        public void AddValue(double x, double y)
        {
            Values.Add(new XYValue(x, y));
        }

        public Stats Stats= new Stats();

        public void CalculateStats(Report trackParameters)
        {
            //calculate avg, min and max
            double sum = 0.0;

            Stats.min = double.MaxValue;
            Stats.max = double.MinValue;
            foreach (XYValue val in Values)
            {
                if (val.Y > Stats.max) Stats.max = val.Y;
                if (val.Y < Stats.min) Stats.min = val.Y;
                
                sum += ProcessFunc.Get(trackParameters.ProcessFunc,val.Y);
            }

            Stats.avg = sum / Values.Count;

            //calculate std. deviation
            double diff;
            sum = 0.0;
            foreach (XYValue val in Values)
            {
                diff = val.Y - Stats.avg;
                sum += diff * diff;
            }

            Stats.stdDev = Math.Sqrt(sum / Values.Count);
        }
        double Sample(double x)
        {
            int i = 1;
            while (i < Values.Count - 1 && Values[i].X < x) i++;
            double x0 = Values[i - 1].X;
            double y0 = Values[i - 1].Y;
            double x1 = Values[i].X;
            double y1 = Values[i].Y;
            double slope = (y1 - y0) / (x1 - x0);
            return y0 + slope*(x-x0);
        }
        public void Resample(int numSamples)
        {
            List<XYValue> resampled = new List<XYValue>(numSamples);
            double minX = Values[0].X;
            double maxX = Values[Values.Count - 1].X;
            double pointDist = (maxX - minX) / (double)(numSamples - 1);
            for (int sample= 0; sample<numSamples; sample++)
            {
                double x = minX + sample * pointDist;
                double y = Sample(x);
                XYValue newSample = new XYValue(x,y);
                resampled.Add(newSample);
            }
            Values= resampled;
        }
    }

    /// <summary>
    /// A group of series. For a given report, there may be more than one series with the same set of
    /// fork values. For example, "AllEvaluations" will read several series
    /// for each of the evaluation episodes, all of them under the same fork values.
    /// </summary>
    public class SeriesGroup
    {
        private Report trackParameters;

        public SeriesGroup(Report parameters)
        {
            trackParameters = parameters;
        }


        public List<Series> SeriesList = new List<Series>();

        /// <summary>
        /// In case we are expecting to have only one series in this group, we can get it by using
        /// MainSeries. The same idea as in TrackGroup, but without having to consolidate series
        /// </summary>
        public Series MainSeries { get { if (SeriesList.Count>0) return SeriesList[0]; return null; } }
        public void AddSeries(Series series) { SeriesList.Add(series); }
        public Stats Stats = new Stats();
    }
 
    /// <summary>
    /// A track contains all the series corresponding to a combination of fork values.
    /// For each Report, a different SeriesGroup will hold all the series with those fork values
    /// </summary>
    public class Track
    {
        public Dictionary<string, string> ForkValues;
        public Dictionary<Report, SeriesGroup> SeriesGroups { get; }
            = new Dictionary<Report, SeriesGroup>();

        public Track(Dictionary<string, string> forkValues)
        {
            ForkValues = forkValues;
        }

        public void AddVariableData(Report variable, SeriesGroup variableData)
        {
            SeriesGroups.Add(variable, variableData);
        }

        public SeriesGroup GetDataSeriesForOrdering(string variable)
        {
            foreach (Report track in SeriesGroups.Keys)
            {
                if (track.Variable == variable) return SeriesGroups[track];
            }
            return null;
        }

        public bool HasData { get { return SeriesGroups.Keys.Count != 0; } }
    }
}
