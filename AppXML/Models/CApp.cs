using AppXML.ViewModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace AppXML.Models
{
    public class CApp:CNode
    {
        private static List<XMLNodeRefViewModel> viewsWithNodeRef = new List<XMLNodeRefViewModel>();
        private static List<ClassViewModel> newClassWithRefs = new List<ClassViewModel>();
        public static bool IsInitializing { get; set; }
        public static string EXE;


        public static void addNewClass(ClassViewModel newClass)
        {
            newClassWithRefs.Add(newClass);
        }
        public static void cleanApp()
        {
            viewsWithNodeRef.Clear();
            newClassWithRefs.Clear();
            EXE = null;
        }

        public static void addView(XMLNodeRefViewModel view)
        {
            if (!viewsWithNodeRef.Contains(view))
                viewsWithNodeRef.Add(view);
        }
        public static void removeViews(List<XMLNodeRefViewModel> views)
        {
            foreach(XMLNodeRefViewModel view in views)
            {
                removeView(view);   
            }
        }
        public static void removeView(XMLNodeRefViewModel view)
        {
            viewsWithNodeRef.Remove(view);
           
        }
        public static void updateViews()
        {
            foreach (XMLNodeRefViewModel view in viewsWithNodeRef)
                view.update();
            foreach (ClassViewModel view in newClassWithRefs)
                view.validate(false);
            
        }
        public CApp(XmlNode node, string n) 
        { 
            name = n;
            _document = new XmlDocument();
         

        }

        private XmlDocument _document;
        public XmlDocument document { get { return _document; } private set { } }
        

        public static void removeFromNew(ClassViewModel classViewModel)
        {
            if (newClassWithRefs.Contains(classViewModel))
                newClassWithRefs.Remove(classViewModel);
        }
    }
}
