using Caliburn.Micro;
using Badger.Data.NeuralNetwork;
using Badger.ViewModels.NeuralNetwork.Links;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GongSolutions.Wpf.DragDrop;
using Badger.Data.NeuralNetwork.Links;
using System.Windows;
using System.Collections.Specialized;
using Badger.ViewModels.NeuralNetwork.Windows;
using Badger.Data.NeuralNetwork.Parameters;
using Badger.Utils;
using Badger.Data;

namespace Badger.ViewModels.NeuralNetwork
{
    class ChainViewModel : PropertyChangedBase
    {
        public Chain ChainData { get; }
        private NetworkArchitectureViewModel _parent;
        public ChainViewModel(Chain chain, NetworkArchitectureViewModel parent)
        {
            ChainData = chain;
            _parent = parent;

            ChainLinks = new ObservableCollection<LinkBaseViewModel>();
            ChainLinks.CollectionChanged += ChainLinks_CollectionChanged;
            refreshLinks();
        }

        private void ChainLinks_CollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            if (_ignoreChainLinksChanges)
                return;

            switch (e.Action)
            {
                case NotifyCollectionChangedAction.Add:
                    foreach (LinkBaseViewModel item in e.NewItems)
                    {
                        if (!ChainData.ChainLinks.Contains(item.LinkBaseData))
                            ChainData.ChainLinks.Add(item.LinkBaseData);
                    }
                    break;

                case NotifyCollectionChangedAction.Remove:
                case NotifyCollectionChangedAction.Reset:
                    foreach (LinkBaseViewModel item in e.OldItems)
                    {
                        if (ChainData.ChainLinks.Contains(item.LinkBaseData))
                            ChainData.ChainLinks.Remove(item.LinkBaseData);
                    }
                    break;
            }
        }

        public void ValidateInputCompatibility()
        {
            foreach (var item in ChainLinks)
                item.ValidateInputCompatibility();
        }

        public ObservableCollection<LinkBaseViewModel> ChainLinks { get; }

        private bool _ignoreChainLinksChanges = false;
        private void refreshLinks()
        {
            _ignoreChainLinksChanges = true;
            ChainLinks.Clear();
            foreach (var link in ChainData.ChainLinks)
                ChainLinks.Add(new LinkBaseViewModel(link, this));
            _ignoreChainLinksChanges = false;
        }

        public void AddChainLink()
        {
            var windowManager = new WindowManager();
            var context = new AddLinkWindowViewModel();
            windowManager.ShowDialog(context);

            if (context.Result.HasValue)
            {
                int insertIndex = ChainLinks.Count;
                LinkBase link;
                switch (context.Result.Value)
                {
                    case LinkType.InputLayer:
                        if (ChainLinks.Count > 0)
                        {
                            if (ChainData.ChainLinks.CountListType(typeof(InputLayer)) > 0)
                            {
                                MessageBox.Show("Only one Input Layer is allowed (or usefull) per chain.");
                                return;
                            }
                        }
                        insertIndex = 0;
                        link = new InputLayer(ChainData, String.Format("Input Layer"));
                        //TODO: Fix
                        ((InputDataParameter)link.Parameters[0]).InputDataValue = _parent.NetworkArchitectureData.Problem.Inputs[0];
                        break;

                    case LinkType.ActivationLayer:
                        link = new ActivationLayer(ChainData, String.Format("Activation Layer"));
                        break;

                    case LinkType.Convolution1DLayer:
                        link = new Convolution1DLayer(ChainData, String.Format("1D Convolution Layer"));
                        break;

                    case LinkType.Convolution2DLayer:
                        link = new Convolution2DLayer(ChainData, String.Format("2D Convolution Layer"));
                        break;

                    case LinkType.Convolution3DLayer:
                        link = new Convolution3DLayer(ChainData, String.Format("3D Convolution Layer"));
                        break;

                    default:
                    case LinkType.DenseLayer:
                        link = new DenseLayer(ChainData, String.Format("Dense Layer"));
                        break;

                    case LinkType.DropoutLayer:
                        link = new DropoutLayer(ChainData, String.Format("Dropout Layer"));
                        break;

                    case LinkType.FlattenLayer:
                        link = new FlattenLayer(ChainData, String.Format("Flatten Layer"));
                        break;

                    case LinkType.ReshapeLayer:
                        link = new ReshapeLayer(ChainData, String.Format("Reshape Layer"));
                        break;

                    case LinkType.MergeLayer:
                        link = new MergeLayer(ChainData, String.Format("Merge Layer"));
                        break;
                }

                ChainData.ChainLinks.Insert(insertIndex, link);
                ValidateInputCompatibility();
                refreshLinks();
            }

        }

        public void Delete()
        {
            _parent.Chains.Remove(this);
        }

        public string Name
        {
            get { return ChainData.Name; }
            set
            {
                ChainData.Name = value;
                NotifyOfPropertyChange(() => Name);
            }
        }

        public ChainViewModelDropHandler DropHandler { get; set; }

        public class ChainViewModelDropHandler : IDropTarget
        {
            public void DragOver(IDropInfo dropInfo)
            {

            }

            public void Drop(IDropInfo dropInfo)
            {

            }
        }
    }
}
