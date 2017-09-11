using Caliburn.Micro;
using Badger.Data.NeuralNetwork.Parameters;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.ViewModels.NeuralNetwork
{
    class LinkConnectionViewModel : PropertyChangedBase
    {
        public LinkConnection LinkConnectionData { get; }
        public LinkConnectionViewModel(LinkConnection linkConnection)
        {
            LinkConnectionData = linkConnection;
        }

        public string TargetID { get { return LinkConnectionData.Target.ID; } }
        public string TargetName { get { return LinkConnectionData.Target.Name; } }

    }
}
