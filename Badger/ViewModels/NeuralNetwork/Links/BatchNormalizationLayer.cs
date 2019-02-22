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


namespace Badger.Data.NeuralNetwork.Links
{
    [Serializable]
    public class BatchNormalizationLayer : LinkBase
    {
        private BatchNormalizationLayer() : base() { }
        public BatchNormalizationLayer(Chain parent) : base(parent) { }

        public BatchNormalizationLayer(Chain parent, string name) : base(parent, name) { }

        public override string TypeName { get { return "Batch normalization Layer"; } }

        public override bool IsInputCompatible
        {
            get
            {
                LinkBase previousLink = GetPreviousLink();
                if (previousLink == null)
                {
                    return false;
                }

                return true;
            }
        }

        public override int? GetTensorRank()
        {
            if (GetPreviousLink() is null)
                return null;
            return GetPreviousLink().GetTensorRank();
        }
    }
}