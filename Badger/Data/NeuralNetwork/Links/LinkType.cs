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

namespace Badger.Data.NeuralNetwork.Links
{
    public enum LinkType
    {
        [Description("Input Layer")]
        InputLayer,
        [Description("Activation Layer")]
        ActivationLayer,
        [Description("1D Convolution Layer")]
        Convolution1DLayer,
        [Description("2D Convolution Layer")]
        Convolution2DLayer,
        [Description("3D Convolution Layer")]
        Convolution3DLayer,
        [Description("Dense Layer")]
        DenseLayer,
        [Description("Dropout Layer")]
        DropoutLayer,
        [Description("Flatten Layer")]
        FlattenLayer,
        [Description("Reshape Layer")]
        ReshapeLayer,
        [Description("Merge Layer")]
        MergeLayer,
        [Description("Batch Normalization Layer")]
        BatchNormalizationLayer,
        [Description("Linear Transformation Layer")]
        LinearTransformationLayer,
    }
}
