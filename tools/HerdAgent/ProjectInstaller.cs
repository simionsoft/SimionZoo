
using System.ComponentModel;
using System.Configuration.Install;

using NetFwTypeLib;
using System;
using System.ServiceProcess;

namespace Herd
{
    [RunInstaller(true)]
    public partial class ProjectInstaller : System.Configuration.Install.Installer
    {
        public ProjectInstaller()
        {
            InitializeComponent();
        }

        private void serviceInstaller1_AfterInstall(object sender, InstallEventArgs e)
        {
            INetFwMgr icfMgr = null;
            try
            {
                Type TicfMgr = Type.GetTypeFromProgID("HNetCfg.FwMgr");
                icfMgr = (INetFwMgr)Activator.CreateInstance(TicfMgr);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
                return;
            }

            try
            {
                INetFwProfile profile;
                INetFwOpenPort portClass;
                Type TportClass = Type.GetTypeFromProgID("HNetCfg.FWOpenPort");

                //TCP Exception
                portClass = (INetFwOpenPort)Activator.CreateInstance(TportClass);

                // Get the current profile
                profile = icfMgr.LocalPolicy.CurrentProfile;

                // Set the port properties
                portClass.Scope = NetFwTypeLib.NET_FW_SCOPE_.NET_FW_SCOPE_ALL;
                portClass.Enabled = true;
                portClass.Protocol = NetFwTypeLib.NET_FW_IP_PROTOCOL_.NET_FW_IP_PROTOCOL_TCP;
                portClass.Name = HerdAgent.FirewallExceptionNameTCP;
                portClass.Port = HerdAgent.m_comPortHerd;

                // Add the port to the ICF Permissions List
                profile.GloballyOpenPorts.Add(portClass);

                //UDP Exception
                portClass = (INetFwOpenPort)Activator.CreateInstance(TportClass);

                // Get the current profile
                profile = icfMgr.LocalPolicy.CurrentProfile;

                // Set the port properties
                portClass.Scope = NetFwTypeLib.NET_FW_SCOPE_.NET_FW_SCOPE_ALL;
                portClass.Enabled = true;
                portClass.Protocol = NetFwTypeLib.NET_FW_IP_PROTOCOL_.NET_FW_IP_PROTOCOL_UDP;
                portClass.Name = HerdAgent.FirewallExceptionNameUDP;
                portClass.Port = HerdAgent.m_discoveryPortHerd;

                // Add the port to the ICF Permissions List
                profile.GloballyOpenPorts.Add(portClass);
            }
            finally
            {
                //Run the service we just installed
                using (ServiceController sc = new ServiceController("HerdAgent"))
                {
                    sc.Start();
                }
            }



            ////add the firewall rules
            //var proc1 = new ProcessStartInfo();
            //string command;
            //proc1.UseShellExecute = true;
            //proc1.WorkingDirectory = @"c:\windows\system32";
            //proc1.FileName = @"c:\windows\system32\cmd.exe";
            //proc1.Verb = "runas";
            //proc1.WindowStyle = ProcessWindowStyle.Hidden;

            ////tcp inbound rule
            //command = @"netsh advfirewall firewall add rule name=HerdAgentTCPException protocol=tcp action=allow dir=in service=HerdAgent remoteip=localsubnet localport=" + Herd.HerdAgent.m_comPortHerd;
            //proc1.Arguments = "/c " + command;
            //Process.Start(proc1);
            ////udp inbound rule
            //command = @"netsh advfirewall firewall add rule name=HerdAgentUDPException protocol=udp action=allow dir=in service=HerdAgent remoteip=localsubnet localport=" + Herd.HerdAgent.m_discoveryPortHerd;
            //proc1.Arguments = "/c " + command;
            //Process.Start(proc1);

            ////start the service
            //new ServiceController(serviceInstaller1.ServiceName).Start();
        }

        private void serviceInstaller1_AfterUninstall(object sender, InstallEventArgs e)
        {
            INetFwPolicy2 firewallPolicy = (INetFwPolicy2)Activator.CreateInstance(
                Type.GetTypeFromProgID("HNetCfg.FwPolicy2"));
            firewallPolicy.Rules.Remove(HerdAgent.FirewallExceptionNameTCP);
            firewallPolicy.Rules.Remove(HerdAgent.FirewallExceptionNameUDP);
            //remove the firewall rules
            //var proc1 = new ProcessStartInfo();
            //string command;
            //proc1.UseShellExecute = true;
            //proc1.WorkingDirectory = @"c:\windows\system32";
            //proc1.FileName = @"c:\windows\system32\cmd.exe";
            //proc1.Verb = "runas";
            //proc1.WindowStyle = ProcessWindowStyle.Hidden;

            ////tcp inbound rule
            //command = @"netsh advfirewall firewall delete rule name=HerdAgentTCPException";
            //proc1.Arguments = "/c " + command;
            //Process.Start(proc1);

            ////udp inbound rule
            //command = @"netsh advfirewall firewall delete rule name=HerdAgentUDPException";
            //proc1.Arguments = "/c " + command;
            //Process.Start(proc1);
        }

        private void serviceProcessInstaller1_AfterInstall(object sender, InstallEventArgs e)
        {

        }
    }
}
