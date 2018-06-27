using Badger.Simion;
using Caliburn.Micro;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;

namespace Badger.ViewModels
{
    public class WireViewModel : PropertyChangedBase
    {
        WiresViewModel m_parent;

        string m_name;
        public string Name
        {
            get { return m_name; }
            set { m_name = value; NotifyOfPropertyChange(() => Name); }
        }

        double m_minimum = 0.0;
        double m_maximum = 0.0;
        public double Minimum {get{return m_minimum;} set{m_minimum= value;NotifyOfPropertyChange(() => Minimum); } }
        public double Maximum { get { return m_maximum; } set { m_maximum = value; NotifyOfPropertyChange(() => Maximum); } }

        bool m_bLimit = false;
        public bool Limit { get { return m_bLimit; } set { m_bLimit = value;NotifyOfPropertyChange(() => Limit); } }

        public WireViewModel(string name, WiresViewModel parent)
        {
            m_parent = parent;
            Name = name;
        }
        public WireViewModel(string name, WiresViewModel parent, double minimum, double maximum)
        {
            m_parent = parent;
            Name = name;
            Minimum = minimum;
            Maximum = maximum;
            Limit = true;
        }

        public void Remove()
        {
            m_parent.RemoveWire(this);
        }
    }

    public class WiresViewModel : Screen
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

        public void AddWire(string name)
        {
            foreach (WireViewModel wire in Wires)
            {
                if (wire.Name == name)
                    return;
            }
            Wires.Add(new WireViewModel(name, this));
        }

        public void AddWire(string name, double minimum, double maximum)
        {
            foreach (WireViewModel wire in Wires)
            {
                if (wire.Name == name)
                {
                    //set the limits regardless of the previous limits
                    wire.Minimum = minimum;
                    wire.Maximum = maximum;
                    wire.Limit = true;
                    return;
                }
            }
            Wires.Add(new WireViewModel(name, this, minimum, maximum));
        }

        public void RemoveWire(WireViewModel wire)
        {
            Wires.Remove(wire);
        }

        public List<string> GetWireNames()
        {
            List<string> outList = new List<string>();
            foreach (WireViewModel wire in m_wires)
                outList.Add(wire.Name);
            return outList;
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
