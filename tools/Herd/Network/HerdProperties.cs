/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

namespace Herd.Network
{
    public class XmlTags
    {
        public const string Requirements = "Requirements";
        public const string TargetPlatformRequirements = "Target-Platform";
        public const string TargetPlatformNameAttr = "Name";
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
        public const string Linux64 = "Linux-64";
        public const string Linux32 = "Linux-32";

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
