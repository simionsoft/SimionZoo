using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using System.Windows.Threading;
using System.Dynamic;
using System.Windows;
using System.Xml;

namespace AppXML.ViewModels
{
    public class WindowClassViewModel: Screen
    {

        private ClassViewModel _father;
        private ClassViewModel _result;
        private XmlDocument _doc;

        public ClassViewModel Class { get { return _father.ResumeClass; } set { _father.ResumeClass = value; } }

        public WindowClassViewModel(string className,ClassViewModel father,XmlDocument doc)
        {
            _father = father;
            _doc = doc;
            if (father.ResumeClass == null)
                _father.ResumeClass = new ClassViewModel(className, false,doc);
            
           
        }
        public void Save()
        {
            
           // _father.ResumeClass = this.Class;
            bool ok = Class.validate();
            //string c = _father.ResumeClass.Multis[0].HeaderClass.Choice.Class.XMLNODE[0].SelectedOption;
            if (!ok)
            {
                DialogViewModel dvm = new DialogViewModel(null, "The form is not validated. Do you want to save it?", DialogViewModel.DialogType.YesNo);
                dynamic settings = new ExpandoObject();
                settings.WindowStyle = WindowStyle.ToolWindow;
                settings.ShowInTaskbar = true;
                settings.Title = "WARNING";

                new WindowManager().ShowDialog(dvm, null, settings);

                if (dvm.DialogResult == DialogViewModel.Result.OK)
                {
                    TryClose();
                }
            }
            else
            {
                XmlNode c = _father.ResumeClass.getXmlNode();
                XmlNode resume = _father.ResumeClass.resume;
                string result = obteinResume(c, resume);
                string test = "_RESUME: " + result;
                _father.Resume = test;
                TryClose();
            }
        }

        private string obteinResume(XmlNode c, XmlNode resume)
        {

           string result = "";
           foreach(XmlNode rChild in resume.ChildNodes)
           {
               string rTag = rChild.Name;
               foreach(XmlNode cChild in c.ChildNodes)
               {
                   string cTag = cChild.Name;
                   if (cTag == rTag)
                   {
                       if (!rChild.HasChildNodes)
                       {
                           result += cChild.InnerText + " ";
                           return result;
                       }
                       else
                       {
                           result +=obteinResume(cChild, rChild);
                       }
                   }
                   
               }
           }
           return result;
        }
    }
}
