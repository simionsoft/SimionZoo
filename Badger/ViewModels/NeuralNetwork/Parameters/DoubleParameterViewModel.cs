using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Badger.Data.NeuralNetwork.Parameters;
using Badger.ViewModels.NeuralNetwork.Links;

namespace Badger.ViewModels.NeuralNetwork.Parameters
{
    class DoubleParameterViewModel : ParameterBaseViewModel
    {
        public DoubleParameter DoubleParameterData { get; }
        public DoubleParameterViewModel(DoubleParameter doubleParameter, LinkBaseViewModel parent) : base(doubleParameter, parent)
        {
            DoubleParameterData = doubleParameter;
        }
        public double Value
        {
            get { return DoubleParameterData.Value; }
            set
            {
                DoubleParameterData.Value = value;
                NotifyOfPropertyChange(() => Value);
            }
        }
    }
}
