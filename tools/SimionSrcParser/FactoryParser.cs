using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimionSrcParser
{
    public class ChoiceParser : Parser
    {
        protected List<IParameter> choices = new List<IParameter>();
        public ChoiceParser() : base("CHOICE", true) { }
        public override void processParameter(Constructor parent)
        {
            ChoiceParameter choiceParameter = new ChoiceParameter(parsedArguments[0], parsedArguments[2]
                , parsedArguments[3], parsedArguments[4]);
        }
    }
    public class ChoiceElementParser
    {
        protected List<string> parsedArguments;

        public ChoiceElementParser()
        {
            parsedArguments = new List<string>();
        }
        public void parse(string content, Constructor parent)
        {
            string sPattern;
            sPattern = @"{([^,]+),([^}]+)}";

            foreach (Match match in Regex.Matches(content, sPattern))
            {
                string choiceElementName = match.Groups[1].Value.Trim(' ', '"');
                string choiceValue = match.Groups[2].Value.Trim(' ', '"');
                parsedArguments.Clear();

                //var functionArgumentsMatch = Regex.Match(match.Groups[0].Value, ParameterParser.extractFuncRegex);
                //string arguments = functionArgumentsMatch.Groups[2].Value;
                //var argumentMatches = Regex.Matches(arguments, ParameterParser.extractArgsRegex);
                //foreach (Match argMatch in argumentMatches)
                //{
                //    strippedArgumentValue = argMatch.Value.Trim(' ', '\"', '\n', '\t');

                //    parsedArguments.Add(strippedArgumentValue);
                //}
                //processParameter(parent);
            }
        }

    }

    public class Factory: ParameterizedObject
    {
        public Factory(string className, string paramName, string body, string bodyPrefix)
        {
            m_name = className;
            //g_parameterParser.parse(this, body);
        }
        public override string outputXML(int level)
        {
            throw new NotImplementedException();
        }
    }
}
