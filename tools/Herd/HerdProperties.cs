namespace Herd
{
    public class XmlTags
    {
        public const string Requirements = "Requirements";
        public const string Input = "Input-File";
        public const string Output = "Output-File";
        public const string Architecture = "Architecture";
        public const string NumCPUCores = "NumCPUCores";
        public const string Exe = "Exe";
        public const string Include = "Include";

        public const string HerdAgentDescription = "HerdAgent";

        public const string Version = "Version";

        public const string RenameAttr = "Rename";
    }
    public class PropValues
    {
        public const string Win64 = "Win-64";
        public const string Win32 = "Win-32";
        public const string Unix64 = "Unix-64";
        public const string Unix32 = "Unix-32";

        public const string AllCPUCores = "*";
        public const string None = "N/A";

        public const string StateAvailable = "available";
        public const string StateBusy = "busy";
        public const string StateCancelling = "cancelling";
    }
    public class Deprecated
    {
        public const string NumCPUCores = "NumberOfProcessors";
        public const string CUDA = "CudaInfo";
        public const string ProcessorArchitecture = "ProcessorArchitecture";
    }
    public class PropNames
    {
        //properties
        public const string NumCPUCores = "NumCPUCores";
        public const string Architecture = XmlTags.Architecture;
        public const string CUDA = "CUDA";


        public const string HerdAgentVersion = "HerdAgentVersion";
        public const string ProcessorId = "ProccesorId";
        
        public const string State = "State";
        public const string TotalMemory = "Memory";

        public const string ProcessorLoad = "ProcessorLoad";

    }
}
