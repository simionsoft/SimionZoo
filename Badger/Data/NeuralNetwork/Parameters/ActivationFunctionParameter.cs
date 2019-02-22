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
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Badger.Data.NeuralNetwork.Links;

namespace Badger.Data.NeuralNetwork.Parameters
{
    public class ActivationFunctionParameter : ParameterBase<ActivationFunction>
    {
        private ActivationFunctionParameter() { }
        public ActivationFunctionParameter(string name, LinkBase link) : base(name, link) { }
        public ActivationFunctionParameter(string name, ActivationFunction value, LinkBase link) : base(name, value, link) { }
    }

    public enum ActivationFunction
    {
        [Description("Sigmoid")]
        sigmoid = 0,
        [Description("Exponential Linear Unit")]
        elu,
        [Description("Scaled Exponential Linear Unit")]
        selu,
        [Description("Softplus")]    
        softplus,
        [Description("Softsign")]
        softsign,
        [Description("Rectified Linear Unit")]
        relu,
        [Description("Tanh")]
        tanh,
        [Description("Hard Sigmoid")]
        hard_sigmoid,
        [Description("Softmax")]
        softmax,
        [Description("Linear")]
        linear
    }
}
