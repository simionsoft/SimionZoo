using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace SimionSrcParser
{

    public class ParameterParser
    {
        public const string extractTokenRegex = @"(?>[^()]+|\((?<open>)|\)(?<-open>))*";
        public const string extractFuncRegex = @"(.+?)\((.*)\)$";
        public const string extractArgsRegex = @"(?:[^,()]+((?:\((?>[^""()]+|\((?<open>)|\)(?<-open>))*\)))*)+";
        private List<Parser> m_parsers= new List<Parser>();

        public ParameterParser()
        {
            m_parsers.Add(new IntParameterParser());
            m_parsers.Add(new DoubleParameterParser());
            m_parsers.Add(new BoolParameterParser());
            m_parsers.Add(new StringParameterParser());
            m_parsers.Add(new FilePathParameterParser());
            m_parsers.Add(new DirPathParameterParser());
            m_parsers.Add(new ChildObjectParameterParser());
            m_parsers.Add(new ChildObjectFactoryParameterParser());
            m_parsers.Add(new StateVarRefParser());
            m_parsers.Add(new ActionVarRefParser());
        }
        public void parse(string body, ref List<Parameter> outParameterList)
        {
            foreach (Parser parser in m_parsers)
            {
                parser.parse(body, ref outParameterList);
                //string sPattern = parser.inSrcTemplateName + @"\s*\(" + extractTokenRegex + @"\)";

                //foreach (Match match in Regex.Matches(body, sPattern))
                //{
                //    var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                //    string arguments = functionArgumentsMatch.Groups[2].Value;
                //    var parameterMatches = Regex.Matches(arguments, extractArgsRegex);
                //    Parser newParameter = new Parser();
                //    newParameter.m_type = parser;

                //    Console.WriteLine("Parameter: " + parameterMatches[1].Value + "(" + parser.m_outXmlTag + ")");
                //}
            }
        }

    }
}
