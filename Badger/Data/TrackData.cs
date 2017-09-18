using System;
using System.Collections.Generic;
using Badger.Data;

namespace Badger.Simion
{
    public class DataProcess
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
    public class StatData
    {
        public double min { get; set; }
        public double max { get; set; }
        public double avg { get; set; }
        public double stdDev { get; set; }
    }
    public class ReportParams
    {
        public string Variable;
        public PlotType Type;
        public string ProcessFunc;
        public ReportParams(string varName, PlotType type, string processFunc)
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
    public class XYSeries
    {
        public string Id { get; set; } = null;
        public List<XYValue> Values { get; set; } = new List<XYValue>();

        public void AddValue(double x, double y)
        {
            Values.Add(new XYValue(x, y));
        }

        public StatData Stats= new StatData();

        public void CalculateStats(ReportParams trackParameters)
        {
            //calculate avg, min and max
            double sum = 0.0;

            Stats.min = double.MaxValue;
            Stats.max = double.MinValue;
            foreach (XYValue val in Values)
            {
                if (val.Y > Stats.max) Stats.max = val.Y;
                if (val.Y < Stats.min) Stats.min = val.Y;
                
                sum += DataProcess.Get(trackParameters.ProcessFunc,val.Y);
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

    }
    public class DataSeries
    {
        private ReportParams trackParameters;

        public DataSeries(ReportParams parameters)
        {
            trackParameters = parameters;
        }

        //Each series may have more than one subseries. For example, "AllEvaluations" returns a series
        //for each of the evaluation episodes. Thus, we need a list of subseries (
        public List<XYSeries> SubSeries = new List<XYSeries>();

        public XYSeries Series { get { if (SubSeries.Count>0) return SubSeries[0]; return null; } }
        public void AddSeries(XYSeries series) { SubSeries.Add(series); }
        public StatData Stats = new StatData();
    }
 
    public class TrackData
    {
        public Dictionary<string, string> ForkValues;
        public Dictionary<ReportParams, DataSeries> Data { get; }
            = new Dictionary<ReportParams, DataSeries>();

        public TrackData(Dictionary<string, string> forkValues)
        {
            ForkValues = forkValues;
        }

        public void AddVariableData(ReportParams variable, DataSeries variableData)
        {
            Data.Add(variable, variableData);
        }

        public DataSeries GetDataSeriesForOrdering(string variable)
        {
            foreach (ReportParams track in Data.Keys)
            {
                if (track.Variable == variable) return Data[track];
            }
            return null;
        }

        public bool HasData { get { return Data.Keys.Count != 0; } }
    }
}
