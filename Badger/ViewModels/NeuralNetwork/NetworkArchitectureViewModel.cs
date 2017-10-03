using Caliburn.Micro;
using Badger.Data.NeuralNetwork;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.ViewModels.NeuralNetwork
{
    class NetworkArchitectureViewModel : PropertyChangedBase
    {
        public NetworkArchitecture NetworkArchitectureData { get; }
        public NetworkArchitectureViewModel(NetworkArchitecture networkArchitecture)
        {
            NetworkArchitectureData = networkArchitecture;
            Chains = new ObservableCollection<ChainViewModel>();
            Chains.CollectionChanged += Chains_CollectionChanged;
            refreshChains();
        }

        private void Chains_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            if (_ignoreChainsChanges)
                return;

            switch (e.Action)
            {
                case NotifyCollectionChangedAction.Add:
                    foreach (ChainViewModel item in e.NewItems)
                    {
                        if (!NetworkArchitectureData.Chains.Contains(item.ChainData))
                            NetworkArchitectureData.Chains.Add(item.ChainData);
                    }
                    break;

                case NotifyCollectionChangedAction.Remove:
                case NotifyCollectionChangedAction.Reset:
                    foreach (ChainViewModel item in e.OldItems)
                    {
                        if (NetworkArchitectureData.Chains.Contains(item.ChainData))
                            NetworkArchitectureData.Chains.Remove(item.ChainData);
                    }
                    break;
            }
        }

        private bool _ignoreChainsChanges = false;
        private void refreshChains()
        {
            _ignoreChainsChanges = true;
            Chains.Clear();
            foreach (var chain in NetworkArchitectureData.Chains)
                Chains.Add(new ChainViewModel(chain, this));
            _ignoreChainsChanges = false;
        }

        public ObservableCollection<ChainViewModel> Chains { get; }

        public void AddChain()
        {
            Chains.Add(new ChainViewModel(new Chain(NetworkArchitectureData, "Chain"), this));
        }
        public void ValidateInputCompatibility()
        {
            foreach (var item in Chains)
                item.ValidateInputCompatibility();
        }
    }
}
