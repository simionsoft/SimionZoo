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
using System.Text.RegularExpressions;

namespace SimionSrcParser
{

    public class ConstructorParameterParser
    {
        public const string extractTokenRegex = @"(?>[^()]+|\((?<open>)|\)(?<-open>))*";
        public const string extractFuncRegex = @"(.+?)\((.*)\)$";
        public const string extractArgsRegex = @"(?:[^,()]+((?:\((?>[^""()]+|\((?<open>)|\)(?<-open>))*\)))*)+";
        private List<Parser> m_parsers= new List<Parser>();

        public ConstructorParameterParser()
        {
            m_parsers.Add(new IntParameterParser());
            m_parsers.Add(new DoubleParameterParser());
            m_parsers.Add(new BoolParameterParser());
            m_parsers.Add(new StringParameterParser());
            m_parsers.Add(new FilePathParameterParser());
            m_parsers.Add(new DirPathParameterParser());
            m_parsers.Add(new NeuralNetworkParameterParser());
            m_parsers.Add(new ChildObjectParameterParser());
            m_parsers.Add(new ChildObjectFactoryParameterParser());
            m_parsers.Add(new StateVarRefParser());
            m_parsers.Add(new ActionVarRefParser());
            m_parsers.Add(new WireConnectionParser());
            m_parsers.Add(new MultiValueParameterParser());
            m_parsers.Add(new MultiValueFactoryParameterParser());
            m_parsers.Add(new MultiValueSimpleParameterParser());
            m_parsers.Add(new MultiVariableParameterParser());
            m_parsers.Add(new StateVariableParser());
            m_parsers.Add(new ActionVariableParser());
            m_parsers.Add(new ConstantParser());
            m_parsers.Add(new MetadataParser());
            m_parsers.Add(new EnumParameterParser());
        }
        public void parse(Constructor parent,string body)
        {
            foreach (Parser parser in m_parsers)
            {
                parser.parse(body, parent);
            }
        }

    }
}
