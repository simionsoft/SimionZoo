using Caliburn.Micro;
using Badger.Data.NeuralNetwork.Links;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.ViewModels.NeuralNetwork.Windows
{
    class AddLinkWindowViewModel : Screen
    {
        public void Accept()
        {
            TryClose();
        }

        public void Cancel()
        {
            Result = null;
            TryClose();
        }

        public LinkType? Result { get; set; } = LinkType.DenseLayer;
    }
}
