using System.Collections.ObjectModel;
using System.Xml;
using System.Windows;
using Caliburn.Micro;
using System.Windows.Forms;
using System.IO;
using System.Dynamic;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System;
using System.Diagnostics;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;
using System.IO.Pipes;
using System.Xml.Linq;
using System.Linq;
using System.Xml.XPath;
using System.Windows.Media;
using System.Threading;
using System.Globalization;
using System.Collections.Concurrent;
using Herd;
using AppXML.Models;
using AppXML.ViewModels;
using AppXML.Data;


namespace AppXML.ViewModels
{
    public class MonitorWindowViewModel : Caliburn.Micro.Screen
    {
        public ExperimentQueueMonitorViewModel experimentQueueMonitor{get;set;}
        public EvaluationPlotViewModel evaluationPlot{get;set;}

        public MonitorWindowViewModel(List<HerdAgentViewModel> freeHerdAgents
            , List<ExperimentViewModel> pendingExperiments, Utility.LogFunction logFunction)
        {
            evaluationPlot = new EvaluationPlotViewModel();
            experimentQueueMonitor = new ExperimentQueueMonitorViewModel(freeHerdAgents, pendingExperiments
                , evaluationPlot,logFunction);
        }

        public void runExperiments(string batchName, bool monitorProgress= true, bool receiveJobResults= true)
        {
            Task.Run(() => experimentQueueMonitor.runExperimentsAsync(batchName,monitorProgress,receiveJobResults));
        }

        protected override void OnDeactivate(bool close)
        {
            if (close)
                experimentQueueMonitor.stopExperiments();
            base.OnDeactivate(close);
        }

   
    }
}
