using Caliburn.Micro;
using Badger.Data.NeuralNetwork;
using System.Collections.ObjectModel;
using System.Collections.Specialized;

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
                        int index = 0;
                        if (!NetworkArchitectureData.Chains.Contains(item.ChainData))
                            NetworkArchitectureData.Chains.Insert(e.NewStartingIndex + index++, item.ChainData);
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
