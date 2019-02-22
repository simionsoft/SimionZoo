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


using System.Collections.Generic;
using System.Xml;

namespace Herd.Files
{
    public class Fork
    {
        public string Name;

        public List<Fork> Forks = new List<Fork>();
        public List<ForkValue> Values = new List<ForkValue>();

        public Fork(XmlNode configNode)
        {
            if (configNode.Attributes.GetNamedItem(XMLTags.aliasAttribute) != null)
                Name = configNode.Attributes[XMLTags.aliasAttribute].Value;

            foreach (XmlNode child in configNode.ChildNodes)
            {
                if (child.Name == XMLTags.forkTag)
                {
                    Fork newFork = new Fork(child);
                    Forks.Add(newFork);
                }
                else if (child.Name == XMLTags.forkValueTag)
                {
                    ForkValue newValue = new ForkValue(child);
                    Values.Add(newValue);
                }
            }
        }
    }
}
