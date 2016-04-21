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
        private List<int> ids = new List<int>();
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
            Task.Factory.StartNew(() =>
            {
               
                
            Parallel.ForEach(Processes, (process) =>
            //foreach(ProcessStateViewModel process in Processes)
                                                {
                                                     try
                                                     {
                                                        string name = process.pipeName ;
                                                        Process p = new Process();
                                                        ProcessStartInfo startInfo = new ProcessStartInfo();
                                                        startInfo.FileName = Path.Combine(Directory.GetCurrentDirectory(),Models.CApp.EXE);
                                                        startInfo.Arguments = process.Label + " " + process.pipeName;
                                                        //startInfo.CreateNoWindow = true;
                                                        startInfo.WindowStyle = ProcessWindowStyle.Hidden;

                                                        //not to read 23.232 as 23232
                                                        Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;
                                                        //Task.Delay(1000).Wait();
                                                        var server = new NamedPipeServerStream(name);
                                                    
                                                        p.StartInfo = startInfo;
                                                        p.Start();
                                                        //Process.Start(startInfo);
                                                        this.ids.Add(p.Id);
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
                                                        ids.Remove(p.Id);
                                                        reader.Close();
                                                        readers.Remove(reader);
                                                        server.Close();
                                                        readers.Remove(server);
                                                     }
                                                     catch (Exception ex)
                                                     {
                                                         process.SMS = "ABORTED";
                                                         process.addMessage(ex.StackTrace);
                                                     }
                                                });
               
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
            run();

           
           
        }
   
       
        internal void closeAll()
        {

           
               
                    foreach(int id in ids)
                    {
                        try
                        {
                            Process p = Process.GetProcessById(id);
                            if (p != null && !p.HasExited)
                              p.Kill();
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine(ex.StackTrace);
                        }
                    }
                    ids.Clear();
                    foreach(object item in readers)
                    {
                        if(item is StreamReader)
                            (item as StreamReader).Close();
                        if (item is NamedPipeServerStream)
                            (item as NamedPipeServerStream).Close();
                    }

               
            

        }
    }
}
