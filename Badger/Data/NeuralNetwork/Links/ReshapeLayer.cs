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
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;
using Badger.Data.NeuralNetwork.Parameters;

namespace Badger.Data.NeuralNetwork.Links
{
    [Serializable]
    public class ReshapeLayer : LinkBase
    {
        private ReshapeLayer() : base() { }
        public ReshapeLayer(Chain parent) : base(parent) { }

        public ReshapeLayer(Chain parent, string name) : base(parent, name)
        {
            Parameters.Add(new IntTuple4DParameter("4D Shape", this));
        }

        public override string TypeName { get { return "Reshape Layer"; } }

        public override bool IsInputCompatible
        {
            get
            {
                LinkBase previousLink = GetPreviousLink();
                if (previousLink == null)
                {
                    return false;
                }

                //TODO: add real code here!
                return true;
            }
        }

        public override int? GetTensorRank()
        {
            var itp = GetParameterByName<IntTuple4DParameter>("4D Shape");
            int rank = 0;
            if (itp.Value.x1 != 0)
                rank++;
            if (itp.Value.x2 != 0)
                rank++;
            if (itp.Value.x3 != 0)
                rank++;
            if (itp.Value.x4 != 0)
                rank++;

            return rank;
        }
    }
}
