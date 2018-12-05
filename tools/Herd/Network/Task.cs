

namespace Herd.Network
{
    public class HerdTask
    {
        public string Exe;
        public string Arguments;
        public string Pipe;
        public string AuthenticationToken;
        public string Name;
        public HerdTask()
        {
            Name = "";
            Exe = "";
            Arguments = "";
            Pipe = "";
            AuthenticationToken = "";
        }
        public override string ToString()
        {
            string ret = "Task = " + Name + "\n" + "Exe= " + Exe + "\n" + "Arguments= " + Arguments + "\n";
            ret += "Pipe= " + Pipe + "\n" + "Auth.Token= " + AuthenticationToken + "\n";
            return ret;
        }
    }
}
