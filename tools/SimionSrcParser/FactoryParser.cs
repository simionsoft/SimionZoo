using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using Simion;

namespace SimionSrcParser
{
    public class ChoiceParser
    {
        protected List<IParameter> choices = new List<IParameter>();
        public ChoiceParser(){ }
        public void parse(ParameterizedObject parent, string srcCode)
        {
            string sPattern = @"CHOICE\s*<\s*(\w+)\s*>\(([^,]+),([^,]+),([^,]+),([^)]*?)\)";

            foreach (Match match in Regex.Matches(srcCode, sPattern))
            {
                string className = match.Groups[1].Value.Trim(' ', '"');
                string choiceName = match.Groups[3].Value.Trim(' ', '"');
                string comment = match.Groups[4].Value.Trim(' ', '"');
                string choiceElements,prefix;
                choiceElements = match.Groups[5].Value.Trim(' ', '"', '\t', '\n');
                SimionSrcParser.getEnclosedBody(match.Groups[5].Value.Trim(' ', '"','\t','\n'),0,"{","}",out choiceElements,out prefix);
                choiceElements= choiceElements.Trim(' ', '"', '\t', '\n');
                ChoiceParameter newChoice = new ChoiceParameter(className,choiceName, comment);

                string sChoiceElementPattern = @"{\s*([^,]+),\s*CHOICE_ELEMENT_(NEW|FACTORY)\s*<(\w+)>\s*}";
                foreach (Match choiceElementMatch in Regex.Matches(choiceElements, sChoiceElementPattern))
                {
                    string choiceElementName = choiceElementMatch.Groups[1].Value.Trim(' ', '"');
                    string choiceElementClass= choiceElementMatch.Groups[3].Value.Trim(' ', '"');
                    string choiceElementType= choiceElementMatch.Groups[2].Value.Trim(' ', '"');
                    ChoiceElementParameter.Type type;
                    if (choiceElementType == "NEW") type = ChoiceElementParameter.Type.New;
                    else type = ChoiceElementParameter.Type.Factory;

                    ChoiceElementParameter choiceElement = new ChoiceElementParameter(choiceElementName
                        , choiceElementClass, type);
                    newChoice.addParameter(choiceElement);
                }
                parent.addParameter(newChoice);
            }
        }
    }
 

    public class Factory: ParameterizedObject
    {
        private ChoiceParser m_choiceParser= new ChoiceParser();

        public Factory(string className, string paramName, string body, string bodyPrefix)
        {
            m_name = className;
            m_choiceParser.parse(this, body);
        }

        public override string outputXML(int level)
        {
            string output = "";
            SimionSrcParser.addIndentation(ref output, level);
            output += "<" + XMLConfig.classDefinitionNodeTag + " Name=\"" + m_name + "\">\n";
            base.outputXML(level + 1);
            output += "</" + XMLConfig.classDefinitionNodeTag + ">\n";
            return output;
        }
    }
}
