using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace SimionSrcParser
{

    public class ParameterType
    {
        public string m_outXmlTag;
        public string m_inSrcTemplateName;
        protected Dictionary<string, int> m_arguments; //name - argumentIndex pairs
        protected bool m_bTemplateParameter;
        public ParameterType(string inSrcTemplateName, string outXmlTag, Dictionary<string,int> arguments
            ,bool bTemplateParameter= false)
        {
            m_outXmlTag = outXmlTag;
            m_inSrcTemplateName = inSrcTemplateName;
            m_arguments = arguments;
            m_bTemplateParameter = bTemplateParameter;
        }
    }
    public class Parameter
    {
        public ParameterType m_type;
        public Dictionary<string, string> properties= new Dictionary<string, string>();
    }
    public class ParameterParser
    {
        private string extractTokenRegex = @"(?>[^()]+|\((?<open>)|\)(?<-open>))*";
        private string extractFuncRegex = @"(.+?)\((.*)\)$";
        private string extractArgsRegex = @"(?:[^,()]+((?:\((?>[^""()]+|\((?<open>)|\)(?<-open>))*\)))*)+";
        private List<ParameterType> m_parameterTypes= new List<ParameterType>();
        

        public ParameterParser()
        {
            m_parameterTypes.Add(new ParameterType(@"INT_PARAM", Simion.XMLConfig.integerNodeTag
                , new Dictionary<string, int>() { { @"Name", 1 },{ @"Comment", 2 }, { @"Default", 3 } }));
            m_parameterTypes.Add(new ParameterType(@"DOUBLE_PARAM", Simion.XMLConfig.doubleNodeTag
                , new Dictionary<string, int>() { { @"Name", 1 }, { @"Comment", 2 }, { @"Default", 3 } }));
            m_parameterTypes.Add(new ParameterType(@"BOOL_PARAM", Simion.XMLConfig.boolNodeTag
                , new Dictionary<string, int>() { { @"Name", 1 }, { @"Comment", 2 }, { @"Default", 3 } }));
            m_parameterTypes.Add(new ParameterType(@"STRING_PARAM", Simion.XMLConfig.stringNodeTag
                , new Dictionary<string, int>() { { @"Name", 1 }, { @"Comment", 2 }, { @"Default", 3 } }));
            m_parameterTypes.Add(new ParameterType(@"DIR_PATH_PARAM", Simion.XMLConfig.dirPathNodeTag
                , new Dictionary<string, int>() { { @"Name", 1 }, { @"Comment", 2 }, { @"Default", 3 } }));
            m_parameterTypes.Add(new ParameterType(@"FILE_PATH_PARAM", Simion.XMLConfig.filePathNodeTag
                , new Dictionary<string, int>() { { @"Name", 1 }, { @"Comment", 2 }, { @"Default", 3 } }));
        }
        public void parse(string body, ref List<Parameter> outParameterList)
        {
            foreach (ParameterType parType in m_parameterTypes)
            {
                string sPattern = parType.m_inSrcTemplateName + @"\s*\(" + extractTokenRegex + @"\)";

                foreach (Match match in Regex.Matches(body, sPattern))
                {
                    var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, extractFuncRegex);
                    string arguments = functionArgumentsMatch.Groups[2].Value;
                    var parameterMatches = Regex.Matches(arguments, extractArgsRegex);
                    Parameter newParameter = new Parameter();
                    newParameter.m_type = parType;

                    Console.WriteLine("Parameter: " + parameterMatches[1].Value + "(" + parType.m_outXmlTag + ")");
                }
            }
        }

    }
}
