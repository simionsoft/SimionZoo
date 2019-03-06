using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Xml;

namespace SimionSrcParser
{
    public class ClassMethod
    {
        public enum MethodType {Constructor, Destructor, Regular };

        public MethodType Type { get; } = MethodType.Regular;
        public string Name { get; } = null;
        public string ReturnType { get; } = null;
        public string Arguments { get; } = null;

        public string ReturnValueDescription;
        public Dictionary<string, string> ArgumentDescriptions = new Dictionary<string, string>();
        public string MethodSummary { get; set; } = null;

        void ParseComments(CaptureCollection comments)
        {
            char[] trimmedChars = new char[] { '\n','\r','/',' ' };
            string fullCommentXml = "<doc>"; //we need a root node
            //Remove '///', spaces and new-line characters from line endings and beginnings
            foreach(Capture capture in comments)
            {
                fullCommentXml+= capture.Value.Trim(trimmedChars) + " ";
            }
            fullCommentXml += "</doc>";

            XmlDocument doc = new XmlDocument();
            doc.LoadXml(fullCommentXml);

            //Add summary if there is one
            XmlNode summary = doc.SelectSingleNode("doc/summary");
            if (summary != null)
                MethodSummary = summary.InnerText.Trim(trimmedChars);

            //Add input argument descriptions if there are
            foreach (XmlNode input in doc.SelectNodes("doc/param"))
            {
                string name = input.Attributes["name"].Value;
                string description = input.InnerText.Trim(trimmedChars);
                ArgumentDescriptions[name]=  description;
            }

            //Add return description if there is one
            XmlNode returnDescription = doc.SelectSingleNode("doc/returns");
            if (returnDescription != null)
                ReturnValueDescription = summary.InnerText.Trim(trimmedChars);
        }

        public ClassMethod(string name, CaptureCollection comments, string arguments, string returnType, MethodType type = MethodType.Regular)
        {
            Name = name;
            Arguments = arguments;
            ReturnType = returnType;
            Type = type;

            ParseComments(comments);
        }
    }
}