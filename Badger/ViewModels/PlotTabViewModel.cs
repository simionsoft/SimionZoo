using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;
using System.Collections.ObjectModel;
using Caliburn.Micro;


namespace AppXML.ViewModels
{
    public class PlotTabViewModel : PropertyChangedBase
    {


        private PlotEditorWindowViewModel m_parent;

        private PlotViewModel m_plot = new PlotViewModel("Unnamed", false);
        public PlotViewModel plot { get { return m_plot; } set { } }

        //private ObservableCollection<ExperimentViewModel> m_experimentList = new ObservableCollection<ExperimentViewModel>();
        //public ObservableCollection<ExperimentViewModel> experimentList { get { return m_experimentList; } set { } }

        private string m_name = "Unnamed";
        public string name { get { return m_name; } set { m_name = value; } }

        //SOURCE OPTIONS
        //private const string m_optionLastEvalEpisode = "Last evaluation episode";
        //private const string m_optionAllEvalEpisodes = "All evaluation episodes";

        //private List<string> m_sourceOptions = new List<string>();
        //public List<string> sourceOptions { get { return m_sourceOptions; } set { } }

        //private string m_selectedSource;
        //public string selectedSource { get { return m_selectedSource; } 
        //    set { m_selectedSource = value;
        //    generatePlotIfValidates();}   }

        //VARIABLE LIST
        //private List<string> m_variableList= new List<string>();
        //public List<string> variableList { get { return m_variableList; } set { } }

        //private string m_selectedVariable;
        //public string selectedVariable { get { return m_selectedVariable; }
        //    set { m_selectedVariable = value;
        //    updateExperimentList();
        //    generatePlotIfValidates(); } }

        public PlotTabViewModel(PlotEditorWindowViewModel parent)
        {
            m_parent = parent;
            //fill the options combobox
            //sourceOptions.Add(m_optionAllEvalEpisodes);
            //sourceOptions.Add(m_optionLastEvalEpisode);
            //fill the available variables combobox
            //parent.getVariableList(ref m_variableList);
            //NotifyOfPropertyChange(() => variableList);
        }
        //private void updateExperimentList()
        //{
        //    if (m_selectedVariable!=null)
        //    lock(m_variableList)
        //    {
        //        m_parent.getExperimentList(m_selectedVariable,ref m_experimentList);
        //    }
        //    NotifyOfPropertyChange(() => experimentList);
        //}
        //private void generatePlotIfValidates()
        //{
        //    if (selectedSource!=null && selectedVariable!=null)
        //    {
        //        Task.Run(() => generatePlot());
        //    }
        //}
        //private void generatePlot()
        //{
        //    foreach(ExperimentViewModel experiment in m_experimentList)
        //    {
        //        int numVariables = 0;
        //        int variableIndex= m_parent.getVariableIndex(experiment,m_selectedVariable, ref numVariables);
        //        drawVariableSeries(experiment.name, variableIndex, numVariables,experiment.getLogFilePath());
        //    }
        //    m_plot.updatePlot();
        //}

        //private void drawVariableSeries(string seriesName, int variableIndex, int numVariables, string logFilePath)
        //{
        //    double avgValue = 0.0; // in case we have to calculate the variable's average value (m_optionAllEvaluationEpisodes)
        //    int numEpisodes= 0,fileFormatVersion=0;
        //    int seriesId = m_plot.addLineSeries(seriesName);

        //    //load the experiment log file
        //    FileStream logFile = File.OpenRead(logFilePath);
        //    using (BinaryReader logReader = new BinaryReader(logFile))
        //    {
        //        readExperimentLogHeader(logReader,ref numEpisodes, ref fileFormatVersion);
        //        for (int episode= 0; episode<numEpisodes; episode++)
        //        {
        //            int episodeType= 0, episodeIndex= 0;
        //            int stepIndex = 0, numLoggedSteps;
        //            double experimentRealTime = 0.0, episodeSimTime = 0.0, episodeRealTime = 0.0;

        //            readEpisodeHeader(logReader, ref episodeType, ref episodeIndex
        //                , ref numVariables);

        //            numLoggedSteps = 0;
        //            bool bLastStep= readStepHeader(logReader, ref stepIndex
        //                    , ref experimentRealTime, ref episodeSimTime
        //                    , ref episodeRealTime);

        //            while (!bLastStep)
        //            {
        //                //do we have to draw data from this step?
        //                if (episodeType == m_episodeTypeEvaluation &&
        //                    (m_selectedSource == m_optionAllEvalEpisodes || (m_selectedSource == m_optionLastEvalEpisode && episodeIndex == numEpisodes)))
        //                {
        //                    numLoggedSteps++;
        //                    byte[] stepData = readStepData(logReader, numVariables);

        //                    double[] doubleData = new double[numVariables];
        //                    Buffer.BlockCopy(stepData, 0, doubleData, 0, (int)numVariables * sizeof(double));

        //                    if (m_selectedSource == m_optionAllEvalEpisodes)
        //                        avgValue += doubleData[variableIndex];
        //                    else m_plot.addLineSeriesValue(seriesId, stepIndex, doubleData[variableIndex]);
        //                }
        //                else readStepData(logReader, numVariables);

        //                //read another step
        //                bLastStep = readStepHeader(logReader, ref stepIndex
        //                    , ref experimentRealTime, ref episodeSimTime
        //                    , ref episodeRealTime);
        //            }
        //            //end of episode
        //            if (m_selectedSource == m_optionAllEvalEpisodes)
        //                m_plot.addLineSeriesValue(seriesId, episodeIndex, avgValue / (double)numLoggedSteps);

        //        }
        //    }
        //    logFile.Close();
        //}


    }
}