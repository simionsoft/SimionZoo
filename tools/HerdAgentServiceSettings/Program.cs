using Herd;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;

namespace HerdAgentServiceSettings
{
    class Program
    {
        static void Main(string[] args)
        {
            System.IO.File.WriteAllText(@"C:\Users\Roland\Desktop\testtesttest.log", String.Join("\n", args));

            Console.WriteLine("Setting up the Herdagent service...");
            Console.WriteLine("You need to specify the user in whichs context all experiments will be executed");
            string username = "";

            while (String.IsNullOrEmpty(username) || String.IsNullOrWhiteSpace(username))
            {
                Console.Write("Enter the username:");
                username = Console.ReadLine();
            }

            Console.WriteLine("You need to specify a passphrase to authenticate future communications.");
            string pass = "";

            while (true)
            {
                Console.Write("Enter the passphrase:");
                pass = Console.ReadLine();
                Console.Write("Repeat the passphrase:");
                if (pass == Console.ReadLine())
                    break;
                Console.WriteLine("Repetition does not match. Please repeat!");
            }

            Console.WriteLine("Saving settings...");
            Credentials cred = new Credentials();
            cred.AuthenticationCode = pass;
            cred.UserSID = getSID(username);
            cred.Save(Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location));
            Console.WriteLine("Settings saved.");
            Console.ReadKey();
        }

        private static string getSID(string username)
        {
            NTAccount f = new NTAccount(username);
            SecurityIdentifier s = (SecurityIdentifier)f.Translate(typeof(SecurityIdentifier));
            return s.ToString();
        }
    }
}
