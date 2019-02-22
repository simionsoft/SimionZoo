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
    public class MergeLayer : LinkBase
    {
        private MergeLayer() : base() { }
        public MergeLayer(Chain parent) : base(parent) { }

        public MergeLayer(Chain parent, string name) : base(parent, name)
        {
            Parameters.Add(new IntParameter("Axis", this));
            Parameters.Add(new LinkConnectionListParameter("Links", this));
        }

        public override string TypeName { get { return "Merge Layer"; } }

        public override bool IsInputCompatible
        {
            get
            {
                //now check, of we have to make this false again
                var list = (Parameters[1] as LinkConnectionListParameter).Value;
                if (list.Count > 0)
                {
                    //check the ranks
                    int? rawRank = list[0].Target.GetTensorRank();
                    if (!rawRank.HasValue)
                    {
                        return false;
                    }
                    int rank = rawRank.Value;
                    for (int i = 1; i < list.Count; i++)
                    {
                        if (!rawRank.Equals(list[i].Target.GetTensorRank()))
                        {
                            return false;
                        }
                    }
                }

                return true;
            }
        }

        public override int? GetTensorRank()
        {
            var list = GetParameterByName<LinkConnectionListParameter>("Links").Value;
            if (list.Count > 0)
            {
                return list[0].Target.GetTensorRank();
            }
            return null;
        }
    }
}
