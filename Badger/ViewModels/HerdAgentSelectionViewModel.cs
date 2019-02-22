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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    class HerdAgentSelectionViewModel: PropertyChangedBase
    {

        //Utility methods
        /// <summary>
        /// Adds in order to a list of ints if input int is not on the list
        /// </summary>
        /// <param name="intList">in-out list where the new integer is added</param>
        /// <param name="newInt">new integer to be added to the list</param>
        void AddInOrder(BindableCollection<int> intList, int newInt)
        {
            if (intList.Contains(newInt))
                return;

            int i = 0;
            while (i < intList.Count - 1 && intList[i] >= newInt) i++;

            if (i <= intList.Count - 1)
                intList.Insert(i, newInt);
            else intList.Add(newInt);
        }

        /// <summary>
        /// Adds in order to a list of strings if input string is not on the list, assuming all strings
        /// are formatted using version numbers or ip addresses (xxx.xxx.xxx.xxx)
        /// </summary>
        /// <param name="stringList">in-out string list</param>
        /// <param name="newString">new string to be added</param>
        void AddInOrder(BindableCollection<string> stringList, string newString)
        {
            if (stringList.Contains(newString))
                return;

            Version newStringVersion = new Version(newString);

            int i = 0;
            while (i < stringList.Count - 1 && new Version(stringList[i]).CompareTo( newStringVersion)>=0) i++;

            if (i <= stringList.Count - 1)
                stringList.Insert(i, newString);
            else stringList.Add(newString);
        }

        //Version
        string m_minVersion = "";
        public string MinVersion
        {
            get { return m_minVersion; }
            set { m_minVersion = value; NotifyOfPropertyChange(() => MinVersion); }
        }
        string m_maxVersion = "";
        public string MaxVersion
        {
            get { return m_maxVersion; }
            set { m_maxVersion = value; NotifyOfPropertyChange(() => MaxVersion); }
        }
        public BindableCollection<string> VersionList { get; set; } = new BindableCollection<string>();

        //NumProcessors
        int m_minNumProcessors = int.MaxValue;
        public int MinNumProcessors
        {
            get { return m_minNumProcessors; }
            set { m_minNumProcessors = value; NotifyOfPropertyChange(() => MinNumProcessors); }
        }
        int m_maxNumProcessors = int.MinValue;
        public int MaxNumProcessors
        {
            get { return m_maxNumProcessors; }
            set { m_maxNumProcessors = value; NotifyOfPropertyChange(() => MaxNumProcessors); }
        }
        public BindableCollection<int> NumProcessorsList { get; set; } = new BindableCollection<int>();

        //IP address
        string m_minIPAddress = "";
        public string MinIPAddress
        {
            get { return m_minIPAddress; }
            set { m_minIPAddress = value; NotifyOfPropertyChange(() => MinIPAddress); }
        }
        string m_maxIPAddress = "";
        public string MaxIPAddress
        {
            get { return m_maxIPAddress; }
            set { m_maxIPAddress = value; NotifyOfPropertyChange(() => MaxIPAddress); }
        }
        public BindableCollection<string> IPAddressList { get; set; } = new BindableCollection<string>();

        BindableCollection<HerdAgentViewModel> m_herdAgents;
        public HerdAgentSelectionViewModel(BindableCollection<HerdAgentViewModel> herdAgents)
        {
            m_herdAgents = herdAgents;

            Version minVersion = new Version("255.255.255.255");
            Version maxVersion = new Version("0.0.0.0");
            Version agentVersion;

            //we use Version to compare ip addresses too
            Version minIPAddress = new Version("255.255.255.255");
            Version maxIPAddress = new Version("0.0.0.0");
            Version agentIPAddress;
            
            foreach(HerdAgentViewModel herdAgent in herdAgents)
            {
                //update minimums and maximums for each property

                //NumProcessors
                AddInOrder(NumProcessorsList, herdAgent.NumProcessors);
                if (herdAgent.NumProcessors < MinNumProcessors)
                    MinNumProcessors = herdAgent.NumProcessors;
                if (herdAgent.NumProcessors > MaxNumProcessors)
                    MaxNumProcessors = herdAgent.NumProcessors;
                //Version
                AddInOrder(VersionList, herdAgent.Version);
                agentVersion = new Version(herdAgent.Version);
                if (minVersion.CompareTo(agentVersion) > 0)
                {
                    MinVersion = herdAgent.Version;
                    minVersion = agentVersion;
                }
                if (maxVersion.CompareTo(agentVersion) < 0)
                {
                    MaxVersion = herdAgent.Version;
                    maxVersion = agentVersion;
                }
                //Ip address
                AddInOrder(IPAddressList, herdAgent.IpAddressString);
                agentIPAddress = new Version(herdAgent.IpAddressString);
                if (minIPAddress.CompareTo(agentIPAddress) > 0)
                {
                    MinIPAddress = herdAgent.IpAddressString;
                    minIPAddress = agentIPAddress;
                }
                if (maxIPAddress.CompareTo(agentIPAddress) < 0)
                {
                    MaxIPAddress = herdAgent.IpAddressString;
                    maxIPAddress = agentIPAddress;
                }
            }
        }

        public void Select()
        {
            foreach(HerdAgentViewModel herdAgent in m_herdAgents)
            {
                if ( herdAgent.NumProcessors >= MinNumProcessors && herdAgent.NumProcessors <= MaxNumProcessors
                    && new Version(herdAgent.Version).CompareTo(new Version(MinVersion))>=0
                    && new Version(herdAgent.Version).CompareTo(new Version(MaxVersion)) <= 0
                    && new Version(herdAgent.IpAddressString).CompareTo(new Version(MinIPAddress)) >= 0
                    && new Version(herdAgent.IpAddressString).CompareTo(new Version(MaxIPAddress)) <= 0 )
                    herdAgent.IsSelected = true;
                else
                    herdAgent.IsSelected = false;
            }
        }
    }
}
