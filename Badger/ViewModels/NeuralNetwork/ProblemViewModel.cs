/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

using Caliburn.Micro;
using Badger.Data.NeuralNetwork;
using Badger.ViewModels.NeuralNetwork.Links;
using Badger.ViewModels.NeuralNetwork.Optimizers;
using Badger.ViewModels.NeuralNetwork.Parameters;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.ViewModels.NeuralNetwork
{
    class ProblemViewModel : PropertyChangedBase
    {
        public Problem ProblemData { get; }

        public ProblemViewModel(Problem problem)
        {
            ProblemData = problem;
            NetworkArchitecture = new NetworkArchitectureViewModel(problem.NetworkArchitecture);
            OptimizerSetting = new OptimizerSettingViewModel(problem.OptimizerSetting);

            Inputs = new ObservableCollection<InputDataViewModel>();
            foreach (var input in problem.Inputs)
                Inputs.Add(new InputDataViewModel(input));

            OutputConfiguration = new OutputConfigurationViewModel(ProblemData.Output);
        }

        public NetworkArchitectureViewModel NetworkArchitecture { get; set; }
        public ObservableCollection<InputDataViewModel> Inputs { get; set; }

        public OptimizerSettingViewModel OptimizerSetting { get; set; }

        public OutputConfigurationViewModel OutputConfiguration { get; set; }
    }
}
