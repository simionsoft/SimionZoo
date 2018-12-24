using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

using Caliburn.Micro;
using Herd.Files;
using Herd.Network;

namespace Badger.ViewModels
{
    public class MonitoredJobViewModel: PropertyChangedBase
    {
        Job m_model;

        //We keep two references to MonitoredExperimentalUnits:
        //1) In a collection directly bound to the view
        private BindableCollection<MonitoredExperimentalUnitViewModel> m_monitoredExperimentalUnits
                = new BindableCollection<MonitoredExperimentalUnitViewModel>();
        public BindableCollection<MonitoredExperimentalUnitViewModel> MonitoredExperimentalUnits
        {
            get { return m_monitoredExperimentalUnits; }
            set { m_monitoredExperimentalUnits = value; NotifyOfPropertyChange(() => MonitoredExperimentalUnits); }
        }
        
        //2) In a dictionary for fast view-model look-ups from the name of the experimental unit
        Dictionary<string, MonitoredExperimentalUnitViewModel> ViewModelFromName 
            = new Dictionary<string, MonitoredExperimentalUnitViewModel>();

        public HerdAgentInfo HerdAgent => m_model.HerdAgent;
        public string Name => m_model.Name;

        public CancellationToken m_cancelToken;

        private PlotViewModel m_evaluationPlot;
        private Action<string> m_logFunction;

        private Dictionary<string, int> m_experimentSeriesId = new Dictionary<string, int>();


        public MonitoredJobViewModel(Job job, PlotViewModel evaluationPlot, CancellationToken cancelToken, Action<string> logFunction)
        {
            m_evaluationPlot = evaluationPlot;
            m_cancelToken = cancelToken;
            m_logFunction = logFunction;

            m_model = job;
        }

        DateTime m_lastHeartbeat = DateTime.Now;

        public void OnMessageReceived(string experimentId, string messageId, string messageContent)
        {
            MonitoredExperimentalUnitViewModel experimentVM = ViewModelFromName[experimentId];

            if (experimentVM == null)
                m_logFunction?.Invoke("Could not find the View-Model for experiment " + experimentId);
            else
            {
                experimentVM.LastHeartbeat = DateTime.Now;
                m_lastHeartbeat = DateTime.Now;
                switch (messageId)
                {
                case JobDispatcher.ProgressMessage:
                    double progress = double.Parse(messageContent, CultureInfo.InvariantCulture);
                    experimentVM.Progress = Convert.ToInt32(progress);
                    break;
                case JobDispatcher.EvaluationMessage:
                    //<Evaluation>0.0,-1.23</Evaluation>
                    string[] values = messageContent.Split(',');
                    string seriesName = experimentVM.Name;
                    int seriesId;
                    if (values.Length == 2)
                    {
                        if (!m_experimentSeriesId.Keys.Contains(experimentVM.Name))
                        {
                            seriesId = m_evaluationPlot.AddLineSeries(seriesName);
                            m_experimentSeriesId.Add(seriesName, seriesId);
                        }
                        else seriesId = m_experimentSeriesId[seriesName];

                        m_evaluationPlot.AddLineSeriesValue(seriesId, double.Parse(values[0], CultureInfo.InvariantCulture)
                            , double.Parse(values[1], CultureInfo.InvariantCulture));
                    }
                    break;
                case JobDispatcher.GeneralMessage:
                    experimentVM.AddStatusInfoLine(messageContent);
                    break;
                case JobDispatcher.EndMessage:
                    if (messageContent == JobDispatcher.EndMessageOk || messageContent == "")
                    {
                        m_logFunction?.Invoke("Job finished sucessfully");
                        experimentVM.State = Monitoring.State.WAITING_RESULT;
                    }
                    else
                    {
                        m_logFunction?.Invoke("Remote job execution wasn't successful");
                        //Right now, my view on adding failed experiments back to the pending exp. list:
                        //Some experiments may fail because the parameters are just invalid (i.e. FAST)
                        //Much more likely than a network-related error or some other user-related problem
                        //FailedExperiments.Add(experimentVM);
                        experimentVM.State = Monitoring.State.ERROR;
                    }
                    break;
                }
            }
        }

        public void OnStateChanged(string experimentId, Monitoring.State state)
        {
            MonitoredExperimentalUnitViewModel experimentVM = ViewModelFromName[experimentId];
            experimentVM.State = state;
        }

        public void OnAllStatesChanged(Monitoring.State state)
        {
            foreach (MonitoredExperimentalUnitViewModel experimentVM in MonitoredExperimentalUnits)
                experimentVM.State = state;
        }

        public void OnExperimentalUnitLaunched(ExperimentalUnit expUnit)
        {
            MonitoredExperimentalUnitViewModel expUnitVM= new MonitoredExperimentalUnitViewModel(expUnit,m_evaluationPlot);
            ViewModelFromName[expUnit.Name] = expUnitVM;
            MonitoredExperimentalUnits.Add(expUnitVM);
        }


        /// <summary>
        /// Normalized progress in this job [0,1]
        /// </summary>
        public double NormalizedProgress
        {
            get
            {
                double progressSum = 0.0;
                if (MonitoredExperimentalUnits.Count == 0) return progressSum;
                foreach (MonitoredExperimentalUnitViewModel expUnit in MonitoredExperimentalUnits)
                    progressSum += (expUnit.Progress / 100.0); //<- they are percentages
                return progressSum / MonitoredExperimentalUnits.Count;
            }
        }
    }

}
