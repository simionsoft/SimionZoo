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
using Badger.Data.NeuralNetwork.Parameters;
using Badger.Data.NeuralNetwork.Tuple;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.ViewModels.NeuralNetwork.Windows
{
    class MainWindowViewModel : PropertyChangedBase
    {
        public MainWindowViewModel()
        {
            var problem = new Problem();
            foreach (var item in GetSampleInputData())
                problem.Inputs.Add(item);

            Problem = new ProblemViewModel(problem);
        }

        public static List<InputData> GetSampleInputData()
        {
            var result = new List<InputData>();
            result.Add(new InputData("state-input", "State", new IntTuple1D(-1), "The state vector."));
            result.Add(new InputData("action-input", "Action", new IntTuple1D(-1), "The action vector."));
            return result;
        }

        private ProblemViewModel _problem;
        public ProblemViewModel Problem
        {
            get
            {
                return _problem;
            }
            set
            {
                _problem = value;
                NotifyOfPropertyChange(() => Problem);
            }
        }

        public void Export()
        {
            Problem.ProblemData.Export();
        }

        public string ExportToString()
        {
            return Problem.ProblemData.ExportToString();
        }

        public void Import()
        {
            Problem = new ProblemViewModel(Badger.Data.NeuralNetwork.Problem.Import(GetSampleInputData()));
        }

        public void Import(string content)
        {
            Problem = new ProblemViewModel(Badger.Data.NeuralNetwork.Problem.Import(content, GetSampleInputData()));
        }
    }
}
