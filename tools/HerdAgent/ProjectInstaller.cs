
using System.ComponentModel;
using System.Configuration.Install;

using NetFwTypeLib;
using System;
using System.ServiceProcess;

using Herd.Network;

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
        }

        private void serviceInstaller1_AfterUninstall(object sender, InstallEventArgs e)
        {
            INetFwPolicy2 firewallPolicy = (INetFwPolicy2)Activator.CreateInstance(
                Type.GetTypeFromProgID("HNetCfg.FwPolicy2"));
            firewallPolicy.Rules.Remove(HerdAgent.FirewallExceptionNameTCP);
            firewallPolicy.Rules.Remove(HerdAgent.FirewallExceptionNameUDP);
        }

        private void serviceProcessInstaller1_AfterInstall(object sender, InstallEventArgs e)
        {

        }
    }
}
