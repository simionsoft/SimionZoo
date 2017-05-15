using System;
using System.Collections.Generic;
using System.Xml;
using Badger.Simion;
using Badger.ViewModels.Reports;
using Caliburn.Micro;


namespace Badger.ViewModels
{
    public class LoggedExperimentViewModel : SelectableTreeItem
    {
        public string Name { get; set; } = "";

        public string ExeFile { get; set; }

        public List<LoggedPrerequisiteViewModel> Prerequisites { get; set; } = new List<LoggedPrerequisiteViewModel>();

        public List<LoggedForkViewModel> Forks { get; set; } = new List<LoggedForkViewModel>();

        public List<LoggedExperimentalUnitViewModel> ExperimentalUnits { get; set; } = new List<LoggedExperimentalUnitViewModel>();

        public override void TraverseAction(bool doActionLocally, Action<SelectableTreeItem> action)
        {
            if (doActionLocally) LocalTraverseAction(action);
            foreach (LoggedForkViewModel fork in Forks) fork.TraverseAction(true, action);
        }


        public LoggedExperimentViewModel(XmlNode configNode, ReportsWindowViewModel parent)
        {
            m_parentWindow = parent;
            XmlAttributeCollection attrs = configNode.Attributes;

            if (attrs != null)
            {
                if (attrs.GetNamedItem(XMLConfig.nameAttribute) != null)
                    Name = attrs[XMLConfig.nameAttribute].Value;

                if (attrs.GetNamedItem(XMLConfig.ExeFileNameAttr) != null)
                    ExeFile = attrs[XMLConfig.ExeFileNameAttr].Value;
            }

            foreach (XmlNode child in configNode.ChildNodes)
            {
                switch (child.Name)
                {
                    case XMLConfig.forkTag:
                        LoggedForkViewModel newFork = new LoggedForkViewModel(child);
                        Forks.Add(newFork);
                        break;

                    case XMLConfig.PrerequisiteTag:
                        LoggedPrerequisiteViewModel newPrerequisite = new LoggedPrerequisiteViewModel(child);
                        Prerequisites.Add(newPrerequisite);
                        break;

                    case XMLConfig.experimentalUnitNodeTag:
                        LoggedExperimentalUnitViewModel newExpUnit = new LoggedExperimentalUnitViewModel(child);

                        if (parent != null)
                        {
                            newExpUnit.logDescriptor = new XmlDocument();
                            newExpUnit.logDescriptor.Load(newExpUnit.logDescriptorFilePath);
                            List<string> variableNames = newExpUnit.processDescriptor();
                            foreach (var name in variableNames) AddVariable(name);
                        }

                        ExperimentalUnits.Add(newExpUnit);
                        break;
                }
            }

        }

        /// <summary>
        ///     Call after reading the log file descriptor of each experimetal unit
        /// </summary>
        /// <param name="variableName"></param>
        public void AddVariable(string variableName)
        {
            bool bVarExists = false;
            int i = 0, len = variables.Count;

            while (!bVarExists && i < len)
            {
                if (variables[i].name == variableName)
                    bVarExists = true;
                i++;
            }

            if (!bVarExists)
            {
                variables.Add(new LoggedVariableViewModel(variableName));
                inGroupSelectionVariables.Add(variableName);
                orderByVariables.Add(variableName);
            }
        }

        //Variables
        private BindableCollection<LoggedVariableViewModel> m_variables
            = new BindableCollection<LoggedVariableViewModel>();
        public BindableCollection<LoggedVariableViewModel> variables
        {
            get { return m_variables; }
            set { m_variables = value; NotifyOfPropertyChange(() => variables); }
        }

        private bool m_bVariableSelection = true;
        public bool bVariableSelection
        {
            get { return m_bVariableSelection; }
            set
            {
                m_bVariableSelection = value;
                foreach (LoggedVariableViewModel var in variables)
                {
                    var.bIsSelected = value;
                    m_parentWindow.validateQuery();
                    NotifyOfPropertyChange(() => bIsSelected);
                }
            }
        }

        private BindableCollection<string> m_inGroupSelectionVariables = new BindableCollection<string>();
        public BindableCollection<string> inGroupSelectionVariables
        {
            get { return m_inGroupSelectionVariables; }
            set { m_inGroupSelectionVariables = value; NotifyOfPropertyChange(() => inGroupSelectionVariables); }
        }

        private BindableCollection<string> m_orderByVariables = new BindableCollection<string>();
        public BindableCollection<string> orderByVariables
        {
            get { return m_orderByVariables; }
            set { m_orderByVariables = value; NotifyOfPropertyChange(() => orderByVariables); }
        }

        public void groupByThisFork(string forkName)
        {
            //this method is called from the context menu
            //informs the parent window that results should be grouped by this fork
            m_groupByForks.Add(forkName);
            m_parentWindow.validateQuery();
            NotifyOfPropertyChange(() => groupBy);
            bGroupsEnabled = true;
        }

        //Group By
        private BindableCollection<string> m_groupByForks = new BindableCollection<string>();
        public BindableCollection<string> GroupByForks { get; set; } = new BindableCollection<string>();

        public string groupBy
        {
            get
            {
                string s = "";
                for (int i = 0; i < m_groupByForks.Count - 1; i++) s += m_groupByForks[i] + ",";
                if (m_groupByForks.Count > 0) s += m_groupByForks[m_groupByForks.Count - 1];
                return s;
            }
        }

        public void resetGroupBy()
        {
            m_groupByForks.Clear();
            NotifyOfPropertyChange(() => groupBy);
        }


        private bool m_bGroupsEnabled = false; //no groups by default
        public bool bGroupsEnabled
        {
            get { return m_bGroupsEnabled; }
            set { m_bGroupsEnabled = value; NotifyOfPropertyChange(() => bGroupsEnabled); }
        }

    }





    //public class VarPlotInfo
    //{
    //    public PlotViewModel plot;
    //    public int seriesId;
    //    public int varIndexInLogFile;
    //    public double avg;
    //    public void addValue(double value) { avg += value; }
    //}
    ////for now, it doesn't make much sense to calculate stats but from the last evaluation episode, so that's what we will do
    ////regardless of sourceOption
    //public List<Stat> getVariableStats(List<LoggedVariableViewModel> variables)
    //{
    //    ExperimentData experimentData = SimionLogFile.load(m_logFilePath);
    //    List<Stat> stats = new List<Stat>();

    //    foreach (LoggedVariableViewModel var in variables)
    //    {
    //        int varIndex = m_variablesInLog.FindIndex((name) => name == var.name);
    //        Stat newStat = new Stat(m_name, var.name);
    //        newStat.avg = experimentData.doForEpisodeVar(experimentData.numEpisodes, varIndex,
    //             (episode, vIndex) => { return EpisodeData.calculateVarAvg(episode,vIndex); });
    //        newStat.stdDev = experimentData.doForEpisodeVar(experimentData.numEpisodes, varIndex,
    //             (episode, vIndex) => { return EpisodeData.calculateStdDev(episode, vIndex); });
    //        newStat.min = experimentData.doForEpisodeVar(experimentData.numEpisodes, varIndex,
    //             (episode, vIndex) => { return EpisodeData.calculateMin(episode, vIndex); });
    //        newStat.max = experimentData.doForEpisodeVar(experimentData.numEpisodes, varIndex,
    //             (episode, vIndex) => { return EpisodeData.calculateMax(episode, vIndex); });

    //        stats.Add(newStat);
    //    }


    //    return stats;
    //}
    ////we can use directly the name of the plots as the name of the variables
    ////it should be enough for now
    //public void plotData(List<PlotViewModel> plots, string sourceOption)
    //{
    //    List<VarPlotInfo> varInfoList = new List<VarPlotInfo>();

    //    try
    //    {
    //        //init the series and get the index of each logged variable
    //        plots.ForEach((plot) =>
    //        {
    //            VarPlotInfo varInfo = new VarPlotInfo();
    //            varInfo.plot = plot;
    //            varInfo.seriesId = plot.addLineSeries(m_name);
    //            varInfo.varIndexInLogFile = m_variablesInLog.FindIndex((name) => name == plot.name);
    //            varInfo.avg = 0.0;
    //            varInfoList.Add(varInfo);
    //        });

    //        ExperimentData experimentData = SimionLogFile.load(m_logFilePath);

    //        foreach (VarPlotInfo var in varInfoList)
    //        {
    //            int varIndex = var.varIndexInLogFile;

    //            if (sourceOption == ReportsWindowViewModel.m_optionAllEvalEpisodes)
    //                experimentData.doForEachEvalEpisode(episode =>
    //                        {
    //                            double avg = EpisodeData.calculateVarAvg(episode, varIndex);
    //                            var.plot.addLineSeriesValue(var.seriesId, episode.index, avg);
    //                        });
    //            else if (sourceOption == ReportsWindowViewModel.m_optionLastEvalEpisode)
    //                experimentData.doForEpisodeSteps(experimentData.numEpisodes,
    //                    step =>
    //                    {
    //                        var.plot.addLineSeriesValue(var.seriesId, step.stepIndex
    //                                    , step.data[var.varIndexInLogFile]);
    //                    });
    //        }
    //     }
    //    catch (Exception ex)
    //    {
    //        Console.WriteLine("Error generating plots");
    //        Console.WriteLine(ex.ToString());
    //    }
    //    }
    //}
}