using Caliburn.Micro;
using GongSolutions.Wpf.DragDrop;
using Badger.Data.NeuralNetwork.Parameters;
using Badger.ViewModels.NeuralNetwork.Links;
using Badger.ViewModels.NeuralNetwork.Parameters;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Badger.Data.NeuralNetwork.Problem;

namespace Badger.ViewModels.NeuralNetwork
{
    class OutputConfigurationViewModel : PropertyChangedBase, IDropTarget
    {
        public OutputConfiguration OutputConfigurationData { get; }
        public OutputConfigurationViewModel(OutputConfiguration outputConfiguration)
        {
            OutputConfigurationData = outputConfiguration;

            if (OutputConfigurationData.LinkConnection != null && OutputConfigurationData.LinkConnection.Target != null)
                OutputLinkConnection = new LinkConnectionViewModel(OutputConfigurationData.LinkConnection);
        }

        private LinkConnectionViewModel _outputLinkConnection;
        public LinkConnectionViewModel OutputLinkConnection
        {
            get
            {
                return _outputLinkConnection;
            }
            set
            {
                _outputLinkConnection = value;
                NotifyOfPropertyChange(() => OutputLinkConnection);
            }
        }

        public void DragOver(IDropInfo dropInfo)
        {
            if (dropInfo.Data is LinkBaseViewModel)
            {
                dropInfo.Effects = System.Windows.DragDropEffects.Copy;
                dropInfo.NotHandled = false;
                dropInfo.DropTargetAdorner = DropTargetAdorners.Insert;
            }
        }

        public void Drop(IDropInfo dropInfo)
        {
            var item = dropInfo.Data as LinkBaseViewModel;

            if (item == null)
                return;

            OutputConfigurationData.LinkConnection = new LinkConnection(item.LinkBaseData);
            OutputLinkConnection = new LinkConnectionViewModel(OutputConfigurationData.LinkConnection); 
        }
    }
}
