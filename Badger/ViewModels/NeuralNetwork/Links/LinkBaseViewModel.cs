using Caliburn.Micro;
using Badger.Data.NeuralNetwork.Links;
using Badger.ViewModels.NeuralNetwork.Parameters;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.ViewModels.NeuralNetwork.Links
{
    class LinkBaseViewModel : PropertyChangedBase
    {
        public LinkBase LinkBaseData { get; }
        private ChainViewModel _parent;
        public LinkBaseViewModel(LinkBase linkBase, ChainViewModel parent)
        {
            LinkBaseData = linkBase;
            _parent = parent;

            Parameters = new ObservableCollection<ParameterBaseViewModel>();
            foreach (var parameter in LinkBaseData.Parameters)
                Parameters.Add(ParameterBaseViewModel.CreateInstance(parameter, this));
        }

        public string ID
        {
            get { return LinkBaseData.ID; }
            set
            {
                LinkBaseData.ID = value;
                NotifyOfPropertyChange(() => ID);
            }
        }

        public string Name
        {
            get { return LinkBaseData.Name; }
            set
            {
                LinkBaseData.Name = value;
                NotifyOfPropertyChange(() => Name);
            }
        }

        public void Delete()
        {
            _parent.ChainLinks.Remove(this);
        }

        public string TypeName { get { return LinkBaseData.TypeName; } }

        public bool IsInputCompatible
        {
            get { return LinkBaseData.IsInputCompatible; }
        }

        public ObservableCollection<ParameterBaseViewModel> Parameters { get; set; }

        public void ValidateInputCompatibility()
        {
            NotifyOfPropertyChange(() => IsInputCompatible);
        }
    }
}
