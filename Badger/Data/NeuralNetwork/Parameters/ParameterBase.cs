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

namespace Badger.Data.NeuralNetwork.Parameters
{
    public abstract class ParameterBase
    {
        protected ParameterBase() { }
        protected ParameterBase(LinkBase link)
        {
            Parent = link;
        }
        public ParameterBase(string name, LinkBase link) : this(link)
        {
            Name = name;
        }
        [XmlAttribute]
        public string Name { get; set; }
        [XmlIgnore]
        public LinkBase Parent { get; set; }
    }

    [XmlInclude(typeof(ActivationFunctionParameter))]
    [XmlInclude(typeof(IntParameter))]
    [XmlInclude(typeof(DoubleParameter))]
    [XmlInclude(typeof(IntTuple1DParameter))]
    [XmlInclude(typeof(IntTuple2DParameter))]
    [XmlInclude(typeof(IntTuple3DParameter))]
    [XmlInclude(typeof(IntTuple4DParameter))]
    [XmlInclude(typeof(InputDataParameter))]
    [XmlInclude(typeof(LinkConnectionListParameter))]
    [Serializable]
    public abstract class ParameterBase<T> : ParameterBase
    {
        protected ParameterBase() { }
        protected ParameterBase(LinkBase link) : base(link) { }
        public ParameterBase(string name, T value, LinkBase link) : this(name, link)
        {
            Value = value;
        }
        public ParameterBase(string name, LinkBase link) : base(name, link) { }

        public T Value { get; set; }
    }
}
