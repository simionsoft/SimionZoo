using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using AppXML.Models;
using System.Collections.ObjectModel;
using System.Xml;
using System.Windows.Controls;
using AppXML.Data;

namespace AppXML.ViewModels
{
    public class ClassViewModel
    {
        private ChoiceViewModel _choice;
        private ObservableCollection<IntegerViewModel> _items;
        private ObservableCollection<MultiValuedViewModel> _multis;
        private ObservableCollection<BranchViewModel> _branches;
        private XMLNodeRefViewModel _XMLNODE;

        //faltan los branches pero estan sin crear BranchViewModel y BranchView
        public ClassViewModel(string clasName)
        {
            XmlNode node = CNode.definitions[clasName];
            foreach (XmlNode child in node.ChildNodes)
            {
                if (child.Name == "CHOICE")
                {
                    _choice = new ChoiceViewModel(child);
                }
                else if (child.Name.EndsWith("VALUE"))
                {
                    if (_items == null)
                        _items = new ObservableCollection<IntegerViewModel>();
                    CIntegerValue civ = CNode.getInstance(child) as CIntegerValue;
                    IntegerViewModel ivw = new IntegerViewModel(child.Attributes["Name"].Value, civ);
                    _items.Add(ivw);

                }
                else if (child.Name == "MULTI-VALUED")
                {
                    //to do: añadir los multis a su lista y añadirlo en el xaml
                    if (_multis == null)
                        _multis = new ObservableCollection<MultiValuedViewModel>();
                    MultiValuedViewModel mvvm = new MultiValuedViewModel(child.Attributes["Name"].Value,child.Attributes["Class"].Value);
                    _multis.Add(mvvm);
                }
                else if (child.Name == "BRANCH")
                {
                   
                    if (_branches == null)
                        _branches = new ObservableCollection<BranchViewModel>();
                    BranchViewModel bvm = new BranchViewModel(child.Attributes["Name"].Value, child.Attributes["Class"].Value);
                    _branches.Add(bvm);
                }
                else if(child.Name =="XML-NODE-REF")
                {
                    string label = child.Attributes["Name"].Value;
                    string action = child.Attributes["HangingFrom"].Value;
                    string xmlfile = child.Attributes["XMLFile"].Value;

                    this._XMLNODE = new XMLNodeRefViewModel(label, xmlfile, action);
                }
            }


        }
       /* public ClassViewModel()
        {
            //PARAMERTRO DEL CONSTRUCTOR
            String clasName = "EXPERIMENT";
            Utility.getRootNode("../config/RLSimion.xml");
            //CONSTRUCTOR
            XmlNode node = CNode.definitions[clasName];
            foreach(XmlNode child in node.ChildNodes)
            {
                if(child.Name == "CHOICE")
                {
                    _choice = new ChoiceViewModel(child);
                }
                else if(child.Name.EndsWith("VALUE"))
                {
                    if (_items == null)
                        _items = new ObservableCollection<IntegerViewModel>();
                    CIntegerValue civ = CNode.getInstance(child) as CIntegerValue;
                    IntegerViewModel ivw = new IntegerViewModel(child.Attributes["Name"].Value,civ);
                    _items.Add(ivw);

                }
                else if(child.Name == "MULTI-VALUED")
                {
                    if (_multis == null)
                        _multis = new ObservableCollection<MultiValuedViewModel>();
                    MultiValuedViewModel mvvm = new MultiValuedViewModel(child.Attributes["Name"].Value, child.Attributes["Class"].Value);
                    _multis.Add(mvvm);

                }
                else if(child.Name == "BRANCH")
                {
                    if (_branches == null)
                        _branches = new ObservableCollection<BranchViewModel>();
                    BranchViewModel bvm = new BranchViewModel(child.Attributes["Name"].Value, child.Attributes["Class"].Value);
                    _branches.Add(bvm);
                }
            }
            
            
        }*/
        public string ItemsVisible { get { if(Items == null)return "Hidden";else return "Visible"; } set { } }
        public string ChoiceVisible { get { if(Choice == null)return "Hidden";else return "Visible"; } set { } }
        public string BranchesVisible { get {if(Branches == null)return "Hidden";else return "Visible"; } set { } }
        public string MultisVisible { get { if (Multis == null)return "Hidden"; else return "Visible"; } set { } }
        public ChoiceViewModel Choice { get { return _choice; } set { } }
        public ObservableCollection<IntegerViewModel> Items { get { return _items; } set { } }
        public ObservableCollection<MultiValuedViewModel> Multis { get { return _multis; } set { } }
        public ObservableCollection<BranchViewModel> Branches { get { return _branches; } set { } }
        public XMLNodeRefViewModel XMLNODE { get { return _XMLNODE; } set { } }
    }
}
