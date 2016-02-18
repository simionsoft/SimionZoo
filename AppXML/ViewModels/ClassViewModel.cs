using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using System.Collections.ObjectModel;
using System.Xml;
using AppXML.Models;

namespace AppXML.ViewModels
{
    public class ClassViewModel: PropertyChangedBase
    {
        private string _choiceName;
        private string _choiceLoadXML;
        private ObservableCollection<ChoiceElement> choiceElements;
        private ObservableCollection<BranchViewModel> _branches;
        private ObservableCollection<CIntegerValue> _simples;
        private ObservableCollection<CMultiValued> _multis;
        private string name;
        private XmlNode node;


        
        public ClassViewModel(String className)
        {
            if(CNode.definitions!=null)
            {
                if(CNode.definitions.ContainsKey(className))
                {
                    XmlNode node = CNode.definitions[className];
                    name = className;
                    foreach(XmlNode child in node.ChildNodes)
                    {
                        if(child.Name=="BRANCH")
                        {
                            if (_branches == null)
                                _branches = new ObservableCollection<BranchViewModel>();
                            _branches.Add(new BranchViewModel(CNode.getInstance(child)));
                        }
                        else if(child.Name=="MULTI-VALUED")
                        {
                            if (_multis == null)
                                _multis = new ObservableCollection<CMultiValued>();
                            _multis.Add((CNode.getInstance(child) as CMultiValued));
                        }
                        else if(child.Name.EndsWith("VALUE"))
                        {
                            if (_simples == null)
                                _simples = new ObservableCollection<CIntegerValue>();
                            _simples.Add((CNode.getInstance(child) as CIntegerValue));
                        }
                        else if(child.Name=="CHOICE")
                        {
                            _choiceName = child.Attributes["Name"].Value;
                            if(child.Attributes["LoadXML"]!=null)
                                _choiceLoadXML = child.Attributes["LoadXML"].Value;
                            foreach(XmlNode nodo in child.ChildNodes)
                            {
                                if(nodo.Name=="CHOICE-ELEMENT")
                                {
                                    if (choiceElements == null)
                                        choiceElements = new ObservableCollection<ChoiceElement>();
                                    ChoiceElement ce = new ChoiceElement();
                                    ce.name = nodo.Attributes["Name"].Value;
                                    if(nodo.Attributes["XML"]!=null)
                                        ce.XML = nodo.Attributes["XML"].Value;
                                    foreach(XmlNode childNode in nodo.ChildNodes)
                                    {
                                        if (childNode.Name == "BRANCH")
                                        {
                                            if (ce.branches==null)
                                                ce.branches = new List<CBranch>();
                                            ce.branches.Add((CNode.getInstance(childNode) as CBranch));
                                        } 
                                        else if(childNode.Name == "MULTI-VALUED")
                                        {
                                            if (ce.multis == null)
                                                ce.multis = new List<CMultiValued>();
                                            ce.multis.Add((CNode.getInstance(childNode) as CMultiValued));
                                        }
                                        else if(childNode.Name.EndsWith("VALUE"))
                                        {
                                            if (ce.simples == null)
                                                ce.simples = new List<CIntegerValue>();
                                            ce.simples.Add((CNode.getInstance(childNode) as CIntegerValue));

                                        }
                                    }
                                    this.choiceElements.Add(ce);
                                }
                            }

                        }
                    }
                }
            }
        }
    }
}
