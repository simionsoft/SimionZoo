using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using OxyPlot;
using OxyPlot.Axes;
using System.Collections.ObjectModel;
using System.Collections.Specialized;

namespace AppXML.ViewModels
{
    public class EvaluationPlotViewModel: PropertyChangedBase
    {
        double m_minX = double.MaxValue;
        double m_maxX = double.MinValue;
        double m_minY = double.MaxValue;
        double m_maxY = double.MinValue;

        int m_numSeries= 0;

        private PlotModel m_evaluationPlot;
        public PlotModel evaluationPlot { get { return m_evaluationPlot; } set { } }

        public EvaluationPlotViewModel()
        {
            m_evaluationPlot = new PlotModel { Title="test"};
            var xAxis = new LinearAxis();
            xAxis.Position = AxisPosition.Bottom;
            xAxis.MajorGridlineStyle = LineStyle.Solid;
            xAxis.Minimum = 0.0;
            xAxis.Maximum = 1.0;
            m_evaluationPlot.Axes.Add(xAxis);
            var yAxis = new LinearAxis();
            yAxis.Position = AxisPosition.Left;
            yAxis.MajorGridlineStyle = LineStyle.Solid;
            yAxis.Minimum = 0.0;
            yAxis.Maximum = 1.0;
            m_evaluationPlot.Axes.Add(yAxis);
        }
        
        public int addLineSeries(string title)
        {
            m_numSeries++;

            var newSeries = new OxyPlot.Series.LineSeries { Title = title, MarkerType = MarkerType.None };
            m_evaluationPlot.Series.Add(newSeries);
            return m_numSeries;
        }
        public void addLineSeriesValue(int seriesIndex,double xValue, double yValue)
        {
            if (seriesIndex <= 0 || seriesIndex > m_evaluationPlot.Series.Count)
            {
                //at least, we should log the error   
                return;
            }

            var series = (OxyPlot.Series.LineSeries)m_evaluationPlot.Series[seriesIndex];
            series.Points.Add(new DataPoint(xValue,yValue));
            NotifyOfPropertyChange(() => evaluationPlot);
        }
    }
}
