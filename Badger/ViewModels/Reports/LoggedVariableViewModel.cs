using System.Collections.Generic;
using Caliburn.Micro;
using Badger.Data;
using System.Collections.ObjectModel;
using System.Globalization;
using System.Windows.Data;
using Badger.Simion;

namespace Badger.ViewModels
{
    public class LoggedVariableViewModel : PropertyChangedBase
    {
        public LoggedVariableViewModel(string name)
        {
            m_name = name;

            ProcessFuncs.Add(ProcessFunc.None);
            ProcessFuncs.Add(ProcessFunc.Abs);

            m_selectedPlotTypes = new ObservableCollection<string>();
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

        public string name
        {
            get { return m_name; }
            set
            {
                m_name = value;
                NotifyOfPropertyChange(() => name);
            }
        }

        private ReportType m_selectedPlotType;
        public ReportType SelectedPlotType
        {
            get { return m_selectedPlotType; }
            set
            {
                m_selectedPlotType = value;
                NotifyOfPropertyChange(() => SelectedPlotType);
            }
        }

        private ObservableCollection<string> m_selectedPlotTypes;
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

        private BindableCollection<string> m_processFuncs = new BindableCollection<string>();
        public BindableCollection<string> ProcessFuncs
        {
            get { return m_processFuncs; }
            set { m_processFuncs = value; NotifyOfPropertyChange(() => ProcessFuncs); }
        }
        public string SelectedProcessFunc { get; set; } = ProcessFunc.None;
    }
}