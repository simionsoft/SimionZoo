using System;
using System.Collections.Generic;
using System.Xml;
using System.Collections.ObjectModel;
using Caliburn.Micro;
using Badger.Data;

namespace Badger.ViewModels
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

        private Dictionary<string, string> forkTags = new Dictionary<string, string>();
        public void addForkTag(string forkName,string value)
        {
            forkTags.Add(forkName, value);
        }
        public List<string> getForks()
        {
            List<string> forks = new List<string>();
            foreach (string forkname in forkTags.Keys)
                forks.Add(forkname);
            return forks;
        }
        public string getForkValue(string forkName)
        {
            if (forkTags.ContainsKey(forkName)) return forkTags[forkName];
            return "";
        }

        private List<string> m_variablesInLog = new List<string>();

        private ReportsWindowViewModel m_parent = null;

        public ExperimentLogViewModel(string name, string logDescriptorFilePath
            , string logFilePath, ReportsWindowViewModel parent)
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


  


        public class VarPlotInfo
        {
            public PlotViewModel plot;
            public int seriesId;
            public int varIndexInLogFile;
            public double avg;
            public void addValue(double value) { avg += value; }
        }
        //for now, it doesn't make much sense to calculate stats but from the last evaluation episode, so that's what we will do
        //regardless of sourceOption
        public List<Stat> getVariableStats(List<LoggedVariableViewModel> variables)
        {
            ExperimentData experimentData = SimionLogFile.load(m_logFilePath);
            List<Stat> stats = new List<Stat>();

            foreach (LoggedVariableViewModel var in variables)
            {
                int varIndex = m_variablesInLog.FindIndex((name) => name == var.name);
                Stat newStat = new Stat(m_name, var.name);
                newStat.avg = experimentData.doForEpisodeVar(experimentData.numEpisodes, varIndex,
                     (episode, vIndex) => { return EpisodeData.calculateVarAvg(episode,vIndex); });
                newStat.stdDev = experimentData.doForEpisodeVar(experimentData.numEpisodes, varIndex,
                     (episode, vIndex) => { return EpisodeData.calculateStdDev(episode, vIndex); });
                newStat.min = experimentData.doForEpisodeVar(experimentData.numEpisodes, varIndex,
                     (episode, vIndex) => { return EpisodeData.calculateMin(episode, vIndex); });
                newStat.max = experimentData.doForEpisodeVar(experimentData.numEpisodes, varIndex,
                     (episode, vIndex) => { return EpisodeData.calculateMax(episode, vIndex); });

                stats.Add(newStat);
            }


            return stats;
        }
        //we can use directly the name of the plots as the name of the variables
        //it should be enough for now
        public void plotData(List<PlotViewModel> plots, string sourceOption)
        {
            List<VarPlotInfo> varInfoList = new List<VarPlotInfo>();

            try
            {
                //init the series and get the index of each logged variable
                plots.ForEach((plot) =>
                {
                    VarPlotInfo varInfo = new VarPlotInfo();
                    varInfo.plot = plot;
                    varInfo.seriesId = plot.addLineSeries(m_name);
                    varInfo.varIndexInLogFile = m_variablesInLog.FindIndex((name) => name == plot.name);
                    varInfo.avg = 0.0;
                    varInfoList.Add(varInfo);
                });

                ExperimentData experimentData = SimionLogFile.load(m_logFilePath);

                foreach (VarPlotInfo var in varInfoList)
                {
                    int varIndex = var.varIndexInLogFile;

                    if (sourceOption == ReportsWindowViewModel.m_optionAllEvalEpisodes)
                        experimentData.doForEachEvalEpisode(episode =>
                                {
                                    double avg = EpisodeData.calculateVarAvg(episode, varIndex);
                                    var.plot.addLineSeriesValue(var.seriesId, episode.index, avg);
                                });
                    else if (sourceOption == ReportsWindowViewModel.m_optionLastEvalEpisode)
                        experimentData.doForEpisodeSteps(experimentData.numEpisodes,
                            step =>
                            {
                                var.plot.addLineSeriesValue(var.seriesId, step.stepIndex
                                            , step.data[var.varIndexInLogFile]);
                            });
                }
                //        EpisodeData.calculateVarAvg
                //                ,plots[var].addLineSeriesValue(varInfoList[var].seriesId,varIndex);
                //        varInfoList[var].addValue(step.data[varInfoList[var].varIndexInLogFile]);
                //        else plots[var].addLineSeriesValue(varInfoList[var].seriesId, step.stepIndex
                //            , step.data[varInfoList[var].varIndexInLogFile]);
                //        experimentData.doForEachEvalEpisode(EpisodeData.calculateVarAvg, varIndex);
                //        }
                //    }

                //        foreach (EpisodeData episodeData in experimentData.episodes)
                //    {
                //        //do we have to draw data from this step?
                //        if (episodeData.type == m_episodeTypeEvaluation &&
                //            (sourceOption == PlotEditorWindowViewModel.m_optionAllEvalEpisodes
                //            || (sourceOption == PlotEditorWindowViewModel.m_optionLastEvalEpisode
                //            && episodeData.index == experimentData.numEpisodes)))
                //        {
                //            foreach (StepData step in episodeData.steps)
                //            {
                //                for (int var = 0; var < varInfoList.Count; var++)
                //                {
                //                    if (sourceOption == PlotEditorWindowViewModel.m_optionAllEvalEpisodes)
                //                        varInfoList[var].addValue(step.data[varInfoList[var].varIndexInLogFile]);
                //                    else plots[var].addLineSeriesValue(varInfoList[var].seriesId, step.stepIndex
                //                        , step.data[varInfoList[var].varIndexInLogFile]);
                //                }
                //            }
                //        }
                //    }
                //            //end of episode
                //            if (episodeType == m_episodeTypeEvaluation && sourceOption == PlotEditorWindowViewModel.m_optionAllEvalEpisodes)
                //            {
                //                for (int var = 0; var < varInfoList.Count; var++ )
                //                {
                //                    plots[var].addLineSeriesValue(varInfoList[var].seriesId, episodeIndex
                //                        , varInfoList[var].avg / (double)numLoggedSteps);
                //                }

                //            }
                //        }
                //    }
                //    logFile.Close();
                //}
                //catch(Exception ex)
                //{
                //    Console.WriteLine(ex.ToString());
                //}




            }
            catch (Exception ex)
            {
                Console.WriteLine("Error generating plots");
                Console.WriteLine(ex.ToString());
            }
        }
    }
}