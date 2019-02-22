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

using Herd.Files;
using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;



namespace SimionSrcParser
{
    public class Constructor: ParameterizedObject
    {
        public string m_world;
        private List<string> m_baseClasses= new List<string>();
        static ConstructorParameterParser g_parameterParser = new ConstructorParameterParser();

        public Constructor(string className, string paramName, string body, string bodyPrefix)
        {
            m_name = className;
            g_parameterParser.parse(this,body);

            //does this class extend some other parameterized class?
            string sPattern = @"(\w+)\s*\(\s*" + paramName + @"\s*\)";
            foreach(Match match in Regex.Matches(bodyPrefix,sPattern))
            {
                m_baseClasses.Add(match.Groups[1].Value);
            }
        }

        public override string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output,level);
            output += "<" + XMLTags.ClassDefinitionNodeTag + " Name=\"" + m_name + "\"";
            if (m_world!=null)
                output+= " " + XMLTags.worldAttribute + "=\"" + m_world + "\"";
            output += ">\n";

            foreach(string baseClass in m_baseClasses)
            {
                ParameterizedObject baseClassObject= SimionSrcParser.getNamedParamObject(baseClass);
                if (baseClassObject != null)
                    foreach (IParameter param in baseClassObject.parameters)
                        base.addParameter(param);//output += baseClassObject.outputChildrenXML(level + 1);
                else
                    Console.WriteLine("Warning." + m_name + " class extends base class " + baseClass
                    + " but definition has not been found. Ignore if the base class is a template class without any parameters.");
            }
            output += outputChildrenXML(level + 1);
            SimionSrcParser.addIndentation(ref output, level);
            output += "</" + XMLTags.ClassDefinitionNodeTag + ">\n";
            return output;
        }
    }
}
