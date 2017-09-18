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

            ProcessFuncs.Add(DataProcess.None);
            ProcessFuncs.Add(DataProcess.Abs);

            m_selectedPlotTypes = new ObservableCollection<string>();
            m_selectedPlotTypes.CollectionChanged += M_selectedPlotTypes_CollectionChanged;

            EnumDescriptionConverter conv = new EnumDescriptionConverter();
            SelectedPlotTypes.Add((string)((IValueConverter)conv).Convert(PlotType.LastEvaluation,typeof(PlotType),null, CultureInfo.CurrentCulture));
        }

        private void M_selectedPlotTypes_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            NotifyOfPropertyChange(() => SelectedPlotTypes);
            NotifyOfPropertyChange(() => PlaceHolderText);
            SelectedPlotType = PlotType.Undefined;
            EnumDescriptionConverter conv = new EnumDescriptionConverter();
            foreach (var item in m_selectedPlotTypes)
            {
                SelectedPlotType |= (PlotType)((IValueConverter)conv).ConvertBack(item, typeof(PlotType), null, CultureInfo.CurrentCulture);
            }
        }

        private bool m_bIsSelected;

        public bool bIsSelected
        {
            get { return m_bIsSelected; }
            set
            {
                m_bIsSelected = value;
                NotifyOfPropertyChange(() => bIsSelected);

                if (m_parent != null)
                    m_parent.ValidateQuery();
            }
        }

        private bool m_bCheckIsVisible = true;

        public bool bCheckIsVisible
        {
            get { return m_bCheckIsVisible; }
            set
            {
                m_bCheckIsVisible = value;
                NotifyOfPropertyChange(() => bCheckIsVisible);
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

        private PlotType m_selectedPlotType;
        public PlotType SelectedPlotType
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
                NotifyOfPropertyChange("PlaceHolderText");
            }
        }

        public string PlaceHolderText { get { return "Evaluation Type"; } }

        ReportsWindowViewModel m_parent;

        public void setParent(ReportsWindowViewModel parent) { m_parent = parent; }

        private BindableCollection<string> m_processFuncs = new BindableCollection<string>();
        public BindableCollection<string> ProcessFuncs
        {
            get { return m_processFuncs; }
            set { m_processFuncs = value; NotifyOfPropertyChange(() => ProcessFuncs); }
        }
        public string SelectedProcessFunc { get; set; } = DataProcess.None;
    }
}