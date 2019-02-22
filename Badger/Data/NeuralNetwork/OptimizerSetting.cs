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

using Badger.Data.NeuralNetwork.Links;
using Badger.Data.NeuralNetwork.Optimizers;
using Badger.Utils;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace Badger.Data.NeuralNetwork
{
    public class OptimizerSetting
    {
        private OptimizerSetting() { }
        public OptimizerSetting(OptimizerType value)
        {
            Value = value;
        }

        private OptimizerBase _optimizer;
        public OptimizerBase Optimizer
        {
            get
            {
                return _optimizer;
            }
            set
            {
                _optimizer = value;
                if (_optimizer != null)
                {
                    if (_optimizer.Type != Value)
                        Value = _optimizer.Type;
                }
            }
        }

        private OptimizerType _value;

        [XmlIgnore]
        public OptimizerType Value
        {
            get
            {
                return _value;
            }
            set
            {
                _value = value;
                var type = value.GetType();
                type = ((TypeReferenceAttribute)type.GetMember(value.ToString())[0].GetCustomAttributes(typeof(TypeReferenceAttribute), false)[0]).ReferenceType;
                if (Optimizer == null || (Optimizer != null && Optimizer.GetType() != type))
                {
                    Optimizer = (OptimizerBase)Activator.CreateInstance(type);
                    Optimizer.InitializeParameters();
                }
            }
        }
    }
}