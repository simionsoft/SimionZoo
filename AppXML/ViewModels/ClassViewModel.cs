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

                }
                else if (child.Name == "BRANCH")
                {
                    //to do: cuando BranchViewModel este hecho hacer la rama

                }
            }


        }
        public ClassViewModel()
        {
            //PARAMERTRO DEL CONSTRUCTOR
            String clasName = "WORLD";
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

                }
                else if(child.Name == "BRANCH")
                {

                }
            }
            
            
        }
        public ChoiceViewModel Choice { get { return _choice; } set { } }
        public ObservableCollection<IntegerViewModel> Items { get { return _items; } set { } }
        
    }
}
