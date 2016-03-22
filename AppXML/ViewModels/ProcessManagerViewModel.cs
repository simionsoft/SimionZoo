using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Threading;

namespace AppXML.ViewModels
{
    public class ProcessManagerViewModel:Caliburn.Micro.PropertyChangedBase
    {
        private ObservableCollection<ProcessStateViewModel> _processes;
        public ObservableCollection<ProcessStateViewModel> Processes
        {
            get { return _processes; }
            set
            {
                _processes = value;
                NotifyOfPropertyChange(() => Processes);
            }
        }
        public ProcessManagerViewModel()
        {
            _processes = new ObservableCollection<ProcessStateViewModel>();
            //ProcessStateViewModel p1 = new ProcessStateViewModel("primero");
            //ProcessStateViewModel p2 = new ProcessStateViewModel();
            //_processes.Add(p1);
            //_processes.Add(p2);
        }
        public ProcessManagerViewModel(List<ProcessStateViewModel> processes)
        {
            _processes = new ObservableCollection<ProcessStateViewModel>(processes);
            
        }
        public void addProcess(ProcessStateViewModel process)
        {
            Processes.Add(process);
            NotifyOfPropertyChange(() => Processes);
        }
        public void addProcess(List<ProcessStateViewModel> processes)
        {
            foreach(ProcessStateViewModel process in processes)
                _processes.Add(process);
            NotifyOfPropertyChange(() => Processes);
        }
        public void run()
        {
            Parallel.ForEach(Processes, (process) =>
                                                {
                                                    string name = process.pipeName ;
                                                    ProcessStartInfo startInfo = new ProcessStartInfo();
                                                    startInfo.FileName = Path.Combine(Directory.GetCurrentDirectory(),"../debug/RLSimion.exe");
                                                    startInfo.Arguments = process.Label + " " + process.pipeName;

                                                    //not to read 23.232 as 23232
                                                    Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;
                                                    //Task.Delay(1000).Wait();
                                                    var server = new NamedPipeServerStream(name);
                                                    Process.Start(startInfo);
                                                    server.WaitForConnection();
                                                    StreamReader reader = new StreamReader(server);
                                                    process.SMS = "Running";
                                                    System.Windows.Forms.Application.DoEvents();
                                                    bool reading = true;
                                                    while (reading)
                                                    {
                                                        string sms = reader.ReadLine();
                                                        XmlDocument xml = new XmlDocument();
                                                        xml.LoadXml(sms);
                                                        XmlNode node = xml.DocumentElement;
                                                        if (node.Name == "Progress")
                                                        {
                                                            double progress = Convert.ToDouble(node.InnerText);
                                                            process.Status = Convert.ToInt32(progress);
                                                            if (progress==100.0)
                                                            {
                                                                reading = false;
                                                                process.SMS = "Finished";

                                                            }
                                                            else if (node.Name == "Message")
                                                            {

                                                            }
                                                            System.Windows.Forms.Application.DoEvents();
                                                        }
                                                    }
                                                    reader.Close();
                                                    server.Close();
                                                });
            
                
            
        }
        public ProcessManagerViewModel(List<string> paths)
        {
            this.Processes = new ObservableCollection<ProcessStateViewModel>();
            foreach (string name in paths)
            {
                ProcessStateViewModel psv = new ProcessStateViewModel(name);
                _processes.Add(psv);
            }
           

           
           
        }
        private void StartServer(string name)
        {
            Task.Factory.StartNew(() =>
            {
                var server = new NamedPipeServerStream(name);
                server.WaitForConnection();
                //StreamReader reader = new StreamReader(server);
                StreamWriter writer = new StreamWriter(server);
                int i = 0;
                while (i <= 100)
                {

                    writer.WriteLine("<Progress>" + i + "</Progress>");
                    writer.Flush();
                    Task.Delay(25).Wait();
                    i++;

                }
                //writer.Close();
                //server.Close();
            });
        }
        public void Notify()
        {
            NotifyOfPropertyChange(() => Processes);
        }
    }
}
