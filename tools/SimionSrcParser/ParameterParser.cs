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
            m_parsers.Add(new NeuralNetworkProblemDescriptionParameterParser());
            m_parsers.Add(new ChildObjectParameterParser());
            m_parsers.Add(new ChildObjectFactoryParameterParser());
            m_parsers.Add(new StateVarRefParser());
            m_parsers.Add(new ActionVarRefParser());
            m_parsers.Add(new MultiValueParameterParser());
            m_parsers.Add(new MultiValueFactoryParameterParser());
            m_parsers.Add(new MultiValueSimpleParameterParser());
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
