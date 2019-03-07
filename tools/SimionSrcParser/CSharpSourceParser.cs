using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace SimionSrcParser
{
    class CSharpSourceParser: SimionSrcParser
    {
        void ParseClass(string filename, string namespaceName, string className, string classDefinition)
        {
            //We only process comments starting with ///
            //and public methods
            string sPattern = @"(\s*///[^\r\n]+\r*\n*)+\s*(?:public|protected)?\s+(?:async\s+)?(?:static\s+)?(\w+)\s+(\w+)\(([^\)]*)\)";
            string methodName, returnType, arguments;
            CaptureCollection comments;
            foreach (Match match in Regex.Matches(classDefinition, sPattern))
            {
                comments = match.Groups[1].Captures;
                returnType = match.Groups[2].Value;
                methodName = match.Groups[3].Value;
                arguments = match.Groups[4].Value;

                string fullClassName = namespaceName + "." + className;
                ObjectClass objClass = ParsedObjectClasses.Find(c => c.Name == fullClassName);
                if (objClass == null)
                {
                    objClass = new ObjectClass(filename, fullClassName);
                    ParsedObjectClasses.Add(objClass);
                }
                objClass.AddMethod(new ClassMethod(methodName, comments, arguments, returnType, ClassMethod.MethodType.Regular));
            }
        }
        void ParseNamespace(string filename, string namespaceName, string namespaceContent)
        {
            //Extract every namespace
            string extractNamespaceExpReg = @"(?:public)?\s+(?:static\s+)?class\s+(\w+)\s*(?:\:\s*[\w\.]+(?:\s*,\s*\w+)?)?[\r\n\s]*\{((?:[^{}]|(?<open>\{)|(?<-open>\}))+(?(open)(?!)))\}";
            foreach (Match match in Regex.Matches(namespaceContent, extractNamespaceExpReg))
            {
                string className = match.Groups[1].Value;
                //if the class inherits remove that part
                int pos = className.IndexOf(':');
                if (pos > 0)
                    className = className.Substring(0, pos);
                string classContent = match.Groups[2].Value;

                ParseClass(filename, namespaceName, className, classContent);
            }
        }
        void ParseSrcCode(string filename, string content)
        {
            //Extract every namespace
            string extractNamespaceExpReg = @"namespace\s+([\w\.]+)\s*\r*\n*\{((?:[^{}]|(?<open>\{)|(?<-open>\}))+(?(open)(?!)))\}";
            foreach (Match match in Regex.Matches(content, extractNamespaceExpReg))
            {
                string namespaceName = match.Groups[1].Value;
                string namespaceContent = match.Groups[2].Value;

                ParseNamespace(filename, namespaceName, namespaceContent);
            }
        }
        void ParseSrcFile(string filename)
        {
            string originalFileContents = File.ReadAllText(filename, Encoding.UTF8);

            numCharsProcessed += originalFileContents.Length;

            ParseSrcCode(Herd.Utils.GetFilename(filename), originalFileContents);
        }
        public override void ParseSourceFilesInDir(string inputDir)
        {
            //Parse .cpp files for constructor and factory definition
            List<string> sourceFiles = new List<string>(Directory.EnumerateFiles(inputDir, "*.cs"
                , SearchOption.AllDirectories));
            List<string> ignorePatterns = new List<string> { ".xaml.cs", "\\obj\\" };
            foreach (string ignorePattern in ignorePatterns)
                sourceFiles.RemoveAll(file => file.Contains(ignorePattern));
            foreach (var file in sourceFiles)
            {
                ParseSrcFile(file);
            }
        }

        public override List<ParameterizedObject> GetParameterizedObjects()
        {
            return null;
        }
        public override int PostProcess()
        {
            return 0;
        }
        List<ObjectClass> ParsedObjectClasses= new List<ObjectClass>();
        public override List<ObjectClass> GetObjectClasses()
        {
            return ParsedObjectClasses;
        }
        int numCharsProcessed = 0;
        public override int GetNumBytesProcessed()
        {
            return numCharsProcessed;
        }
    }
}
