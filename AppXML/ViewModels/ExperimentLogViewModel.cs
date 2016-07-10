using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Dynamic;
using System.Windows;
using System.Collections.ObjectModel;
using Caliburn.Micro;
using System.IO;

namespace AppXML.ViewModels
{
    public class ExperimentLogViewModel:PropertyChangedBase
    {
        private const string m_descriptorRootNodeName = "ExperimentLogDescriptor";

        private bool m_bIsSelected = false;
        public bool bIsSelected {
            get { return m_bIsSelected; }
            set 
            {
                m_bIsSelected= value;
                m_parent.updateAvailableVariableList();
                m_parent.updateVariableListHeader();
                m_parent.updateLogListHeader();
                NotifyOfPropertyChange(()=>bIsSelected); }}

        private string m_logFilePath= "";
        public string logFilePath { get { return m_logFilePath; } set { m_logFilePath = value; } }

        private string m_name;
        public string name { get { return m_name; }
            set { m_name = value; NotifyOfPropertyChange(() => name); }
        }

        private XmlDocument m_logDescriptor;
        public XmlDocument logDescriptor{get{return m_logDescriptor;} 
            set{m_logDescriptor=value;}}

        private List<string> m_variablesInLog = new List<string>();

        private PlotEditorWindowViewModel m_parent = null;

        public ExperimentLogViewModel(string name, string logDescriptorFilePath
            , string logFilePath, PlotEditorWindowViewModel parent)
        {
            m_parent = parent;
            m_name = name;
            m_logDescriptor = new XmlDocument();
            logDescriptor.Load(logDescriptorFilePath);
            processDescriptor();

            m_logFilePath = logFilePath;
        }
        private void processDescriptor()
        {
            XmlNode node = m_logDescriptor.FirstChild;
            if (node.Name==m_descriptorRootNodeName)
            {
                foreach (XmlNode child in node.ChildNodes)
                {
                    if (child.Name == "State-variable" || child.Name == "Action-variable" || child.Name == "Reward-variable"
                        || child.Name == "Stat-variable")
                    {
                        string varName = child.InnerText;                
                        m_variablesInLog.Add(varName);
                    }
                }
            }
        }
        public void addVariablesToList(ref ObservableCollection<LoggedVariableViewModel> variableList)
        {
            if (variableList.Count==0)
            {
                //if the list is empty, we add all the variables available in this log
                foreach(string var in m_variablesInLog)
                {
                    variableList.Add(new LoggedVariableViewModel(var, m_parent));
                }
            }
            else
            {
                //else, we intersect both sets: remove all variables not present in this log
                foreach(LoggedVariableViewModel variable in variableList)
                {
                    if (!m_variablesInLog.Contains(variable.name))
                        variableList.Remove(variable);
                }
            }
        }
        public struct VarPlotInfo
        {
            public int seriesId;
            public int varIndexInLogFile;
            public double avg;
            public void addValue(double value) { avg += value; }
        }
        //we can use directly the name of the plots as the name of the variables
        //it should be enough for now
        public void plotData(List<PlotViewModel> plots,string sourceOption)
        {
            int numLoggedSteps;
            int numEpisodes = 0, fileFormatVersion = 0;
            int episodeType = 0, episodeIndex = 0;
            int stepIndex = 0, numVariablesLogged = 0;
            bool bLastStep;
            double expRealTime = 0.0, episodeSimTime = 0.0, episodeRealTime = 0.0;

            List<VarPlotInfo> varInfoList = new List<VarPlotInfo>();

            try
            {
                //init the series and get the index of each logged variable
                plots.ForEach((plot) =>
                {
                    VarPlotInfo varInfo = new VarPlotInfo();
                    varInfo.seriesId= plot.addLineSeries(m_name);
                    varInfo.varIndexInLogFile= m_variablesInLog.FindIndex((name) => name == plot.name);
                    varInfo.avg= 0.0;
                    varInfoList.Add(varInfo);
                });

                FileStream logFile = File.OpenRead(m_logFilePath);
                using (BinaryReader binaryReader = new BinaryReader(logFile))
                {
                    readExperimentLogHeader(binaryReader, ref numEpisodes, ref fileFormatVersion);
                    for (int i = 0; i < numEpisodes; i++)
                    {
                        readEpisodeHeader(binaryReader, ref episodeType, ref episodeIndex
                            , ref numVariablesLogged);
                        numLoggedSteps = 0;

                        bLastStep = readStepHeader(binaryReader, ref stepIndex
                            , ref expRealTime, ref episodeSimTime, ref episodeRealTime);

                        while (!bLastStep)
                        {
                            //do we have to draw data from this step?
                            if (episodeType == m_episodeTypeEvaluation &&
                                (sourceOption == PlotEditorWindowViewModel.m_optionAllEvalEpisodes
                                || (sourceOption == PlotEditorWindowViewModel.m_optionLastEvalEpisode && episodeIndex == numEpisodes)))
                            {
                                numLoggedSteps++;
                                byte[] stepData = readStepData(binaryReader, m_variablesInLog.Count);

                                double[] doubleData = new double[m_variablesInLog.Count];
                                Buffer.BlockCopy(stepData, 0, doubleData, 0, (int)m_variablesInLog.Count * sizeof(double));

                                for (int var = 0; var < varInfoList.Count; var++ )
                                {
                                    if (sourceOption == PlotEditorWindowViewModel.m_optionAllEvalEpisodes)
                                        varInfoList[var].addValue (doubleData[varInfoList[var].varIndexInLogFile]);
                                    else plots[var].addLineSeriesValue(varInfoList[var].seriesId, stepIndex
                                        , doubleData[varInfoList[var].varIndexInLogFile]);
                                }
                            }
                            else readStepData(binaryReader, numVariablesLogged);

                            bLastStep = readStepHeader(binaryReader, ref stepIndex
                                        , ref expRealTime, ref episodeSimTime, ref episodeRealTime);
                        }
                        //end of episode
                        if (sourceOption == PlotEditorWindowViewModel.m_optionAllEvalEpisodes)
                        {
                            for (int var = 0; var < varInfoList.Count; var++ )
                            {
                                plots[var].addLineSeriesValue(varInfoList[var].seriesId, episodeIndex
                                    , varInfoList[var].avg / (double)numLoggedSteps);
                            }
                            
                        }
                    }
                }
                logFile.Close();
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        private const int m_episodeTypeEvaluation = 0;
        private const int m_episodeTypeTraining = 1;

        //BINARY LOG FILE STUFF: constants, reading methods, etc...
        private const int HEADER_MAX_SIZE = 16;

        private const int EXPERIMENT_HEADER = 1;
        private const int EPISODE_HEADER = 2;
        private const int STEP_HEADER = 3;
        private const int EPISODE_END_HEADER = 4;
        /*
struct ExperimentHeader
{
	__int64 magicNumber = EXPERIMENT_HEADER;
	__int64 fileVersion = CLogger::BIN_FILE_VERSION;
	__int64 numEpisodes = 0;

	__int64 padding[HEADER_MAX_SIZE - 3]; //extra space
	ExperimentHeader()
	{
		memset(padding, 0, sizeof(padding));
	}
};
*/
        static private void readExperimentLogHeader(BinaryReader logReader, ref int numEpisodes, ref int fileFormatVersion)
        {
            int magicNumber = (int)logReader.ReadInt64();
            fileFormatVersion = (int)logReader.ReadInt64();
            numEpisodes = (int)logReader.ReadInt64();
            byte[] padding = logReader.ReadBytes(sizeof(double) * (HEADER_MAX_SIZE - 3));
        }
        /*
struct EpisodeHeader
{
	__int64 magicNumber = EPISODE_HEADER;
	__int64 episodeType;
	__int64 episodeIndex;
    __int64 numVariablesLogged;

	__int64 padding[HEADER_MAX_SIZE - 4]; //extra space
	EpisodeHeader()
	{
		memset(padding, 0, sizeof(padding));
	}
};*/
        static private void readEpisodeHeader(BinaryReader logReader, ref int episodeType
            , ref int episodeIndex, ref int numVariablesLogged)
        {
            int magicNumber = (int)logReader.ReadInt64();
            episodeType = (int)logReader.ReadInt64();
            episodeIndex = (int)logReader.ReadInt64();
            numVariablesLogged = (int)logReader.ReadInt64();

            byte[] padding = logReader.ReadBytes(sizeof(double) * (HEADER_MAX_SIZE - 4));
        }
        /*
struct StepHeader
{
	__int64 magicNumber = STEP_HEADER;
	__int64 stepIndex;
	double experimentRealTime;
	double episodeSimTime;
	double episodeRealTime;

	__int64 padding[HEADER_MAX_SIZE - 5]; //extra space
	StepHeader()
	{
		memset(padding, 0, sizeof(padding));
	}
};
        */

        //this function return whether there is more data to read from the current episode or not
        static private bool readStepHeader(BinaryReader logReader, ref int stepIndex
            , ref double ExperimentRealTime
            , ref double EpisodeSimTime, ref double EpisodeRealTime)
        {

            int magicNumber = (int)logReader.ReadInt64();
            stepIndex = (int)logReader.ReadInt64();
            ExperimentRealTime = logReader.ReadDouble();
            EpisodeSimTime = logReader.ReadDouble();
            EpisodeRealTime = logReader.ReadDouble();
            logReader.ReadChars(sizeof(double) * (HEADER_MAX_SIZE - 5));
            if (magicNumber == EPISODE_END_HEADER) return true;
            return false;
        }
        static private byte[] readStepData(BinaryReader logReader, int numVariables)
        {
            return logReader.ReadBytes(sizeof(double) * (int)numVariables);
        }
    }
}