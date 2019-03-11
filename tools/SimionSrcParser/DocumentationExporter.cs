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
        static void ExportMethod(StreamWriter writer, ClassMethod method, FileFormatter exporter)
        {
            exporter.Title3(writer, exporter.InlineCode(method.ReturnType + " "
                + method.Name + "(" + method.Arguments + ")"));

            exporter.OpenList(writer);
            if (method.MethodSummary != null)
            {
                exporter.ListItem(writer, exporter.InlineBold("Summary"));
                exporter.PlainText(writer, method.MethodSummary);
            }
            //export inputs and their descriptions
            if (method.ArgumentDescriptions.Keys.Count > 0)
            {
                exporter.ListItem(writer, exporter.InlineBold("Parameters"));
                exporter.OpenList(writer);
                foreach (string argument in method.ArgumentDescriptions.Keys)
                {
                    exporter.Tab(writer, exporter.InlineListItem(
                        exporter.InlineItalic(argument) + ": " + method.ArgumentDescriptions[argument]));
                }
                exporter.CloseList(writer);
            }
            //export output
            if (method.ReturnValueDescription != null)
            {
                exporter.ListItem(writer, exporter.InlineBold("Return Value"));
                exporter.PlainText(writer, method.ReturnValueDescription);
            }
            exporter.CloseList(writer);
        }
        public static void ExportDocumentation(string outputDir, string projectName, FileFormatter exporter, List<ObjectClass> classes)
        {
            if (!outputDir.EndsWith("\\") && !outputDir.EndsWith("/"))
                outputDir += "/";

            string extension = exporter.FormatExtension();

            string outputIndexFile = outputDir + projectName + extension;

            classes = classes.OrderBy(x => x.Name).ToList();

            Directory.CreateDirectory(outputDir + projectName);
            using (StreamWriter indexWriter = File.CreateText(outputIndexFile))
            {
                exporter.OpeningSection(indexWriter);
                exporter.Title1(indexWriter, "Project: " + projectName);
                exporter.Comment(indexWriter, "This file has been automatically generated. Please do not edit it");
                exporter.Title2(indexWriter, "API Reference");
                exporter.OpenList(indexWriter);
                foreach (ObjectClass objClass in classes)
                {
                    exporter.ListItem(indexWriter
                        , exporter.InlineLink(objClass.Name, projectName + "/" + objClass.Name));
                    string outputMdFile = outputDir + projectName + "/" + objClass.Name + extension;
                    using (StreamWriter classWriter = File.CreateText(outputMdFile))
                    {
                        exporter.OpeningSection(classWriter);
                        exporter.Title1(classWriter, "Class " + objClass.Name);
                        exporter.Comment(classWriter, "Source: " + objClass.SrcFileName);

                        if (objClass.Constructors.Count > 0)
                        {
                            exporter.Title2(classWriter, "Constructors");
                            foreach (ClassMethod method in objClass.Constructors)
                                ExportMethod(classWriter, method, exporter);
                        }
                        if (objClass.Destructors.Count > 0)
                        {
                            exporter.Title2(classWriter, "Destructors");
                            foreach (ClassMethod method in objClass.Destructors)
                                ExportMethod(classWriter, method, exporter);
                        }
                        if (objClass.Methods.Count > 0)
                        {
                            exporter.Title2(classWriter, "Methods");
                            foreach (ClassMethod method in objClass.Methods)
                                ExportMethod(classWriter, method, exporter);
                        }
                        exporter.ClosingSection(classWriter);
                    }
                }
                exporter.CloseList(indexWriter);
                exporter.ClosingSection(indexWriter);
            }
        }
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
