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
using System.Net;
using System.Dynamic;
using System.Windows;
using Caliburn.Micro;
using NetJobTransfer;
using AppXML.Data;
using System.Net.Sockets;
using System.Xml.Linq;

namespace AppXML.ViewModels
{
    public class ProcessManagerViewModel:Caliburn.Micro.PropertyChangedBase
    {
        
        private CancellationTokenSource cts;
        private ObservableCollection<ProcessStateViewModel> _processes;
        private List<Process> ids = new List<Process>();
        private List<object> readers = new List<object>();
        private List<IPEndPoint> canceler;
        public ObservableCollection<ProcessStateViewModel> Processes
        {
            get { return _processes; }
            set
            {
                _processes = value;
                NotifyOfPropertyChange(() => Processes);
            }
        }
        private bool isWorking = false;
   
        public ProcessManagerViewModel(List<ProcessStateViewModel> processes)
        {
            _processes = new ObservableCollection<ProcessStateViewModel>(processes);
            //construct();
        }

        public void run()
        {
            int coreNumbers = Environment.ProcessorCount;
            if(coreNumbers>=_processes.Count)
                run(new List<ProcessStateViewModel>(Processes));
            else
            {
                //antes de nada se lanzan los n primero experimentos
                int index = 0;               
                
                
                    Dictionary<IPEndPoint, int> slaves = null;
                    int totalCores;
                    slaves=Data.Utility.getSlaves(out totalCores);
                    if ((slaves == null || slaves.Count == 0))
                    {
                        
                            var results = Processes.Skip(index).Take(Processes.Count-index);
                            run(results);
                            index = Processes.Count;
                        
                    }
                    else
                    {
                        if (canceler == null)
                            canceler = new List<IPEndPoint>(slaves.Keys);
                        else
                            canceler.Clear();
                        double proportion = (double)Processes.Count/totalCores;
                        if (cts == null)
                            cts = new CancellationTokenSource();
                        ParallelOptions po = new ParallelOptions();
                        po.CancellationToken = cts.Token;
                        Task.Factory.StartNew(() => { 
                        Parallel.ForEach(slaves.Keys, po, (key) =>
                                                {
                                                    if (index == Processes.Count)
                                                    {
                                                        var TcpSocket = new TcpClient();
                                                        //my guess is we don't need this
                                                        //Thread.Sleep(2000);
                                                        TcpSocket.Connect(key.Address, 4444);
                                                        NetworkStream netStream = TcpSocket.GetStream();
                                                        byte[] youAreFree = Encoding.ASCII.GetBytes(CJobDispatcher.m_freeMessage);
                                                        netStream.Write(youAreFree, 0, youAreFree.Length);
                                                        netStream.Close();
                                                        netStream.Dispose();
                                                        TcpSocket.Close();
                
                                                    }
                                                    else
                                                    {
                                                        int cores = slaves[key];
                                                        object o = index;
                                                        Monitor.Enter(o);
                                                        int amount = (int)Math.Floor(cores * proportion);
                                                        if (index + amount > Processes.Count)
                                                            amount = Processes.Count - index;
                                                        IEnumerable<ProcessStateViewModel> myPro = Processes.Skip(index).Take(amount);
                                                        index += amount;
                                                        Monitor.Exit(o);
                                                        using (cts.Token.Register(Thread.CurrentThread.Abort))
                                                        {
                                                            try
                                                            {
                                                                runOneJob(myPro, key, cts.Token);
                                                            }
                                                            catch (Exception ex)
                                                            {
                                                                //stop the job
                                                                Console.WriteLine(ex.StackTrace);

                                                            }

                                                        }
                                                    }
                                                    
                                                });
                        
                        });
                       
                      
                    }
                
               
               
               
            }
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
        private void runOneJob(IEnumerable<ProcessStateViewModel> processes,IPEndPoint endPoint, CancellationToken ct)
        {
            Dictionary<string, ProcessStateViewModel> myPipes = new Dictionary<string, ProcessStateViewModel>();
            Task.Factory.StartNew(() => {
            Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;
            //hay que preparar cada trabajo y lanzarlo
            CJob job = new CJob();
            job.name = "xxxxx";
            job.exeFile = Models.CApp.EXE;
            foreach (ProcessStateViewModel process in processes)
            {
                job.comLineArgs.Add(process.Label + " "+ process.pipeName);
                job.inputFiles.Add(process.Label);
                Utility.getInputsAndOutputs(process.Label, ref job);
                process.SMS="RUNNING REMOTELY";
                myPipes.Add(process.pipeName, process);

            }
            
            Shepherd shepherd;
            
            
            var TcpSocket = new TcpClient();
            //my guess is we don't need to sleep here
            //Thread.Sleep(2000);
            TcpSocket.Connect(endPoint.Address, 4444);
            {
                using (NetworkStream netStream = TcpSocket.GetStream())
                {
                    shepherd = new Shepherd(netStream);
                    shepherd.writeMessage(CJobDispatcher.m_aquireMessage,true);
                    shepherd.SendJobQuery(job);
                    string xmlItem;
                    while(true)
                    {
                        shepherd.read();
                        xmlItem = shepherd.processNextXMLItem();

                        if (xmlItem!="")
                        {
                            XmlDocument doc = new XmlDocument();
                            doc.LoadXml(xmlItem);
                            XmlNode e = doc.DocumentElement;
                            string key = e.Name;
                            XmlNode message = e.FirstChild;
                            if (message.Name == "Progress")
                            {
                                double progress = Convert.ToDouble(message.InnerText);
                                myPipes[key].Status = Convert.ToInt32(progress);
                                if (progress == 100)
                                    myPipes[key].SMS = "The experiment has been completed";
                            }
                            else if (message.Name == "Message")
                            {
                                myPipes[key].addMessage(message.InnerText);
                            }
                            else
                                if (key == XMLStream.m_defaultMessageType
                                && message.InnerText == CJobDispatcher.m_endMessage)
                                break;
                        }
                    }

                    shepherd.ReceiveJobResult();
                    
                }
                TcpSocket.Close();
            }
                
            });
            

        }
        public void run(IEnumerable<ProcessStateViewModel> Processes)
        {
            isWorking = true;
            if(cts==null)
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
            //construct();

           
           
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
            XMLStream xmlStream = new XMLStream();
            string xmlItem;
            XmlDocument xml = new XmlDocument();
            //StreamReader reader = new StreamReader(server);
            //readers.Add(reader); //<- mmmmm esto no sé si se puede quitar, es para probar nada más
            //readers.Add(server);
            process.SMS = "Running";
            System.Windows.Forms.Application.DoEvents();
            //bool reading = true;
            while (server.IsConnected)
            {
                try
                {
                    xmlStream.readFromNamedPipeStream(server);
                    xmlItem = xmlStream.processNextXMLItem();
                    if (xmlItem != "")
                    {
                        xml.LoadXml(xmlItem);
                        XmlNode node = xml.DocumentElement;
                        if (node.Name == "Progress")
                        {
                            double progress = Convert.ToDouble(node.InnerText);
                            process.Status = Convert.ToInt32(progress);

                            if (progress == 100.0)
                                process.SMS = "Finished";
                        }
                        else if (node.Name == "Message")
                        {
                            process.addMessage(node.InnerText);

                        }
                        System.Windows.Forms.Application.DoEvents();
                    }
                }
                catch
                {
                    Console.WriteLine("ahu");
                }

            }
            //ids.Remove(p);
            //reader.Close();
            //readers.Remove(reader);
            server.Close();
            //readers.Remove(server);
        }
       
        internal void closeAll()
        {
            if(cts!=null)
                cts.Cancel();   
            if(canceler!=null)
            {
                foreach(IPEndPoint ip in canceler)
                {
                    UdpClient m_discoverySocket;
                    m_discoverySocket = new UdpClient();
                    var RequestData = Encoding.ASCII.GetBytes("QUIT");
                    m_discoverySocket.Send(RequestData, RequestData.Length, new IPEndPoint(ip.Address, CJobDispatcher.m_discoveryPortHerd));
                }
            }

        }
    }
}
