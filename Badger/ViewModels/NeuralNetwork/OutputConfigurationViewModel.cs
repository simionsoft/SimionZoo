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

            DummyItems = new ObservableCollection<LinkBaseViewModel>();
            if (OutputConfigurationData.LinkConnection != null && OutputConfigurationData.LinkConnection.Target != null)
                DummyItems.Add(new LinkBaseViewModel(OutputConfigurationData.LinkConnection.Target, null));
        }

        public ObservableCollection<LinkBaseViewModel> DummyItems { get; set; }
        public LinkConnectionViewModel OutputLinkConnection
        {
            get
            {
                if (DummyItems.Count == 0)
                    return null;
                return new LinkConnectionViewModel(new LinkConnection(DummyItems[0].LinkBaseData));
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

            DummyItems.Clear();
            DummyItems.Add(item);
            OutputConfigurationData.LinkConnection = new LinkConnection(item.LinkBaseData);
        }
    }
}
