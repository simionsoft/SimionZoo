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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;
using Badger.Data.NeuralNetwork.Links;
using Badger.Data.NeuralNetwork.Tuple;
using System.Runtime.Serialization;

namespace Badger.Data.NeuralNetwork.Parameters
{
    public class InputDataParameter : ParameterBase<String>
    {
        protected InputDataParameter() { }
        public InputDataParameter(LinkBase link) : base("Input Data", link) { }
        public InputDataParameter(InputData value, LinkBase link) : base("Input Data", value.ID, link) { }
        public InputDataParameter(String value, LinkBase link) : base("Input Data", value, link) { }

        [XmlIgnore]
        public InputData InputDataValue
        {
            get
            {
                foreach (var item in Parent.ParentChain.NetworkArchitecture.Problem.Inputs)
                {
                    if (item.ID.Equals(Value))
                    {
                        return item;
                    }
                }

                return null;
            }
            set
            {
                Value = value.ID;
            }
        }
    }
}
