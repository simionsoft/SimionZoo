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
using System.Collections.Concurrent;

namespace AppXML.ViewModels
{
    public class ProcessManagerViewModel:Caliburn.Micro.PropertyChangedBase
    {
        
        private CancellationTokenSource cts;
        private ObservableCollection<ProcessStateViewModel> _processes;
        private List<Process> ids = new List<Process>();
        private List<object> readers = new List<object>();
        public ObservableCollection<ProcessStateViewModel> Processes
        {
            get { return _processes; }
            set
            {
                _processes = value;
                NotifyOfPropertyChange(() => Processes);
            }
        }
   
        public ProcessManagerViewModel(List<ProcessStateViewModel> processes)
        {
            _processes = new ObservableCollection<ProcessStateViewModel>(processes);
            run();
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
            cts = new CancellationTokenSource();
            ParallelOptions po = new ParallelOptions();
            po.CancellationToken = cts.Token;
           
            
           
            

            var t=Task.Factory.StartNew(() =>
            {
               
                
            Parallel.ForEach(Processes,po, (process) =>
                                                {
                                                    Process p = new Process();
                                                    string name = process.pipeName;
                                                    var server = new NamedPipeServerStream(name);
                                                    using (cts.Token.Register(Thread.CurrentThread.Abort))
                                                    {
                                                        try
                                                        {
                                                            runOneProcess(process,p,server,cts.Token);
                                                        }
                                                        catch (Exception ex)
                                                        {
                                                            if (p != null && !p.HasExited)
                                                            {
                                                                p.Kill();
                                                                p.Dispose();
                                                            }
                                                            if (server != null && server.IsConnected)
                                                            {
                                                                server.Close();
                                                                server.Dispose();
                                                            }
                                                            process.SMS = "ABORTED";
                                                            process.addMessage(ex.StackTrace);
                                         
                                                            
                                                            
                                                               
                                                            
                                                           
                                                        }
                                                        
                                                    }
                                                });
               
            },cts.Token);
            
                
            
        }
        public ProcessManagerViewModel(List<string> paths)
        {
            this.Processes = new ObservableCollection<ProcessStateViewModel>();
            foreach (string name in paths)
            {
                ProcessStateViewModel psv = new ProcessStateViewModel(name);
                _processes.Add(psv);
            }
            run();

           
           
        }
        private void runOneProcess(ProcessStateViewModel process, Process p, NamedPipeServerStream server, CancellationToken ct)
        {
            
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.FileName = Path.Combine(Directory.GetCurrentDirectory(), Models.CApp.EXE);
            startInfo.Arguments = process.Label + " " + process.pipeName;
            //startInfo.CreateNoWindow = true;
            startInfo.WindowStyle = ProcessWindowStyle.Hidden;

            //not to read 23.232 as 23232
            Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;
            //Task.Delay(1000).Wait();
            

            p.StartInfo = startInfo;
            p.Start();
            //Process.Start(startInfo);
            this.ids.Add(p);
            server.WaitForConnection();
            StreamReader reader = new StreamReader(server);
            readers.Add(reader);
            readers.Add(server);
            process.SMS = "Running";
            System.Windows.Forms.Application.DoEvents();
            //bool reading = true;
            while (server.IsConnected)
            {
                string sms = reader.ReadLine();
                XmlDocument xml = new XmlDocument();
                if (sms != null)
                {
                    xml.LoadXml(sms);
                    XmlNode node = xml.DocumentElement;
                    if (node.Name == "Progress")
                    {
                        double progress = Convert.ToDouble(node.InnerText);
                        process.Status = Convert.ToInt32(progress);

                        if (progress == 100.0)
                        {
                            //reading = false;
                            process.SMS = "Finished";

                        }
                    }
                    else if (node.Name == "Message")
                    {
                        process.addMessage(node.InnerText);

                    }
                    System.Windows.Forms.Application.DoEvents();
                }
                else
                {

                }

            }
            //ids.Remove(p);
            reader.Close();
            //readers.Remove(reader);
            server.Close();
            //readers.Remove(server);
        }
       
        internal void closeAll()
        {
            if(cts!=null)
                cts.Cancel();                           

        }
    }
}
