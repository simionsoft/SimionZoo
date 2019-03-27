using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimionSrcParser
{
    public class DocumentationExporter
    {
        public static void ExportGUIParameters(string filename, List<ParameterizedObject> objects)
        {
            //Export as XML for Badger
            using (StreamWriter outputFile = File.CreateText(filename))
            {
                //// encoding=\"utf-8\"
                outputFile.WriteLine("<?xml version=\"1.0\"?>\n<DEFINITIONS>");
                foreach (ParameterizedObject paramObj in objects)
                    outputFile.Write(paramObj.OutputXML(1));
                outputFile.Write(@"</DEFINITIONS>");
            }
        }
        public static void ExportGUIParametersForHumans(string outputFile, FileFormatter formatter, List<ParameterizedObject> objects)
        {
            //Export as XML for Badger
            using (StreamWriter writer = File.CreateText(outputFile))
            {
                formatter.OpeningSection(writer);
                formatter.Title1(writer, "GUI-editable parameters");

                //Regular classes first
                formatter.Title2(writer, "Regular classes");
                foreach (ParameterizedObject paramObj in objects)
                {
                    if (!paramObj.IsWorld() && paramObj.Parameters.Count > 0)
                    {
                        formatter.Title3(writer, paramObj.name);
                        formatter.OpenList(writer);
                        foreach (IParameter parameter in paramObj.Parameters)
                        {
                            string parameterDescription = formatter.InlineBold(parameter.GetName());
                            if (parameter.GetParameterType() != null)
                                parameterDescription += " (" + formatter.InlineItalic(parameter.GetParameterType()) + ")";
                            if (parameter.GetDescription() != null)
                                parameterDescription += " : " + parameter.GetDescription();
                            formatter.ListItem(writer, parameterDescription);
                        }
                        formatter.CloseList(writer);
                    }
                }

                //Then worlds
                formatter.Title2(writer, "Worlds");
                foreach (ParameterizedObject paramObj in objects)
                {
                    if (paramObj.IsWorld() && paramObj.Parameters.Count > 0)
                    {
                        formatter.Title3(writer, paramObj.name);
                        formatter.Title4(writer, "State variables");
                        formatter.OpenList(writer);
                        WorldParameter worldParameter;
                        foreach (IParameter parameter in paramObj.Parameters)
                        {
                            worldParameter = parameter as WorldParameter;
                            if (worldParameter != null && worldParameter.GetWorldParameterType() == WorldParser.WorldParameterType.StateVariable)
                                formatter.ListItem(writer, parameter.GetName());
                        }
                        formatter.CloseList(writer);
                        formatter.Title4(writer, "Action variables");
                        formatter.OpenList(writer);
                        foreach (IParameter parameter in paramObj.Parameters)
                        {
                            worldParameter = parameter as WorldParameter;
                            if (worldParameter != null && worldParameter.GetWorldParameterType() == WorldParser.WorldParameterType.ActionVariable)
                                formatter.ListItem(writer, parameter.GetName());
                        }
                        formatter.CloseList(writer);
                    }
                    
                }
                formatter.ClosingSection(writer);
            }
        }
    }
}
