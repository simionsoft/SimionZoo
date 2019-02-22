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
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Xml.Serialization;

namespace Badger.Data.NeuralNetwork.Optimizers
{
    [XmlInclude(typeof(OptimizerSGD))]
    [XmlInclude(typeof(OptimizerMomentumSGD))]
    [XmlInclude(typeof(OptimizerNesterov))]
    [XmlInclude(typeof(OptimizerFSAdaGrad))]
    [XmlInclude(typeof(OptimizerAdam))]
    [XmlInclude(typeof(OptimizerAdaGrad))]
    [XmlInclude(typeof(OptimizerRMSProp))]
    [XmlInclude(typeof(OptimizerAdaDelta))]
    public abstract class OptimizerBase
    {
        public ObservableCollection<OptimizerParameter<string, Double>> Parameters { get; set; }
        public abstract void InitializeParameters();
        public abstract OptimizerType Type { get; }
    }

    public class OptimizerSGD : OptimizerBase
    {
        public OptimizerSGD() : base() { }

        public override OptimizerType Type { get { return OptimizerType.SGD; } }

        public override void InitializeParameters()
        {
            Parameters = new ObservableCollection<OptimizerParameter<string, double>>();
            Parameters.Add(new OptimizerParameter<string, double>("Learning rate", 0.01));
        }
    }

    public class OptimizerMomentumSGD : OptimizerBase
    {
        public OptimizerMomentumSGD() : base() { }
        public override OptimizerType Type { get { return OptimizerType.MomentumSGD; } }

        public override void InitializeParameters()
        {
            Parameters = new ObservableCollection<OptimizerParameter<string, double>>();
            Parameters.Add(new OptimizerParameter<string, double>("Learning rate", 0.01));
            Parameters.Add(new OptimizerParameter<string, double>("Momentum", 0.1));
        }
    }

    public class OptimizerNesterov : OptimizerBase
    {
        public OptimizerNesterov() : base() { }
        public override OptimizerType Type { get { return OptimizerType.Nesterov; } }

        public override void InitializeParameters()
        {
            Parameters = new ObservableCollection<OptimizerParameter<string, double>>();
            Parameters.Add(new OptimizerParameter<string, double>("Learning rate", 0.01));
            Parameters.Add(new OptimizerParameter<string, double>("Momentum", 0.1));
        }
    }

    public class OptimizerFSAdaGrad : OptimizerBase
    {
        public OptimizerFSAdaGrad() : base() { }
        public override OptimizerType Type { get { return OptimizerType.FSAdaGrad; } }

        public override void InitializeParameters()
        {
            Parameters = new ObservableCollection<OptimizerParameter<string, double>>();
            Parameters.Add(new OptimizerParameter<string, double>("Learning rate", 0.01));
            Parameters.Add(new OptimizerParameter<string, double>("Momentum", 0.1));
            Parameters.Add(new OptimizerParameter<string, double>("Variance momentum", 0.1));
        }
    }

    public class OptimizerAdam : OptimizerBase
    {
        public OptimizerAdam() : base() { }
        public override OptimizerType Type { get { return OptimizerType.Adam; } }

        public override void InitializeParameters()
        {
            Parameters = new ObservableCollection<OptimizerParameter<string, double>>();
            Parameters.Add(new OptimizerParameter<string, double>("Learning rate", 0.001));
            Parameters.Add(new OptimizerParameter<string, double>("Momentum", 0.9));
            Parameters.Add(new OptimizerParameter<string, double>("Variance momentum", 0.999));
            Parameters.Add(new OptimizerParameter<string, double>("Epsilon", 1e-8));
        }
    }

    public class OptimizerAdaGrad : OptimizerBase
    {
        public OptimizerAdaGrad() : base() { }
        public override OptimizerType Type { get { return OptimizerType.AdaGrad; } }

        public override void InitializeParameters()
        {
            Parameters = new ObservableCollection<OptimizerParameter<string, double>>();
            Parameters.Add(new OptimizerParameter<string, double>("Learning rate", 0.01));
        }
    }

    public class OptimizerRMSProp : OptimizerBase
    {
        public OptimizerRMSProp() : base() { }
        public override OptimizerType Type { get { return OptimizerType.RMSProp; } }

        public override void InitializeParameters()
        {
            Parameters = new ObservableCollection<OptimizerParameter<string, double>>();
            Parameters.Add(new OptimizerParameter<string, double>("Learning rate", 0.001));
            Parameters.Add(new OptimizerParameter<string, double>("Gamma", 0.95));
            Parameters.Add(new OptimizerParameter<string, double>("Inc", 2.0));
            Parameters.Add(new OptimizerParameter<string, double>("Dec", 0.01));
            Parameters.Add(new OptimizerParameter<string, double>("Max", 0.1));
            Parameters.Add(new OptimizerParameter<string, double>("Min", 0.0001));
        }
    }

    public class OptimizerAdaDelta : OptimizerBase
    {
        public OptimizerAdaDelta() : base() { }
        public override OptimizerType Type { get { return OptimizerType.AdaDelta; } }

        public override void InitializeParameters()
        {
            Parameters = new ObservableCollection<OptimizerParameter<string, double>>();
            Parameters.Add(new OptimizerParameter<string, double>("Learning rate", 1.0));
            Parameters.Add(new OptimizerParameter<string, double>("Rho", 0.95));
            Parameters.Add(new OptimizerParameter<string, double>("Epsilon", 1e-8));
        }
    }
}
