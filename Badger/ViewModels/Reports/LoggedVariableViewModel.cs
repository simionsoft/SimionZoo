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

using System.Runtime.Serialization;
using System.Collections.ObjectModel;
using System.Globalization;
using System.Windows.Data;

using Caliburn.Micro;

using Badger.Data;



namespace Badger.ViewModels
{
    [DataContract]
    public class LoggedVariableViewModel : PropertyChangedBase
    {
        public LoggedVariableViewModel(string name)
        {
            m_name = name;

            m_selectedPlotTypes.CollectionChanged += M_selectedPlotTypes_CollectionChanged;

            EnumDescriptionConverter conv = new EnumDescriptionConverter();
            SelectedPlotTypes.Add((string)((IValueConverter)conv).Convert(ReportType.LastEvaluation,typeof(ReportType),null, CultureInfo.CurrentCulture));
        }

        private void M_selectedPlotTypes_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            NotifyOfPropertyChange(() => SelectedPlotTypes);
            NotifyOfPropertyChange(() => PlaceholderText);
            SelectedPlotType = ReportType.Undefined;
            EnumDescriptionConverter conv = new EnumDescriptionConverter();
            foreach (var item in m_selectedPlotTypes)
            {
                SelectedPlotType |= (ReportType)((IValueConverter)conv).ConvertBack(item, typeof(ReportType), null, CultureInfo.CurrentCulture);
            }
        }

        private bool m_bIsSelected;
        [DataMember]
        public bool IsSelected
        {
            get { return m_bIsSelected; }
            set
            {
                m_bIsSelected = value;
                NotifyOfPropertyChange(() => IsSelected);
            }
        }

        private bool m_bIsCheckVisible = true;
        [DataMember]
        public bool IsCheckVisible
        {
            get { return m_bIsCheckVisible; }
            set
            {
                m_bIsCheckVisible = value;
                NotifyOfPropertyChange(() => IsCheckVisible);
            }
        }

        private string m_name;
        [DataMember]
        public string VariableName
        {
            get { return m_name; }
            set
            {
                m_name = value;
                NotifyOfPropertyChange(() => VariableName);
            }
        }

        private ReportType m_selectedPlotType;
        [DataMember]
        public ReportType SelectedPlotType
        {
            get { return m_selectedPlotType; }
            set
            {
                m_selectedPlotType = value;
                NotifyOfPropertyChange(() => SelectedPlotType);
            }
        }

        private ObservableCollection<string> m_selectedPlotTypes = new ObservableCollection<string>();
        [DataMember]
        public ObservableCollection<string> SelectedPlotTypes
        {
            get { return m_selectedPlotTypes; }
            set
            {
                m_selectedPlotTypes = value;
                NotifyOfPropertyChange(() => SelectedPlotTypes);
                NotifyOfPropertyChange(()=> PlaceholderText);
            }
        }

        public string PlaceholderText { get { return "Evaluation Type"; } }

        ReportsWindowViewModel m_parent;

        public void setParent(ReportsWindowViewModel parent) { m_parent = parent; }

        [DataMember]
        public BindableCollection<string> ProcessFuncs { get; set;} = new BindableCollection<string>() { ProcessFunc.None, ProcessFunc.Abs };

        [DataMember]
        public string SelectedProcessFunc { get; set; } = ProcessFunc.None;

        /// <summary>
        /// Sets the notifying property. Needs to be used after loading a view model from a file
        /// </summary>
        /// <param name="notifying">the value to be set</param>
        public void SetNotifying(bool notifying)
        {
            IsNotifying = notifying;
            m_selectedPlotTypes.CollectionChanged += M_selectedPlotTypes_CollectionChanged;
        }
    }
}