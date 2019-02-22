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

using System.IO;
using System.Runtime.Serialization;

using Caliburn.Micro;

using Badger.Data;

namespace Badger.ViewModels
{
    [DataContract]
    public class StatViewModel : PropertyChangedBase
    {
        [DataMember]
        public string ExperimentalUnitConfigFile { get; set; }
        [DataMember]
        public string LogBinaryFile { get; set; }
        [DataMember]
        public string LogDescriptorFile { get; set; }
        [DataMember]
        public string ExperimentId { get; set; }
        [DataMember]
        public string TrackId { get; set; }
        [DataMember]
        public Stats Stats { get; set; }

        public StatViewModel(string experimentId, string trackId, Stats stats
            , string logBinaryFile, string logDescriptorFile, string experimentalUnitConfigFile)
        {
            ExperimentId = experimentId;
            TrackId = trackId;
            Stats = stats;
            LogBinaryFile = logBinaryFile;
            LogDescriptorFile = logDescriptorFile;
            ExperimentalUnitConfigFile = experimentalUnitConfigFile;
        }

        public void Export(StreamWriter fileWriter, string leftSpace)
        {
            fileWriter.WriteLine(leftSpace + "<Stats>");
            fileWriter.WriteLine(leftSpace + "  <Avg>" + Stats.avg + "</Avg>");
            fileWriter.WriteLine(leftSpace + "  <StdDev>" + Stats.stdDev + "</StdDev>");
            fileWriter.WriteLine(leftSpace + "  <Max>" + Stats.max + "</Max>");
            fileWriter.WriteLine(leftSpace + "  <Min>" + Stats.min + "</Min>");
            fileWriter.WriteLine(leftSpace + "</Stats>");
        }
    }
}