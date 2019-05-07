/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

using System;
using System.Collections.Generic;

namespace Badger.Data
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
        //"beauty": heuristic method that tells how nice a curve looks (ascending or descending)
        public double ascBeauty { get; set; }
        public double dscBeauty { get; set; }
    }
    public class Report
    {
        public string Variable;
        public ReportType Type;
        public string ProcessFunc;
        public bool Resample = false;
        public double TimeOffset = 0.0;
        public double MinEpisodeLength = 0.0;
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


        public Series ()
        {
        }
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

            //curve "beauty"
            double prevY= Values[0].Y;
            double prevX = Values[0].X;
            double ascBeauty = 0.0;
            double dscBeauty = 0.0;
            double sumSlopes = 0.0;
            double slope;

            foreach (XYValue val in Values)
            {
                if (val.Y > Stats.max) Stats.max = val.Y;
                if (val.Y < Stats.min) Stats.min = val.Y;

                if (val.X - prevX != 0.0)
                {
                    slope = (ProcessFunc.Get(trackParameters.ProcessFunc, val.Y)
                        - prevY) / (val.X - prevX);


                    if (slope > 0.0)
                    {
                        ascBeauty += slope;
                        dscBeauty -= slope * 10.0;
                    }
                    else if (slope < 0.0)
                    {
                        ascBeauty += slope * 10.0;
                        dscBeauty -= slope;
                    }
                    sumSlopes += slope;
                }
                prevY = val.Y;
                prevX = val.X;
                sum += ProcessFunc.Get(trackParameters.ProcessFunc,val.Y);
            }
            double avgSlope = sumSlopes / Values.Count;

            Stats.ascBeauty = ascBeauty / Values.Count;
            Stats.dscBeauty = dscBeauty / Values.Count;
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

        /// <summary>
        /// This method takes a list of series as input and outputs three series with, respectively the average, maximum and minimum. This method assumes all input series
        /// have the same values in the X axis
        /// </summary>
        /// <param name="seriesList">Input series of values</param>
        /// <param name="minimums">Output maximums for each sample in X</param>
        /// <param name="maximums">Ouptut minimum for each sample in Y</param>
        /// <returns></returns>
        public static void AverageSeriesList(List<Series> seriesList, out Series averages, out Series minimums, out Series maximums)
        {
            averages = null;
            minimums = null;
            maximums = null;

            if (seriesList.Count == 0) return;

            int numValuesPerSeries = seriesList[0].Values.Count;

            averages = new Series();
            minimums = new Series();
            maximums = new Series();

            for (int sample= 0; sample< numValuesPerSeries; sample++)
            {
                double x, avgY= 0, minY= double.MaxValue, maxY= double.MinValue;
                x = seriesList[0].Values[sample].X;
                foreach (Series series in seriesList)
                {
                    avgY += series.Values[sample].Y;
                    if (series.Values[sample].Y < minY) minY= series.Values[sample].Y;
                    if (series.Values[sample].Y > maxY) maxY = series.Values[sample].Y;
                }
                averages.AddValue(x, avgY/seriesList.Count);
                minimums.AddValue(x, minY);
                maximums.AddValue(x, maxY);
            }
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
        public string ExperimentalUnitConfigFile { get; }
        public string LogBinaryFile { get; set; }
        public string LogDescriptorFile { get; set; }
        public Dictionary<string, string> ForkValues;
        public Dictionary<Report, SeriesGroup> SeriesGroups { get; }
            = new Dictionary<Report, SeriesGroup>();

        public Track(Dictionary<string, string> forkValues,string logBinaryFile, string logDescriptorFile, string experimentalUnitConfigFile)
        {
            ForkValues = forkValues;
            LogBinaryFile = logBinaryFile;
            LogDescriptorFile = logDescriptorFile;
            ExperimentalUnitConfigFile = experimentalUnitConfigFile;
        }

        public void AddVariableData(Report variable, SeriesGroup variableData)
        {
            SeriesGroups.Add(variable, variableData);
        }

        public SeriesGroup GetDataSeries(string variable, ReportType type)
        {
            foreach (Report track in SeriesGroups.Keys)
            {
                if (track.Variable == variable && track.Type == type) return SeriesGroups[track];
            }
            return null;
        }

        public bool HasData { get { return SeriesGroups.Keys.Count != 0; } }

        public string TrackId
        {
            get
            {
                if (ForkValues.Count == 0)
                    return "N/A";
                string id = "";
                double value;
                foreach (KeyValuePair<string, string> entry in ForkValues)
                {
                    //we leave the keys of numeric values and remove the keys of alphanumeric ones
                    if (double.TryParse(entry.Value, out value))
                        id += Herd.Utils.LimitLength(entry.Key, 10) + "=" + entry.Value + ",";
                    else id += entry.Value + ",";
                }
                id = id.TrimEnd(',');
                return id;
            }
        }
        public string FullTrackId
        {
            get
            {
                if (ForkValues.Count == 0)
                    return "N/A";
                string id = "";
                double value;
                foreach (KeyValuePair<string, string> entry in ForkValues)
                {
                    //we leave the keys of numeric values and remove the keys of alphanumeric ones
                    if (double.TryParse(entry.Value, out value))
                        id += entry.Key + "=" + entry.Value + ",";
                }
                id = id.TrimEnd(',');
                return id;
            }
        }
    }
}
