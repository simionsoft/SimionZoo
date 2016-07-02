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
using System.Threading;

namespace AppXML.ViewModels
{
    public class EvaluationPlotViewModel: PropertyChangedBase
    {
        private const int m_updateFreq= 1000; //plot refresh freq in millseconds
        private Timer m_timer;

        double m_minX = double.MaxValue;
        double m_maxX = double.MinValue;
        double m_minY = double.MaxValue;
        double m_maxY = double.MinValue;

        int m_numSeries= 0;

        private PlotModel m_evaluationPlot;
        public PlotModel evaluationPlot { get { return m_evaluationPlot; } set { } }

        public EvaluationPlotViewModel()
        {
            m_evaluationPlot = new PlotModel { Title="Evaluation episodes"};
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

            m_timer = new Timer(updatePlot);
            m_timer.Change(m_updateFreq, m_updateFreq);
        }

        private void updatePlot(object state)
        {
            m_evaluationPlot.InvalidatePlot(true);
        }
        
        public int addLineSeries(string title)
        {
            var newSeries = new OxyPlot.Series.LineSeries { Title = title, MarkerType = MarkerType.None };
            m_evaluationPlot.Series.Add(newSeries);
            int newSeriesId = m_numSeries;
            m_numSeries++;
            return newSeriesId;
        }
        public void addLineSeriesValue(int seriesIndex,double xValue, double yValue)
        {
            if (seriesIndex < 0 || seriesIndex >= m_evaluationPlot.Series.Count)
            {
                //at least, we should log the error   
                return;
            }

            OxyPlot.Series.LineSeries series = (OxyPlot.Series.LineSeries) m_evaluationPlot.Series[seriesIndex];
            updatePlotBounds(xValue, yValue);
            series.Points.Add(new DataPoint(xValue,yValue));

            //NotifyOfPropertyChange(() => evaluationPlot);
        }
        private void updatePlotBounds(double x,double y)
        {
            //bool bMustUpdateX = false;
            bool bMustUpdateY = false;
            //if (x<m_minX)
            //{
            //    m_minX = x;
            //    bMustUpdateX = true;
            //}
            //if (x>m_maxX)
            //{
            //    m_maxX = x;
            //    bMustUpdateX = true;
            //}
            if (y<m_minY)
            {
                m_minY = y;
                bMustUpdateY = true;
            }
            if (y>m_maxY)
            {
                m_maxY = y;
                bMustUpdateY = true;
            }
            //if (bMustUpdateX)
            //{
            //    m_evaluationPlot.Axes[0].Maximum = m_maxX;
            //    m_evaluationPlot.Axes[0].Minimum = m_minX;
            //}
            if (bMustUpdateY)
            {
                double tmpMaxY = m_maxY;
                double tmpMinY = m_minY;
                if (tmpMaxY-tmpMinY==0.0)
                {
                    tmpMinY -= 0.01; tmpMaxY += 0.01;
                }
                m_evaluationPlot.Axes[1].Maximum= tmpMaxY;
                m_evaluationPlot.Axes[1].Minimum= tmpMinY;
            }
        }
    }
}
