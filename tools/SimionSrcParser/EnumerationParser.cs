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

using Herd.Files;

namespace SimionSrcParser
{
    public class Enumeration: ParameterizedObject
    {
        static EnumerationParser enumerationParser= new EnumerationParser();
        List<string> m_enumerationValues= new List<string>();
        public Enumeration(string name,string content)
        {
            m_name = name;
            enumerationParser.parse(content, ref m_enumerationValues);
        }
        public override string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output,level);
            output += "<" + XMLTags.EnumDefinitionNodeTag + " " + XMLTags.nameAttribute + "=\"" + m_name + "\">\n";
            output+= outputChildrenXML(level + 1);
            foreach (string value in m_enumerationValues)
            {
                SimionSrcParser.addIndentation(ref output, level + 1);
                output += "<" + XMLTags.enumNodeTag + ">" + value + "</" + XMLTags.enumNodeTag + ">\n";
            }
            SimionSrcParser.addIndentation(ref output, level);
            output += "</" + XMLTags.EnumDefinitionNodeTag + ">\n";
            return output;
        }
    }
    public class EnumerationParser
    {
        public EnumerationParser()
        {

        }
        public void parse(string content,ref List<string> valueList)
        {
            string [] values= content.Split(',');
            foreach (string value in values)
                valueList.Add(value.Trim(' '));
        }
    }
}
