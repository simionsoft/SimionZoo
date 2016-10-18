
using System.ComponentModel;
using System.Configuration.Install;
using System.ServiceProcess;
using System.Diagnostics;

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
            //add the firewall rules
            var proc1 = new ProcessStartInfo();
            string command;
            proc1.UseShellExecute = true;
            proc1.WorkingDirectory = @"c:\windows\system32";
            proc1.FileName = @"c:\windows\system32\cmd.exe";
            proc1.Verb = "runas";
            proc1.WindowStyle = ProcessWindowStyle.Hidden;

            //tcp inbound rule
            command = @"netsh advfirewall firewall add rule name=HerdAgentTCPException protocol=tcp action=allow dir=in service=HerdAgent remoteip=localsubnet localport=" + Herd.HerdAgent.m_comPortHerd;
            proc1.Arguments = "/c " + command;
            Process.Start(proc1);
            //udp inbound rule
            command = @"netsh advfirewall firewall add rule name=HerdAgentUDPException protocol=udp action=allow dir=in service=HerdAgent remoteip=localsubnet localport=" + Herd.HerdAgent.m_discoveryPortHerd;
            proc1.Arguments = "/c " + command;
            Process.Start(proc1);

            //start the service
            new ServiceController(serviceInstaller1.ServiceName).Start();
        }

        private void serviceInstaller1_AfterUninstall(object sender, InstallEventArgs e)
        {
            //remove the firewall rules
            var proc1 = new ProcessStartInfo();
            string command;
            proc1.UseShellExecute = true;
            proc1.WorkingDirectory = @"c:\windows\system32";
            proc1.FileName = @"c:\windows\system32\cmd.exe";
            proc1.Verb = "runas";
            proc1.WindowStyle = ProcessWindowStyle.Hidden;

            //tcp inbound rule
            command = @"netsh advfirewall firewall delete rule name=HerdAgentTCPException";
            proc1.Arguments = "/c " + command;
            Process.Start(proc1);

            //udp inbound rule
            command = @"netsh advfirewall firewall delete rule name=HerdAgentUDPException";
            proc1.Arguments = "/c " + command;
            Process.Start(proc1);
        }
    }
}
