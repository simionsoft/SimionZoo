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
            Dictionary<IPEndPoint, HerdAgentDescription> agentList;


           
            
            //while(true)
            {
                agentList = m_shepherd.getHerdAgentList();
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
            //Task.Factory.StartNew(fillListOfHerdAgents);
            m_timer.AutoReset = true;
            m_timer.Elapsed += new System.Timers.ElapsedEventHandler(fillListOfHerdAgents);
            m_timer.Start();
           
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Dictionary<IPEndPoint, HerdAgentDescription> agentList;
            

            System.Threading.Thread.Sleep(2000);
            agentList = m_shepherd.getHerdAgentList();
            herdAgentListBox.BeginUpdate();
            herdAgentListBox.Items.Clear();
            foreach (var agent in agentList)
            {
                herdAgentListBox.Items.Add(agent.Key.Address.ToString() + " " + agent.Value.ToString());
            }
            herdAgentListBox.EndUpdate();
        }
    }
}
