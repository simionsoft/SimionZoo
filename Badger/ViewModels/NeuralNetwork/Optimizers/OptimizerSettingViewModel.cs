using Caliburn.Micro;
using Badger.Data.NeuralNetwork;
using Badger.Data.NeuralNetwork.Optimizers;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.ViewModels.NeuralNetwork.Optimizers
{
    class OptimizerSettingViewModel : PropertyChangedBase
    {
        public OptimizerSetting OptimizerSettingData { get; }
        public OptimizerSettingViewModel(OptimizerSetting optimizerSetting)
        {
            OptimizerSettingData = optimizerSetting;

            OptimizerParameters = new ObservableCollection<OptimizerParameterViewModel>();
            setParameters();
        }

        public OptimizerType OptimizerType
        {
            get { return OptimizerSettingData.Value; }
            set
            {
                OptimizerSettingData.Value = value;
                NotifyOfPropertyChange(() => OptimizerType);
                setParameters();
            }
        }

        private void setParameters()
        {
            OptimizerParameters.Clear();
            foreach (var item in OptimizerSettingData.Optimizer.Parameters)
                OptimizerParameters.Add(new OptimizerParameterViewModel(item));
        }

        public ObservableCollection<OptimizerParameterViewModel> OptimizerParameters { get; set; }
    }
}
