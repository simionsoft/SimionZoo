using Badger.Data.NeuralNetwork.Parameters;
using Badger.ViewModels.NeuralNetwork.Links;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.ViewModels.NeuralNetwork.Parameters
{
    class IntParameterViewModel : ParameterBaseViewModel
    {
        public IntParameter IntParameterData { get; }

        public IntParameterViewModel(IntParameter intParameter, LinkBaseViewModel parent) : base(intParameter, parent)
        {
            IntParameterData = intParameter;
        }

        public int Value
        {
            get { return IntParameterData.Value; }
            set
            {
                IntParameterData.Value = value;
                NotifyOfPropertyChange(() => Value);
            }
        }
    }
}
