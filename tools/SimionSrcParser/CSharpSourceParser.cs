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
            string sPattern = @"(\s*///[^\r\n]+\r*\n*)+\s*public\s+(?:static\s+)?(\w+)\s+(\w+)\(([^\)]*)\)";
            string methodName, returnType, arguments;
            CaptureCollection comments;
            foreach (Match match in Regex.Matches(classDefinition, sPattern))
            {
                comments = match.Groups[1].Captures;
                returnType = match.Groups[2].Value;
                className = match.Groups[3].Value;
                methodName = match.Groups[4].Value;
                arguments = match.Groups[5].Value;

                ObjectClass objClass = ParsedObjectClasses.Find(c => c.Name == className);
                if (objClass == null)
                {
                    objClass = new ObjectClass(filename, namespaceName + "." + className);
                    ParsedObjectClasses.Add(objClass);
                }
                objClass.AddMethod(new ClassMethod(methodName, comments, arguments, returnType, ClassMethod.MethodType.Regular));
            }
        }
        void ParseNamespace(string filename, string namespaceName, string namespaceContent)
        {
            //Extract every namespace
            string extractNamespaceExpReg = @"(?:public)?\s+(?:static\s+)?class\s+(\w+)\s*\{((?:[^{}]|(?<open>\{)|(?<-open>\}))+(?(open)(?!)))\}";
            foreach (Match match in Regex.Matches(namespaceContent, extractNamespaceExpReg))
            {
                string className = match.Groups[1].Value;
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
