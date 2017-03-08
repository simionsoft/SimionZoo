using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using Badger.Simion;

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
            output += "<" + XMLConfig.classDefinitionNodeTag + " Name=\"" + m_name + "\"";
            if (m_world!=null)
                output+= " " + XMLConfig.worldAttribute + "=\"" + m_world + "\"";
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
            output += "</" + XMLConfig.classDefinitionNodeTag + ">\n";
            return output;
        }
    }
}
