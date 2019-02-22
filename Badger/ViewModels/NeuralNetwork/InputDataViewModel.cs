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

namespace Badger.ViewModels.NeuralNetwork
{
    class InputDataViewModel : PropertyChangedBase
    {
        public InputData InputData { get; }
        public InputDataViewModel(InputData inputData)
        {
            InputData = inputData;
        }

        public string ID
        {
            get { return InputData.ID; }
            set
            {
                InputData.ID = value;
                NotifyOfPropertyChange(() => ID);
            }
        }

        public string Name
        {
            get { return InputData.Name; }
            set
            {
                InputData.Name = value;
                NotifyOfPropertyChange(() => Name);
            }
        }

        public string Description
        {
            get { return InputData.Description; }
            set
            {
                InputData.Description = value;
                NotifyOfPropertyChange(() => Description);
            }
        }

        public IntTupleBase Shape
        {
            get { return InputData.Shape; }
            set
            {
                InputData.Shape = value;
                NotifyOfPropertyChange(() => Shape);
            }
        }

        public int Dimension
        {
            get
            {
                return Shape.Dimension;
            }
            set
            {
                if (value == Shape.Dimension)
                    return;

                switch (value)
                {
                    case 1:
                        Shape = new IntTuple1D(-1);
                        break;
                    case 2:
                        Shape = new IntTuple2D(-1, -1);
                        break;
                    case 3:
                        Shape = new IntTuple3D(-1, -1, -1);
                        break;
                    default:
                    case 4:
                        Shape = new IntTuple4D(-1, -1, -1, -1);
                        break;
                }

            }
        }

        public override bool Equals(object obj)
        {
            if (obj == null || !(obj is InputDataViewModel))
                return false;

            return (obj == this || (obj as InputDataViewModel).ID.Equals(ID));
        }

        public override int GetHashCode()
        {
            return 1213502048 + EqualityComparer<string>.Default.GetHashCode(ID);
        }
    }
}
