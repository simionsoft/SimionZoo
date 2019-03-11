using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimionSrcParser
{
    public abstract class FileFormatter
    {
        public enum Format { Markdown, Html };

        public static void AddIndentation(ref string definition, int level)
        {
            for (int i = 0; i < level; i++) definition += "  ";
        }

        public abstract string FormatExtension();
        public abstract void PlainText(StreamWriter writer, string input);
        public abstract void Title1(StreamWriter writer, string input);
        public abstract void Title2(StreamWriter writer, string input);
        public abstract void Title3(StreamWriter writer, string input);
        public abstract void Title4(StreamWriter writer, string input);
        public abstract void Comment(StreamWriter writer, string input);
        public abstract void OpenList(StreamWriter writer);
        public abstract void CloseList(StreamWriter writer);
        public abstract void ListItem(StreamWriter writer, string input);
        public abstract void Tab(StreamWriter writer, string input);
        public abstract void OpeningSection(StreamWriter writer);
        public abstract void ClosingSection(StreamWriter writer);
        public abstract string InlineListItem(string input);
        public abstract string InlineLink(string text, string link);
        public abstract string InlineItalic(string input);
        public abstract string InlineTab(string input);
        public abstract string InlineCode(string input);
        public abstract string InlineBold(string input);
    }
    public class MarkdownExporter : FileFormatter
    {
        public override string FormatExtension() { return ".md"; }
        public override void PlainText(StreamWriter writer, string input) { writer.WriteLine("  " + input); }
        public override void Title1(StreamWriter writer, string input) { writer.WriteLine("# " + input); }
        public override void Title2(StreamWriter writer, string input) { writer.WriteLine("## " + input); }
        public override void Title3(StreamWriter writer, string input) { writer.WriteLine("### " + input); }
        public override void Title4(StreamWriter writer, string input) { writer.WriteLine("#### " + input); }
        public override void Comment(StreamWriter writer, string input) { writer.WriteLine("> " + input); }
        public override void OpenList(StreamWriter writer) { }
        public override void CloseList(StreamWriter writer) { }
        public override void ListItem(StreamWriter writer, string input) { writer.WriteLine("* " + input); }
        public override void Tab(StreamWriter writer, string input) { writer.WriteLine("  " + input); }
        public override void OpeningSection(StreamWriter writer) { }
        public override void ClosingSection(StreamWriter writer) { }
        public override string InlineListItem(string input) { return "* " + input; }
        public override string InlineLink(string text, string link) { return "[" + text + "](" + link + ")"; }
        public override string InlineItalic(string input) { return "_" + input + "_"; }
        public override string InlineTab(string input) { return "  " + input; }
        public override string InlineCode(string input) { return "`" + input + "`"; }
        public override string InlineBold(string input) { return "*" + input + "*"; }
    }
    public class HtmlExporter: FileFormatter
    {
        public override string FormatExtension() { return ".html"; }
        public override void PlainText(StreamWriter writer, string input) { writer.WriteLine("<p>" + input + "</p>"); }
        public override void Title1(StreamWriter writer, string input) { writer.WriteLine("<h1>" + input + "</h1>"); }
        public override void Title2(StreamWriter writer, string input) { writer.WriteLine("<h2>" + input + "</h2>"); }
        public override void Title3(StreamWriter writer, string input) { writer.WriteLine("<h3>" + input + "</h3>"); }
        public override void Title4(StreamWriter writer, string input) { writer.WriteLine("<h4>" + input + "</h4>"); }
        public override void Comment(StreamWriter writer, string input) { writer.WriteLine("<em>" + input + "</em>"); }
        public override void OpenList(StreamWriter writer) { writer.WriteLine("<ul>"); }
        public override void CloseList(StreamWriter writer) { writer.WriteLine("</ul>"); }
        public override void ListItem(StreamWriter writer, string input) { writer.WriteLine("<li>" + input + "</li>"); }
        public override void Tab(StreamWriter writer, string input) { writer.WriteLine(input); }
        public override string InlineListItem(string input) { return "<li>" + input + "</li>"; }
        public override string InlineLink(string text, string link) { return "<a href=\"" + link + "\">" + text + "</a>"; }
        public override string InlineItalic(string input) { return "<i>" + input + "</i>"; }
        public override string InlineTab(string input) { return input; }
        public override string InlineCode(string input) { return "<code>" + input + "</code>"; }
        public override string InlineBold(string input) { return "<b>" + input + "</b>"; }
        public override void OpeningSection(StreamWriter writer) { writer.WriteLine("<html>\n<body>"); }
        public override void ClosingSection(StreamWriter writer) { writer.WriteLine("</body>\n</html>"); }
    }
}
