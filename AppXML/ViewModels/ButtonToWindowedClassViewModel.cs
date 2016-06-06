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
    public class ButtonToWindowedClassViewModel :ValidableAndNodeViewModel
    {
        private XmlDocument _doc;
        private string m_buttonColor= "Transparent";
        public string buttonColor { get { return m_buttonColor; } set { m_buttonColor = value; } }
        
        private ClassViewModel m_windowedClassVM;
        public ClassViewModel windowedClassVM { get { return m_windowedClassVM; } set { m_windowedClassVM = value; } }

        private string _tag;

        private string m_className;

        private string m_name = "";
        public string name { get { return m_name; } set { m_name = value; } }
        public ClassViewModel Class { get { return m_windowedClassVM; } set { m_windowedClassVM = value; } }

        public ButtonToWindowedClassViewModel(XmlNode xmlNode,XmlDocument doc)
        {
            m_name = xmlNode.Attributes["Name"].Value;
            m_className= xmlNode.Attributes["Class"].Value;

            m_windowedClassVM= new ClassViewModel(m_className,m_name,doc);
            m_windowedClassVM.validate(true);

            Models.CApp.addNewClass(m_windowedClassVM);
            _tag = xmlNode.Attributes["Name"].Value;
            m_buttonColor = "Transparent";
            _doc = doc;
        }
        public void showWindow()
        {


            WindowManager windowManager = new WindowManager();
            dynamic settings = new ExpandoObject();
            settings.WindowStyle = WindowStyle.ThreeDBorderWindow;
            settings.ShowInTaskbar = true;
            settings.Title = m_name;
            settings.WindowState = WindowState.Normal;
            settings.ResizeMode = ResizeMode.CanMinimize;


            windowManager.ShowDialog(m_windowedClassVM, null, settings);



        }
        //public void Save()
        //{

        //    // _father.ResumeClass = this.Class;
        //    bool ok = Class.validate(false);
        //    //string c = _father.ResumeClass.Multis[0].HeaderClass.Choice.Class.XMLNODE[0].SelectedOption;
        //    if (!ok)
        //    {
        //        m_buttonColor = "Red";
        //        DialogViewModel dvm = new DialogViewModel(null, "The form is not validated. Do you want to save it?", DialogViewModel.DialogType.YesNo);
        //        dynamic settings = new ExpandoObject();
        //        settings.WindowStyle = WindowStyle.ToolWindow;
        //        settings.ShowInTaskbar = true;
        //        settings.Title = "WARNING";

        //        new WindowManager().ShowDialog(dvm, null, settings);

        //        if (dvm.DialogResult == DialogViewModel.Result.OK)
        //        {
        //            //TryClose();
        //        }
        //    }
        //    else
        //    {

        //        m_buttonColor = "Transparent";
        //       // TryClose();
        //    }

        //}
        public override bool validate()
        {

            return Class.validate(true);//
        }
        public override List<XmlNode> getXmlNode()
        {
            XmlNode result = AppXML.Data.Utility.resolveTag(_tag, _doc);
            XmlNode lastChild = AppXML.Data.Utility.getLastChild(result);
            foreach (XmlNode child in Class.getXmlNodes())
            {
                if (child != null)
                    lastChild.AppendChild(child);
            }

            List<XmlNode> list = new List<XmlNode>();
            list.Add(result);
            return list;
        }
    }
}
