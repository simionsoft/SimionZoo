using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Badger.Data.NeuralNetwork.Parameters;
using Badger.Data.NeuralNetwork;
using Badger.ViewModels.NeuralNetwork.Links;

namespace Badger.ViewModels.NeuralNetwork.Parameters
{
    class InputDataParameterViewModel : ParameterBaseViewModel
    {
        public InputDataParameter InputDataParameterData { get; }
        public InputDataParameterViewModel(InputDataParameter inputDataParameter, LinkBaseViewModel parent) : base(inputDataParameter, parent)
        {
            InputDataParameterData = inputDataParameter;
        }

        public InputDataViewModel Value
        {
            get { return new InputDataViewModel(InputDataParameterData.InputDataValue); }
            set
            {
                InputDataParameterData.InputDataValue = value.InputData;
                NotifyOfPropertyChange(() => Value);
                Parent.NotifyOfPropertyChange(() => Parent.IsInputCompatible);
            }
        }

    }
}
