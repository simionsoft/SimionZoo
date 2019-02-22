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
