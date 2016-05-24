using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Input;
using Microsoft.Win32;
using System.IO;
using System.Windows.Forms;
using AppXML.Models;
using System.Xml;

namespace AppXML.ViewModels
{
    public class IntegerViewModel: ValidableAndNodeViewModel
    {
       
        private ObservableCollection<TextBox> _textBox;
        private ObservableCollection<ComboBox> _comboBox;
        private ObservableCollection<TextBoxWithFile> _textBoxFile;
        private bool isOptional = false;
        public bool IsOptional { get { return isOptional; } set { isOptional = value; } }
        private string _comment;
        public string Comment { get { return _comment; } set { } }
        private XmlDocument _doc;
        private string _tag;
        public string Tag { get { return _tag; } }
        public string Value
        {
            get { return ""; } 
            set 
            {
                if (this.ComboBox != null)
                {
                    ComboBox[0].SelectedComboValue = value;
                   
                }
                else if (this.TextBoxFile != null)
                {
                    TextBoxFile[0].Default = value;
                   
                }  
                else if (this.TextBox != null)
                {
                    TextBox[0].Default = value;
                   
                    
                }
                    
                
            } 
        }


        public IntegerViewModel(String label, CIntegerValue param, XmlDocument doc, string tag)
        {
            _doc = doc;
            isOptional = param.isOptional;
            _comment = param.comment;
            if (tag == null || tag == "")
                _tag = label;
            else
                _tag = tag;
            if(param.type== validTypes.DirPathValue)
            {
                TextBoxWithFile t1 = new TextBoxWithFile(label, param.defaultValue, "images/folder.jpg", "folder",_comment,param.isOptional);
                List<TextBoxWithFile> t = new List<TextBoxWithFile>();
                t.Add(t1);
                _textBoxFile = new ObservableCollection<TextBoxWithFile>(t);
            }
            else if (param.type==validTypes.FilePathValue)
            {
                TextBoxWithFile t1 = new TextBoxWithFile(label, param.defaultValue, "images/file.jpg", "file",_comment,param.isOptional);
                List<TextBoxWithFile> t = new List<TextBoxWithFile>();
                t.Add(t1);
                _textBoxFile = new ObservableCollection<TextBoxWithFile>(t);
            }
            else if(param.type==validTypes.IntergerValue || param.type == validTypes.StringValue || param.type == validTypes.DecimalValue)
            {
                TextBox t1 = new TextBox(label, param.defaultValue,param.type,_comment,param.isOptional);
                List<TextBox> t = new List<TextBox>();
                t.Add(t1);
                _textBox = new ObservableCollection<TextBox>(t);
            }
            else if (param.type == validTypes.EnumValue)
            {
                List<string> values = Data.Utility.getEnumItems(CIntegerValue.definitions[param.enumClas]);
                ComboBox cb = new ComboBox(label, values,param.defaultValue,_comment,param.isOptional);
                List<ComboBox> uuuu = new List<ViewModels.ComboBox>();
                uuuu.Add(cb);
                _comboBox = new ObservableCollection<ComboBox>(uuuu);
            }
            
           

        }
      
       
       public ObservableCollection<TextBox> TextBox
        {
            get
            {
                return _textBox;
            }
            set
            {
                _textBox = value;
                NotifyOfPropertyChange(() => TextBox);
            }
        }
       public ObservableCollection<ComboBox> ComboBox
       {
           get
           {
               return _comboBox;
           }
           set
           {
               _comboBox = value;
               NotifyOfPropertyChange(()=>ComboBox);
           }
       }
       public ObservableCollection<TextBoxWithFile> TextBoxFile
       {
           get { return _textBoxFile; }
           set { _textBoxFile = value; NotifyOfPropertyChange(() => TextBoxFile); }
       }

       public bool validateIntegerViewModel()
       {
           
            if(_textBox!=null)
            {
                return _textBox[0].validate();
            }
            if(_comboBox!=null)
            {
               return _comboBox[0].validate();
            }
            if(_textBoxFile!=null)
            {
                return _textBoxFile[0].validate();
            }
            return false;
       }
        public bool validateIntegerViewModel(bool isOptional)
        {
            this.isOptional = isOptional;
            return validateIntegerViewModel();
           
        }
        public override bool validate()
        {
            return validateIntegerViewModel();
        }
        public override List<XmlNode> getXmlNode()
        {
            XmlNode result = AppXML.Data.Utility.resolveTag(_tag, _doc);
            XmlNode lastChild = AppXML.Data.Utility.getLastChild(result);
            if(_textBox!=null)
            {
                lastChild.InnerText = _textBox[0].Default;
            }
            if(_comboBox!=null)
            {
               lastChild.InnerText = _comboBox[0].SelectedComboValue;
            }
            if(_textBoxFile!=null)
            {
               lastChild.InnerText = _textBoxFile[0].defaultValue;
            }
            if (lastChild.InnerText =="")
                return null;
            List<XmlNode> list = new List<XmlNode>();
            list.Add(result);
            return list;
            
        }
    }
        
    
    public class TextBox: PropertyChangedBase
    {
        private string label;
        public string defaultValue;
        public validTypes type;
        private string _textColor = "White";
        protected bool isOptional;
        private string _comment;
        public string Comment { get { return _comment; } set { } }

        public bool validate()
        {
            if(_textColor=="Red")
            {
                return false;
            }
            if((Default==null || Default=="") && !isOptional)
            {
                return false;
            }
            return true;
        }


        public string Label
        {
            get
            {
                return label;
            }
            set
            {
                label = value;
            }
        }
        public string Default { get { return defaultValue; } 
            set { 
                    defaultValue = value;
                    NotifyOfPropertyChange(() => Default); 
                    
                } 
        }
        public TextBox(string label,string defaultValue, validTypes tipo,string comment,bool optional)
        {
            this.label = label;
            this.defaultValue = defaultValue;
            type = tipo;
            isOptional = optional;
            this._comment = comment;
        }
        public string TextColor { get { return _textColor; } set { _textColor = value; NotifyOfPropertyChange(() => TextColor); } }
        public void Validate(object sender)
        {
            if (type == validTypes.StringValue)
                return;
            if(isOptional==true && (Default==null || Default==""))
            {
                TextColor = "White";
                return;
            }
            Boolean isValid = AppXML.Data.Utility.validate(Default, type);
            if(!isValid)
            {
                TextColor = "Red";
            }
            else
            {
                TextColor = "White";
            }
        }
       
        
    }
    public class TextBoxWithFile:TextBox
    {
        private string buttonImage;
        public string copyDefault;
        public string Color { get { return color; } set { color = value; NotifyOfPropertyChange(() => Color); } }
        private string color;
        new public string Default 
        { 
            get 
            {
                return AppXML.Data.Utility.getFileName(defaultValue);
            } 
            set 
            {
                defaultValue = value;
                NotifyOfPropertyChange(() => Default);
            } 
        }

        public new bool validate()
        {
            if (!isOptional && (Default == null || Default == ""))
                return true;
            string tmp = defaultValue;
            if (!Path.IsPathRooted(tmp))
            {

                tmp = Path.Combine(Directory.GetCurrentDirectory(), tmp);
                tmp = System.IO.Path.GetFullPath(tmp);
            }
            if (type == validTypes.FilePathValue && !File.Exists(tmp))
            {
                this.TextColor = "Red";
                return false;
                
            }
            if (type == validTypes.DirPathValue && !Directory.Exists(tmp))
            {
                this.TextColor = "Red";
                return false;
            }   
            this.TextColor = "White";
            return true;
        }

        public TextBoxWithFile(string label,string def, string bt, string type,string comment, bool optional):base(label,def,validTypes.FilePathValue,comment,optional)
        {
            buttonImage = bt;
            if (type == "folder")
                this.type = validTypes.DirPathValue;
            else
                this.type = validTypes.FilePathValue;
            copyDefault = defaultValue;
            if (Default!=null && Default.Contains("*."))
                Default = "";

        }
        public string ButtonImage { get { return buttonImage; } set { buttonImage = value; } }
        public validTypes Type
        {
            get { return type; }
            set { type = value; }
           
        }
        public void FindPath()
        {
           
            if(type==validTypes.FilePathValue)
            {
                Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();
                if(copyDefault!=null && copyDefault!="")
                {
                    string extension = copyDefault.Split('.').Last();
                    string filter = "File (." + extension + ")|*." + extension + "|All Files (*.*)|*.*";
                    openFileDialog.Filter = filter;
                   // if(File.Exists(copyDefault))
                    if(!copyDefault.Contains("*."))
                    {
                        //bool isAbsolute = Path.IsPathRooted(copyDefault);
                        //if (isAbsolute)
                         //   openFileDialog.InitialDirectory = Path.GetDirectoryName(copyDefault);
                        //else
                            openFileDialog.InitialDirectory = Path.GetDirectoryName(System.IO.Path.GetFullPath(Path.Combine(Directory.GetCurrentDirectory(), copyDefault)))
;
                    }
                    else //if(copyDefault.Contains("*."))
                    {
                        string dirPath = copyDefault.Split('*').First();
                        if(Directory.Exists(dirPath))
                        {
                            //bool isAbsolute = Path.IsPathRooted(copyDefault);
                            //if (isAbsolute)
                            //    openFileDialog.InitialDirectory = dirPath;
                            //else
                                openFileDialog.InitialDirectory = Path.GetDirectoryName(System.IO.Path.GetFullPath(Path.Combine(Directory.GetCurrentDirectory(), dirPath)));
                        }
                        
                    }
                }             
               
                 if (openFileDialog.ShowDialog() == true)
                 {

                     this.Default = Data.Utility.GetRelativePathTo(Directory.GetCurrentDirectory(), openFileDialog.FileName);
                     NotifyOfPropertyChange(() => Default);
                     this.TextColor = "White";
                 }
                   
            }  
            else if(type==validTypes.DirPathValue)
            {
                FolderBrowserDialog fbd = new FolderBrowserDialog();
                if (copyDefault != null && Directory.Exists(copyDefault))
                {
                    if (Path.IsPathRooted(copyDefault))
                        fbd.SelectedPath = copyDefault;
                    else
                        fbd.SelectedPath = System.IO.Path.GetFullPath(Path.Combine(Directory.GetCurrentDirectory(), Default));
                }
                    
                if (fbd.ShowDialog() == DialogResult.OK)
                    {
                        this.Default = Data.Utility.GetRelativePathTo(Directory.GetCurrentDirectory(), fbd.SelectedPath);
                        NotifyOfPropertyChange(() => Default);

                        this.TextColor = "White";

                    }
                    
            }
                
            

        }
    }
    public class ComboBox : PropertyChangedBase
    {
        private string label;
        private bool isOptional;
        private ObservableCollection<string> _comboValues;
        private string _comment;
        public string Comment { get { return _comment; } set { } }

        public ComboBox(string label, List<string> values,string defaultValue, string comment,bool optional)
        {
            this._comment = comment;
            this.label = label;
            isOptional = optional;
            if (defaultValue != null && defaultValue != "" && values.Contains(defaultValue))
                SelectedComboValue = defaultValue;
            _comboValues = new ObservableCollection<string>(values);
            // SelectedComboValue = _comboValues[0];
                
        }
        public string Label { get { return label; } set { } }
        public ObservableCollection<string> ComboValues { get { return _comboValues; } set { } }

        private string _selectedString;
        public string SelectedComboValue
        {
            get { return _selectedString; }
            set
            {
                _selectedString = value;
                NotifyOfPropertyChange(() => SelectedComboValue);
                // and do anything else required on selection changed
            }
        }
       
        public bool validate()
        {
            if ((_selectedString == null || _selectedString == "") && !isOptional)
                return false;
            else return true;
        }
    }
}
