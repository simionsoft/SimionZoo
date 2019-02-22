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

using Badger.Data;
using Caliburn.Micro;
using System.Runtime.Serialization;

namespace Badger.ViewModels
{
    [DataContract]
    public class PlotLineSeriesPropertiesViewModel : PropertyChangedBase
    {
        private bool m_bVisible;
        [DataMember]
        public bool Visible
        {
            get { return m_bVisible; }
            set
            {
                m_bVisible = value;
                if (LineSeries!=null)
                    LineSeries.IsVisible = Visible;
                NotifyOfPropertyChange(() => Visible);
            }
        }
        private string m_name = "N/A";
        [DataMember]
        public string Name
        {
            get { return m_name; }
            set
            {
                m_name = value;
                NotifyOfPropertyChange(() => Name);
                if (LineSeries!=null)
                    LineSeries.Title = value;
            }
        }
        private string m_description = "N/A";
        [DataMember]
        public string Description
        {
            get { return m_description; }
            set
            {
                m_description = value;
                NotifyOfPropertyChange(() => Description);
            }
        }

        OxyPlot.Series.LineSeries m_lineSeries = null;
        public OxyPlot.Series.LineSeries LineSeries
        {
            get { return m_lineSeries; }
            set
            {
                m_lineSeries = value;
                NotifyOfPropertyChange(() => Visible);
                NotifyOfPropertyChange(() => Description);
                NotifyOfPropertyChange(() => Name);
            }
        }

        public PlotLineSeriesPropertiesViewModel(string name, string description, OxyPlot.Series.LineSeries lineSeries)
        {
            LineSeries = lineSeries;
            Visible = true;
            Name = Herd.Utils.OxyPlotMathNotation(name);
            Description = description;
        }
    }
}
