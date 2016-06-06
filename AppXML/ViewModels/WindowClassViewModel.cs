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

        private ButtonToWindowedClassViewModel _father;
        private ClassViewModel m_Class;
        public ClassViewModel Class { get { return m_Class; }
            set { m_Class = value; }
        }

        public bool validate()
        {
            return m_Class.validate(true);
        }

        public WindowClassViewModel(string className,string name,ButtonToWindowedClassViewModel father,XmlDocument doc)
        {
            _father = father;
            m_Class = new ClassViewModel(className, name, false, doc);
            validate();

            Models.CApp.addNewClass(m_Class);

            //if (father.windowedClassVM == null)
            //{
            //    _father.windowedClassVM = new ClassViewModel(className, className, false, doc, _father);
            //    _father.windowedClassVM.validate(true);
            //    Models.CApp.addNewClass(father);
            //    //_father.ButtonColor = _father.ResumeClass.ButtonColor;

            //}

            
           
        }
        public void Save()
        {
            
           // _father.ResumeClass = this.Class;
            bool ok = validate();
            //string c = _father.ResumeClass.Multis[0].HeaderClass.Choice.Class.XMLNODE[0].SelectedOption;
            if (!ok)
            {
                _father.buttonColor = "Red";
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
                
                _father.buttonColor = "White";
                TryClose();
            }

        }

        private string obteinResume(XmlNode c, XmlNode resume)
        {

           string result = "";
           if (resume==null || !resume.HasChildNodes)
               return result;
           foreach(XmlNode rChild in resume.ChildNodes)
           {
               string rTag = rChild.Name;
               foreach(XmlNode cChild in c.ChildNodes)
               {
                   string cTag = cChild.Name;
                   if(cTag=="Literal")
                   {
                       result += cChild.InnerText;
                   }
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
