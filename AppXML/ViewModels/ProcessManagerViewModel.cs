using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

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
            foreach (ProcessStateViewModel process in Processes)
            {


                //string[] tmp = (myList[0].Split('/'));
                ///string pipeName = @"preuba";
                /*StartServer();
                Task.Delay(1000).Wait();
                var client = new NamedPipeClientStream("PipesOfPiece");
                client.Connect(4000);
                StreamReader reader = new StreamReader(client);*/
                process.SMS = "Running";
                /*Notify();
                bool reading = true;
                while (reading)
                {
                    string sms = reader.ReadLine();
                    XmlDocument xml = new XmlDocument();
                    xml.LoadXml(sms);
                    XmlNode node = xml.DocumentElement;
                    if (node.Name == "Progress")
                    {
                        int progress = Convert.ToInt32(node.InnerText);
                        process.Status = progress;
                        //pwvm.Notify();
                        if (progress == 100)
                            reading = false;
                    }
                    else if (node.Name == "Message")
                    {

                    }
                }

                //       });

                */
            }
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
        private void StartServer()
        {
            Task.Factory.StartNew(() =>
            {
                var server = new NamedPipeServerStream("PipesOfPiece");
                server.WaitForConnection();
                //StreamReader reader = new StreamReader(server);
                StreamWriter writer = new StreamWriter(server);
                int i = 0;
                while (i <= 100)
                {

                    writer.WriteLine("<Progress>" + i + "</Progress>");
                    writer.Flush();
                    Task.Delay(50).Wait();
                    i++;

                }
                writer.Close();
                server.Close();
            });
        }
        public void Notify()
        {
            NotifyOfPropertyChange(() => Processes);
        }
    }
}
