using Caliburn.Micro;
using Badger.Data.NeuralNetwork.Optimizers;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.ViewModels.NeuralNetwork.Optimizers
{
    class OptimizerParameterViewModel : PropertyChangedBase
    {
        public OptimizerParameter<string, double> OptimizerParameterData { get; }
        public OptimizerParameterViewModel(OptimizerParameter<string, double> optimizerParameter)
        {
            OptimizerParameterData = optimizerParameter;
        }

        public string Name { get { return OptimizerParameterData.Key; } }
        public double Value
        {
            get { return OptimizerParameterData.Value; }
            set
            {
                OptimizerParameterData.Value = value;
                NotifyOfPropertyChange(() => Value);
            }
        }
    }
}
