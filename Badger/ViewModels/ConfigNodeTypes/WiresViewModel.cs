using Caliburn.Micro;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace Badger.ViewModels
{
    class WireViewModel : PropertyChangedBase
    {
        WiresViewModel m_parent;

        string m_name;
        public string Name
        {
            get { return m_name; }
            set { m_name = value; NotifyOfPropertyChange(() => Name); }
        }

        public WireViewModel(string name, WiresViewModel parent)
        {
            m_parent = parent;
            Name = name;
        }

        public void Remove()
        {
            m_parent.RemoveWire(this);
        }
    }

    class WiresViewModel : Screen
    {
        ExperimentViewModel m_parent;

        private ObservableCollection<WireViewModel> m_wires = new ObservableCollection<WireViewModel>();
        public ObservableCollection<WireViewModel> Wires { get { return m_wires; } set { m_wires = value; NotifyOfPropertyChange(() => Wires); } }

        public WiresViewModel(ExperimentViewModel parent)
        {
            m_parent = parent;
        }

        public void AddWire()
        {
            int numWires = m_wires.Count;
            Wires.Add(new WireViewModel("Wire-" + numWires.ToString(), this));
        }

        public void RemoveWire(WireViewModel wire)
        {
            Wires.Remove(wire);
        }

        public void GetWireNames(ref ObservableCollection<string> outList)
        {
            if (outList!=null)
            {
                foreach (WireViewModel wire in m_wires)
                    outList.Add(wire.Name);
            }
        }

        void OnWiresChanged()
        {
            m_parent.UpdateWorldReferences();
        }

        /// <summary>
        ///     Updates references to world references
        /// </summary>
        /// <param name="close"></param>
        protected override void OnDeactivate(bool close)
        {
            if (close)
                OnWiresChanged();
            base.OnDeactivate(close);
        }
    }
}
