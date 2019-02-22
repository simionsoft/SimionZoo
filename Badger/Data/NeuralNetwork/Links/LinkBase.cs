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
using System.Runtime.Serialization;
using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Xml.Serialization;
using Badger.Data.NeuralNetwork.Parameters;

namespace Badger.Data.NeuralNetwork.Links
{
    [XmlInclude(typeof(InputLayer))]
    [XmlInclude(typeof(ActivationLayer))]
    [XmlInclude(typeof(Convolution1DLayer))]
    [XmlInclude(typeof(Convolution2DLayer))]
    [XmlInclude(typeof(Convolution3DLayer))]
    [XmlInclude(typeof(DenseLayer))]
    [XmlInclude(typeof(DropoutLayer))]
    [XmlInclude(typeof(FlattenLayer))]
    [XmlInclude(typeof(ReshapeLayer))]
    [XmlInclude(typeof(MergeLayer))]
    [XmlInclude(typeof(LinearTransformationLayer))]
    [XmlInclude(typeof(BatchNormalizationLayer))]
    [Serializable]
    public abstract class LinkBase : IDeserializationCallback
    {
        [XmlIgnore]
        public abstract bool IsInputCompatible { get; }

        protected LinkBase()
        {
            Parameters = new List<ParameterBase>();

            initializeID();
        }
        public LinkBase(Chain parent) : this()
        {
            ParentChain = parent;
        }

        public LinkBase(Chain parent, string name) : this(parent)
        {
            Name = name;
        }

        private void initializeID()
        {
            //generate a 8 character long ID for this link
            //TODO: remove this and replace it with something more robust
            ID = Utility.GetHashString(DateTime.Now.ToFileTimeUtc().ToString() + Regex.Replace(TypeName, @"\s+", "")).Substring(0, 8);
        }

        public T GetParameterByName<T>(string name) where T : ParameterBase
        {
            name = name.ToLower();
            foreach (var item in Parameters)
                if (item.Name.ToLower() == name)
                    return item as T;
            return null;
        }

        [XmlIgnore]
        public Chain ParentChain { get; internal set; }

        public List<ParameterBase> Parameters { get; }

        [XmlAttribute]
        public string Name { get; set; }

        [XmlAttribute]
        public virtual string TypeName { get; }

        public abstract int? GetTensorRank();

        [XmlAttribute]
        public string ID { get; set; }

        public void OnDeserialization(object sender)
        {
            foreach (var item in Parameters)
            {
                item.Parent = this;
            }
        }

        public LinkBase GetPreviousLink()
        {
            int index = ParentChain.ChainLinks.IndexOf(this);
            if (index <= 0)
                return null;
            return ParentChain.ChainLinks[index - 1];
        }
    }
}
