using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Security.AccessControl;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;

namespace Herd
{
    [Serializable()]
    public class Credentials
    {
        public string UserSID { get; set; }
        public string AuthenticationCode { get; set; }

        public String GetAppDataDirectory()
        {
            const string regKeyFolders = @"HKEY_USERS\<SID>\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders";
            const string regValueAppData = @"AppData";

            return Registry.GetValue(regKeyFolders.Replace("<SID>", UserSID), regValueAppData, null) as string;

        }

        public string GetUsername()
        {
            return new System.Security.Principal.SecurityIdentifier(UserSID).Translate(typeof(System.Security.Principal.NTAccount)).ToString();
        }

        public string GetTempDir()
        {
            return Path.Combine(GetAppDataDirectory(), "RLSimion", "temp");
        }

        public static Credentials Load(String parentPath = null)
        {
            String path;
            if (parentPath == null)
            {
                path = "settings.bin";
            }
            else
            {
                path = Path.Combine(parentPath, "settings.bin");
            }

            if (System.IO.File.Exists(path))
            {

                IFormatter formatter = new BinaryFormatter();
                Stream stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.None);
                Credentials results = (Credentials)formatter.Deserialize(stream);
                stream.Close();
                return results;
            }
            else
            {
                return null;
            }
        }

        public void Save(String parentPath = null)
        {
            String path;
            if (parentPath == null)
            {
                path = "settings.bin";
            }
            else
            {
                path = Path.Combine(parentPath, "settings.bin");
            }

            IFormatter formatter = new BinaryFormatter();
            Stream stream = new FileStream(path, FileMode.Create, FileAccess.Write, FileShare.None); formatter.Serialize(stream, this);
            stream.Close();

            protectAccess(path);
        }

        private static void protectAccess(string path)
        {
            // Way safer than string comparison against "BUILTIN\\Administrators"
            IdentityReference BuiltinAdministrators = new SecurityIdentifier(WellKnownSidType.BuiltinAdministratorsSid, null);

            // Grab ACL from file
            FileSecurity FileACL = File.GetAccessControl(path);

            // Check if correct owner is set
            if (FileACL.GetOwner(typeof(SecurityIdentifier)) != BuiltinAdministrators)
            {
                // If not, make it so!
                FileACL.SetOwner(BuiltinAdministrators);
            }

            //delete all rule
            foreach (FileSystemAccessRule fsRule in FileACL.GetAccessRules(true, false, typeof(SecurityIdentifier)))
            {
                FileACL.RemoveAccessRule(fsRule);
            }

            // Add a single explicit rule to allow FullControl
            FileACL.AddAccessRule(new FileSystemAccessRule(BuiltinAdministrators, FileSystemRights.FullControl, AccessControlType.Allow));

            // Enable protection from inheritance, remove existing inherited rules
            FileACL.SetAccessRuleProtection(true, false);

            // Write ACL back to file
            File.SetAccessControl(path, FileACL);
        }
    }
}
