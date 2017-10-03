using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Badger.Data.NeuralNetwork.Parameters;
using System.Collections.ObjectModel;
using GongSolutions.Wpf.DragDrop;
using Badger.ViewModels.NeuralNetwork.Links;

namespace Badger.ViewModels.NeuralNetwork.Parameters
{
    class LinkConnectionListParameterViewModel : ParameterBaseViewModel, IDropTarget
    {
        public LinkConnectionListParameter LinkConnectionListParameterData { get; }
        public LinkConnectionListParameterViewModel(LinkConnectionListParameter linkConnectionListParameter, LinkBaseViewModel parent) : base(linkConnectionListParameter, parent)
        {
            LinkConnectionListParameterData = linkConnectionListParameter;

            Value = new ObservableCollection<LinkConnectionViewModel>();
            refreshValue();
        }

        public ObservableCollection<LinkConnectionViewModel> Value { get; set; }

        private void refreshValue()
        {
            Value.Clear();
            foreach (var connection in LinkConnectionListParameterData.Value)
                Value.Add(new LinkConnectionViewModel(connection));
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

            LinkConnectionListParameterData.Value.Add(new LinkConnection(item.LinkBaseData));
            refreshValue();
        }
    }
}
