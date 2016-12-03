using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using Simion;

namespace SimionSrcParser
{
    public abstract class ChoiceElementParser
    {
        public abstract void parse(string content, ChoiceParameter parent);
    }
    public class ChoiceElementParserV1: ChoiceElementParser
    {
        public override void parse(string content, ChoiceParameter parent)
        {
          //  {
          //      { "Deterministic-Policy-Gaussian-Noise",CHOICE_ELEMENT_NEW<CDeterministicPolicyGaussianNoise>},
		        //{ "Stochastic-Policy-Gaussian-Noise",CHOICE_ELEMENT_NEW<CStochasticPolicyGaussianNoise>}
          //  });
            string sChoiceElementPattern = @"{\s*""([^""]+)"",\s*CHOICE_ELEMENT_(NEW|FACTORY)\s*<(\w+)>\s*}";
            foreach (Match choiceElementMatch in Regex.Matches(content, sChoiceElementPattern))
            {
                string choiceElementName = choiceElementMatch.Groups[1].Value.Trim(' ', '"');
                string choiceElementClass = choiceElementMatch.Groups[3].Value.Trim(' ', '"');
                string choiceElementType = choiceElementMatch.Groups[2].Value.Trim(' ', '"');
                ChoiceElementParameter.Type type;
                if (choiceElementType == "NEW") type = ChoiceElementParameter.Type.New;
                else type = ChoiceElementParameter.Type.Factory;

                ChoiceElementParameter choiceElement = new ChoiceElementParameter(choiceElementName
                    , choiceElementClass, type);
                parent.addParameter(choiceElement);
            }
        }
    }
    public class ChoiceElementParserV2: ChoiceElementParser
    {
        public override void parse(string content, ChoiceParameter parent)
        {
          //  {
          //      { make_tuple("Wind-turbine", CHOICE_ELEMENT_NEW<CWindTurbine>, "World=Wind-turbine")},
		        //{ make_tuple("Underwater-vehicle", CHOICE_ELEMENT_NEW<CUnderwaterVehicle>, "World=Underwater-vehicle")},
		        //{ make_tuple("Pitch-control", CHOICE_ELEMENT_NEW<CPitchControl>, "World=Pitch-control")},
		        //{ make_tuple("Balancing-pole", CHOICE_ELEMENT_NEW<CBalancingPole>, "World=Balancing-pole")},
		        //{ make_tuple("Mountain-car", CHOICE_ELEMENT_NEW<CMountainCar>, "World=Mountain-car") }
          //  });
            string sChoiceElementPattern = @"{\s*make_tuple\s*\(\s*""([^""]+)""\s*,\s*CHOICE_ELEMENT_(NEW|FACTORY)\s*<(\w+)>\s*,\s*""([^""]*)""\s*\)\s*}";
            foreach (Match choiceElementMatch in Regex.Matches(content, sChoiceElementPattern))
            {
                string choiceElementName = choiceElementMatch.Groups[1].Value.Trim(' ', '"');
                string choiceElementClass = choiceElementMatch.Groups[3].Value.Trim(' ', '"');
                string choiceElementType = choiceElementMatch.Groups[2].Value.Trim(' ', '"');
                string badgerMetadata = choiceElementMatch.Groups[4].Value.Trim(' ', '"');
                ChoiceElementParameter.Type type;
                if (choiceElementType == "NEW") type = ChoiceElementParameter.Type.New;
                else type = ChoiceElementParameter.Type.Factory;

                ChoiceElementParameter choiceElement = new ChoiceElementParameter(choiceElementName
                    , choiceElementClass, type, badgerMetadata);
                parent.addParameter(choiceElement);
            }
        }
    }
    public class ChoiceParser
    {
        private List<ChoiceElementParser> m_choiceElementParsers= new List<ChoiceElementParser>();
        protected List<IParameter> choices = new List<IParameter>();
        public ChoiceParser()
        {
            m_choiceElementParsers.Add(new ChoiceElementParserV1());
            m_choiceElementParsers.Add(new ChoiceElementParserV2());
        }
        public void parse(ParameterizedObject parent, string srcCode)
        {
            string sPattern = @"CHOICE\s*<\s*(\w+)\s*>\(([^,]+),([^,]+),([^,]+),([^;]*);";

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

                foreach (ChoiceElementParser choiceElementParser in m_choiceElementParsers)
                    choiceElementParser.parse(choiceElements, newChoice);
                
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
            output += outputChildrenXML(level + 1);
            SimionSrcParser.addIndentation(ref output, level);
            output += "</" + XMLConfig.classDefinitionNodeTag + ">\n";
            return output;
        }
    }
}
