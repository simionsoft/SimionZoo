using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Herd;
using System.Net.Sockets;
using System.Net;
namespace HerdShepherd
{
    public partial class Form1 : Form
    {
        const int m_updateTime = 5000;
        System.Timers.Timer m_timer;
        Shepherd m_shepherd;

        public Form1()
        {
            InitializeComponent();
        }

        private void fillListOfHerdAgents(object sender, System.Timers.ElapsedEventArgs e)
        {
            m_shepherd.sendBroadcastHerdAgentQuery();
          //  Dictionary<IPEndPoint, HerdAgentDescription> agentList;


           
            
            //while(true)
            {
              //  agentList = m_shepherd.getHerdAgentList();
                //herdAgentListBox.BeginUpdate();
                //foreach(var agent in agentList)
                //{
                //    herdAgentListBox.Items.Add(agent.Key.ToString() + " " + agent.Value.ToString());
                //}
                //herdAgentListBox.EndUpdate();
                //System.Threading.Thread.Sleep(1000);
            }
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            m_shepherd = new Shepherd();

            m_timer = new System.Timers.Timer(m_updateTime);
            m_shepherd.sendBroadcastHerdAgentQuery();
            m_shepherd.beginListeningHerdAgentQueryResponses();
           // m_shepherd.setLogMessageHandler(outputTextBox.AppendText);
            //Task.Factory.StartNew(fillListOfHerdAgents);
            m_timer.AutoReset = true;
            m_timer.Elapsed += new System.Timers.ElapsedEventHandler(fillListOfHerdAgents);
            m_timer.Start();
           
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Dictionary<IPEndPoint, HerdAgentDescription> agentList=
                new Dictionary<IPEndPoint,HerdAgentDescription>();
            

           // System.Threading.Thread.Sleep(2000);
            m_shepherd.getHerdAgentList(ref agentList);
            herdAgentListBox.BeginUpdate();
            herdAgentListBox.Items.Clear();
            foreach (var agent in agentList)
            {
                herdAgentListBox.Items.Add(agent.Key.Address.ToString() + " " + agent.Value.ToString());
            }
            herdAgentListBox.EndUpdate();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            CJob updateJob= new CJob();
            CTask updateTask = new CTask();
            updateTask.name = "RunServiceUpdater";
            updateTask.arguments = "";
            updateTask.pipe = "";
            updateTask.exe = "../Debug/HerdAgentUnattendedUpdater.msi";
            updateJob.name = "HerdAgentServiceUpdate";
            updateJob.tasks.Add(updateTask);
            updateJob.inputFiles.Add(updateTask.exe);
            Dictionary<IPEndPoint, HerdAgentDescription> agentList=
                new Dictionary<IPEndPoint,HerdAgentDescription>();
            m_shepherd.getHerdAgentList(ref agentList);
           Task.Factory.StartNew(() =>
            {
                foreach (var agent in agentList)
                {
                    if (agent.Value.getProperty("State") == "available")
                    {
                        m_shepherd.connectToHerdAgent(agent.Key);
                        m_shepherd.SendJobQuery(updateJob);
                        while (true)
                        {
                            m_shepherd.read();
                            string xmlItem = m_shepherd.m_xmlStream.processNextXMLItem();
                            string xmlMessageType = m_shepherd.m_xmlStream.getLastXMLItemTag();
                            string xmlMessage = m_shepherd.m_xmlStream.getLastXMLItemContent();
                            if (xmlMessageType == XMLStream.m_defaultMessageType)
                            {
                                if (xmlMessage == CJobDispatcher.m_endMessage)
                                {
                                    //outputTextBox.AppendText(agent.Key.Address + ": Update correct");
                                    m_shepherd.ReceiveJobResult();
                                    //outputTextBox.AppendText(agent.Key.Address + ": Job finished");
                                    break;
                                }
                                else if (xmlMessage == CJobDispatcher.m_errorMessage)
                                {
                                    //outputTextBox.AppendText(agent.Key.Address + ": Update error");
                                    break;
                                }
                                //else outputTextBox.AppendText("can't understand message: " + xmlMessage);
                            }
                        }
                        m_shepherd.disconnect();
                    }
                }
            });
        }
    }
}
