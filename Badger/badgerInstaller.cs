using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using Badger.Data;
using System.Security.AccessControl;

namespace Badger
{
    [RunInstaller(true)]
    public partial class badgerInstaller : System.Configuration.Install.Installer
    {
        public badgerInstaller()
        {
            InitializeComponent();
        }

        public override void Install(IDictionary stateSaver)
        {
            base.Install(stateSaver);
            string installDir = this.Context.Parameters["installDir"];

            foreach (string dir in new string[] { SimionFileData.experimentDir
                                                , SimionFileData.imageDir, SimionFileData.binDir})
            {
                string targetDir= Path.Combine(installDir,dir);
                DirectoryInfo dirInfo = new DirectoryInfo(targetDir);

                // Now apply user access permissions to the folder
                if (dirInfo != null)
                {
                    DirectorySecurity security = dirInfo.GetAccessControl();
                    security.AddAccessRule(new FileSystemAccessRule("Everyone", FileSystemRights.FullControl, InheritanceFlags.ContainerInherit, PropagationFlags.None, AccessControlType.Allow));
                    security.AddAccessRule(new FileSystemAccessRule("Everyone", FileSystemRights.FullControl, InheritanceFlags.ObjectInherit, PropagationFlags.None, AccessControlType.Allow));
                    dirInfo.SetAccessControl(security);
                }
            }
        }
    }
}
