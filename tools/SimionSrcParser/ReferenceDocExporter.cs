using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimionSrcParser
{
    public class ReferenceDocExporter
    {
        public enum Format { Markdown, Html };

        public static string FormatExtension(Format format)
        {
            if (format == Format.Markdown) return ".md";
            else return ".html";
        }

        public static void PlainText(StreamWriter writer, string input, Format format)
        {
            if (format == Format.Markdown) writer.WriteLine("  " + input);
            else writer.WriteLine("<p>" + input + "</p>");
        }
        public static void Title1(StreamWriter writer, string input, Format format)
        {
            if (format == Format.Markdown) writer.WriteLine("# " + input);
            else writer.WriteLine("<h1>" + input + "</h1>");
        }
        public static void Title2(StreamWriter writer, string input, Format format)
        {
            if (format == Format.Markdown) writer.WriteLine("## " + input);
            else writer.WriteLine("<h2>" + input + "</h2>");
        }
        public static void Title3(StreamWriter writer, string input, Format format)
        {
            if (format == Format.Markdown) writer.WriteLine("### " + input);
            else writer.WriteLine("<h3>" + input + "</h3>");
        }
        public static void Title4(StreamWriter writer, string input, Format format)
        {
            if (format == Format.Markdown) writer.WriteLine("#### " + input);
            else writer.WriteLine("<h4>" + input + "</h4>");
        }
        public static void Comment(StreamWriter writer, string input, Format format)
        {
            if (format == Format.Markdown) writer.WriteLine("> " + input);
            else writer.WriteLine("<em>" + input + "</em>");
        }
        public static void OpenList(StreamWriter writer, Format format)
        {
            if (format == Format.Html) writer.WriteLine("<ul>");
        }
        public static void CloseList(StreamWriter writer, Format format)
        {
            if (format == Format.Html) writer.WriteLine("</ul>");
        }
        public static void ListItem(StreamWriter writer, string input, Format format)
        {
            if (format == Format.Markdown) writer.WriteLine("* " + input);
            else writer.WriteLine("<li>" + input + "</li>");
        }
        public static void Tab(StreamWriter writer, string input, Format format)
        {
            if (format == Format.Markdown) writer.WriteLine("  " + input);
            else writer.WriteLine(input);
        }
        public static string InlineListItem(string input, Format format)
        {
            if (format == Format.Markdown) return "* " + input ;
            else return "<li>" + input + "</li>";
        }
        public static string InlineLink(string text, string link, Format format)
        {
            if (format == Format.Markdown) return "[" + text + "](" + link + ")";
            else return "<a href=\"" + link + "\">" + text + "</a>";
        }
        public static string InlineItalic(string input, Format format)
        {
            if (format == Format.Markdown) return "_" + input + "_";
            else return "<i>" + input + "</i>";
        }
        public static string InlineTab(string input, Format format)
        {
            if (format == Format.Markdown) return "  " + input;
            else return input;
        }
        public static string InlineCode(string input, Format format)
        {
            if (format == Format.Markdown) return "``" + input + "``";
            else return "<code>" + input + "</code>";
        }
        public static string InlineBold(string input, Format format)
        {
            if (format == Format.Markdown) return "**" + input + "**";
            else return "<b>" + input + "</b>";
        }
        public static void OpeningSection(StreamWriter writer, Format format)
        {
            if (format == Format.Html) writer.WriteLine("<html>\n<body>");
        }
        public static void ClosingSection(StreamWriter writer, Format format)
        {
            if (format == Format.Html) writer.WriteLine("</body>\n</html>");
        }
    }
}
