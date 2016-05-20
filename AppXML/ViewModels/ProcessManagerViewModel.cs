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
using System.Runtime.CompilerServices;

namespace AppXML.ViewModels
{
    public class ProcessManagerViewModel:Caliburn.Micro.PropertyChangedBase
    {
        
        private CancellationTokenSource cts;
        private ObservableCollection<ProcessStateViewModel> _processes;
        private List<Process> ids = new List<Process>();
        private List<object> readers = new List<object>();
        private List<IPEndPoint> canceler;
        public ProcessesWindowViewModel owner;
        private int indexOffset = 0;
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
        }
        private void reRun(IEnumerable<ProcessStateViewModel> processes)
        {
            foreach (ProcessStateViewModel p in processes)
            {
                p.Status = 0;
                p.SMS = "ERROR IN REMOTE HOST. LOOKING FOR A NEW HOST";
            }
            Dictionary<IPEndPoint, int> slaves = null;
            int totalCores;
            int i = 0;
            do
            {
                slaves = Data.Utility.getSlaves(out totalCores);
                Thread.Sleep(3000);
                i++;
                if (i == 2000)
                    run(processes);
            } while (slaves == null ||  slaves.Count <1);
            for (int ii = 1; i < slaves.Keys.Count;i++ )
            {
                var TcpSocket = new TcpClient();
                TcpSocket.Connect(slaves.Keys.ElementAt(ii).Address, NetJobTransfer.CJobDispatcher.m_comPortHerd);
                NetworkStream netStream = TcpSocket.GetStream();
                XMLStream xmlStream = new XMLStream();
                xmlStream.writeMessage(netStream, CJobDispatcher.m_freeMessage, true);
                netStream.Close();
                netStream.Dispose();
                TcpSocket.Close();
            }
                canceler.Add(slaves.Keys.ElementAt(0));
                using (cts.Token.Register(Thread.CurrentThread.Abort))
                {
                    try
                    {
                        this.runOneJob(processes, slaves.Keys.ElementAt(0), cts.Token);

                    }
                    catch (Exception ex)
                    {
                        //stop the job
                        Console.WriteLine(ex.StackTrace);

                    }

                }
            
            
            /*
            ParallelOptions po = new ParallelOptions();
            po.CancellationToken = cts.Token;
            Parallel.For(0, slaves.Count, j => { 
            if(j==0)
            {
                canceler.Add(slaves.Keys.ElementAt(0));
                
            }
            else
            {
               
            }
            
            });*/
           

           

            
            
        }
        public void run()
        {
            int coreNumbers = Environment.ProcessorCount;
            indexOffset = 0;
            if(coreNumbers>=_processes.Count)
                run(new List<ProcessStateViewModel>(Processes));
            else
            {
                //antes de nada se lanzan los n primero experimentos
                
                
                
                    Dictionary<IPEndPoint, int> slaves = null;
                    int totalCores;
                    slaves=Data.Utility.getSlaves(out totalCores);
                    if ((slaves == null || slaves.Count == 0))
                    {
                        
                            //var results = Processes.Skip(index).Take(Processes.Count-index);
                            run(Processes);
                            //index = Processes.Count;
                        
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
                        //Task y =Task.Factory.StartNew(() => { 
                        Parallel.ForEach(slaves.Keys, po, (key) =>
                           // foreach(IPEndPoint key in slaves.Keys)
                                                {
                                                    if (indexOffset == Processes.Count)
                                                    {
                                                        var TcpSocket = new TcpClient();
                                                        //my guess is we don't need this
                                                        //Thread.Sleep(2000);
                                                        TcpSocket.Connect(key.Address, NetJobTransfer.CJobDispatcher.m_comPortHerd);
                                                        NetworkStream netStream = TcpSocket.GetStream();
                                                        XMLStream xmlStream = new XMLStream();
                                                        xmlStream.writeMessage(netStream, CJobDispatcher.m_freeMessage, true);
                                                        //byte[] youAreFree = Encoding.ASCII.GetBytes(CJobDispatcher.m_freeMessage);
                                                        //netStream.Write(youAreFree, 0, youAreFree.Length);
                                                        netStream.Close();
                                                        netStream.Dispose();
                                                        TcpSocket.Close();
                
                                                    }
                                                    else
                                                    {
                                                        int cores = slaves[key];
                                                        //object o = index;
                                                        //Monitor.Enter(o);
                                                        int amount = (int)Math.Ceiling(cores * proportion);
                                                        if (indexOffset + amount > Processes.Count)
                                                            amount = Processes.Count - indexOffset;
                                                        IEnumerable<ProcessStateViewModel> myPro = getMines(amount);
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
                                                    
                                                }
                        
                                                );
                        
                      
                       
                     //   },cts.Token);
                      
                      
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
        [MethodImpl(MethodImplOptions.Synchronized)]
        private IEnumerable<ProcessStateViewModel> getMines(int cores)
        {
            IEnumerable<ProcessStateViewModel> myPro = Processes.Skip(indexOffset).Take(cores);
            indexOffset += cores;
            return myPro;
        }
        private void runOneJob(IEnumerable<ProcessStateViewModel> processes,IPEndPoint endPoint, CancellationToken ct)
        {

            Dictionary<string, ProcessStateViewModel> myPipes = new Dictionary<string, ProcessStateViewModel>();
            Task.Factory.StartNew(() => {
                try
                {

               
            Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;
            //hay que preparar cada trabajo y lanzarlo
            CJob job = new CJob();
            job.name = "xxxxx";
            job.exeFile = Models.CApp.EXE;
            if(Models.CApp.pre!=null)
            {
                foreach (string prerec in Models.CApp.pre)
                    job.inputFiles.Add(prerec);
            }
            foreach (ProcessStateViewModel process in processes)
            {
                job.comLineArgs.Add(process.Label + " "+ process.pipeName);
                job.inputFiles.Add(process.Label);
                Utility.getInputsAndOutputs(process.Label, ref job);
                //process.SMS="RUNNING REMOTELY";
                myPipes.Add(process.pipeName, process);

            }

            Shepherd shepherd = null; 
            
            
            var TcpSocket = new TcpClient();
            //my guess is we don't need to sleep here
            //Thread.Sleep(2000);
            TcpSocket.Connect(endPoint.Address, NetJobTransfer.CJobDispatcher.m_comPortHerd);
            {
                using (NetworkStream netStream = TcpSocket.GetStream())
                {
                    shepherd = new Shepherd(TcpSocket,netStream);
                    XMLStream xmlStream = new XMLStream();
                    xmlStream.writeMessage(netStream,CJobDispatcher.m_aquireMessage,true);
                    foreach (ProcessStateViewModel p in processes)
                        p.SMS = "DISPATCHING FILES";
                    shepherd.SendJobQuery(job);
                    foreach (ProcessStateViewModel p in processes)
                        p.SMS = "RUNNING";
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
                                   myPipes[key].SMS = "EXPERIMENT IS FINISHED. WAITING TO SEND FILES";
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
                    foreach (ProcessStateViewModel p in processes)
                        p.SMS = "RECEIVING FILES";
                    shepherd.ReceiveJobResult();
                    
                    foreach (ProcessStateViewModel p in processes)
                    {
                        if (p.Status == 100)
                            p.SMS = "FILES RECEIVED";
                        else
                            p.SMS = "ERROR";
                    }
                    
                }
                TcpSocket.Close();
                owner.isFinished(processes.Count());
            }
                 }
                catch(Exception ex)
                {
                    //to do: aqui salta cuando hay cualquier problema. Si hay problema hay que volver a lanzarlo
                    //mandar a cualquier maquina que este libre
                    this.reRun(myPipes.Values);
                    Console.WriteLine(ex.StackTrace);
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


            p.StartInfo = startInfo;
            p.Start();

            this.ids.Add(p);
            
            server.WaitForConnection();

            XMLStream xmlStream = new XMLStream();
            string xmlItem;
            XmlDocument xml = new XmlDocument();

            process.SMS = "Running";
            System.Windows.Forms.Application.DoEvents();

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
                        else if (node.Name == "Error")
                        {
                            process.addMessage(node.InnerText);
                            process.SMS = "ERROR";
                        }
                        System.Windows.Forms.Application.DoEvents();
                    }
                }
                catch
                {
                    Console.WriteLine("ahu");
                }

            }
            server.Close();
            
            owner.isFinished(1);
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
