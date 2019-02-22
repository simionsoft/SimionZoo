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

using System.Collections.ObjectModel;
using System.IO;
using System.Diagnostics;
using System.Runtime.Serialization;

using Caliburn.Micro;

using Badger.Data;

using Herd.Files;

namespace Badger.ViewModels
{
    [DataContract]
    public class StatsViewModel : PropertyChangedBase
    {
        [DataMember]
        public ObservableCollection<StatViewModel> Stats { get; set; }= new ObservableCollection<StatViewModel>();
        [DataMember]
        public StatViewModel SelectedStat { get; set; } = null;

        private string m_variable;
        [DataMember]
        public string Variable
        {
            get { return m_variable; }
            set { m_variable = value;  NotifyOfPropertyChange(() => Variable); }
        }

        public StatsViewModel(string variableName)
        {
            m_variable = variableName;
        }

        public void addStat(StatViewModel newStat)
        {
            Stats.Add(newStat);
        }

        public void Export(StreamWriter fileWriter, string leftSpace)
        {
            fileWriter.WriteLine(leftSpace + "<" + XMLTags.statVariableTag 
                + " " + XMLTags.nameAttribute + "=\"" + Variable + "\">");
            foreach(StatViewModel stat in Stats)
            {
                fileWriter.WriteLine(leftSpace + "  <" + XMLTags.statVariableItemTag 
                    + " " + XMLTags.groupIdAttribute + "=\"" + stat.ExperimentId + "\" "
                    + XMLTags.trackIdAttribute + "=\"" + stat.TrackId + "\">");
                stat.Export(fileWriter, leftSpace + "    ");
                fileWriter.WriteLine(leftSpace + "  </" + XMLTags.statVariableItemTag + ">");
            }
            fileWriter.WriteLine(leftSpace + "</" + XMLTags.statVariableTag + ">");
        }

        public void VisualizeExperiment()
        {
            Process proc = new Process()
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = Folders.logViewerExePath,
                    Arguments = SelectedStat.LogDescriptorFile,
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    CreateNoWindow = true
                }
            };
            proc.Start();
        }

        public void VisualizeFunctions()
        {
            CaliburnUtility.ShowPopupWindow(new FunctionLogViewModel(SelectedStat.LogDescriptorFile), "Function Log Viewer");
        }

        public void OpenInEditor()
        {
            MainWindowViewModel.Instance.ShowEditorWindow();
            MainWindowViewModel.Instance.EditorWindowVM.LoadExperimentalUnit(SelectedStat.ExperimentalUnitConfigFile);
        }
    }
}
