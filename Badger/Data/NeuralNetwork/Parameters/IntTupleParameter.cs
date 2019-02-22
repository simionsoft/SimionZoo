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
using Badger.Data.NeuralNetwork.Links;
using System.Xml.Serialization;
using Badger.Data.NeuralNetwork.Tuple;

namespace Badger.Data.NeuralNetwork.Parameters
{
    [Serializable]
    public class IntTuple1DParameter : ParameterBase<IntTuple1D>
    {
        private IntTuple1DParameter() { }
        public IntTuple1DParameter(string name, LinkBase link) : base(name, new IntTuple1D(), link) { }
        public IntTuple1DParameter(string name, IntTuple1D value, LinkBase link) : base(name, value, link) { }
    }

    [Serializable]
    public class IntTuple2DParameter : ParameterBase<IntTuple2D>
    {
        private IntTuple2DParameter() { }
        public IntTuple2DParameter(string name, LinkBase link) : base(name, new IntTuple2D(), link) { }
        public IntTuple2DParameter(string name, IntTuple2D value, LinkBase link) : base(name, value, link) { }
    }

    [Serializable]
    public class IntTuple3DParameter : ParameterBase<IntTuple3D>
    {
        private IntTuple3DParameter() { }
        public IntTuple3DParameter(string name, LinkBase link) : base(name, new IntTuple3D(), link) { }
        public IntTuple3DParameter(string name, IntTuple3D value, LinkBase link) : base(name, value, link) { }
    }

    [Serializable]
    public class IntTuple4DParameter : ParameterBase<IntTuple4D>
    {
        private IntTuple4DParameter() { }
        public IntTuple4DParameter(string name, LinkBase link) : base(name, new IntTuple4D(), link) { }
        public IntTuple4DParameter(string name, IntTuple4D value, LinkBase link) : base(name, value, link) { }
    }

}
