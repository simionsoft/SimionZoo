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

namespace AppXML.ViewModels
{
    public class IntegerViewModel: Caliburn.Micro.PropertyChangedBase
    {
       
        private ObservableCollection<TextBox> _textBox;
        private ObservableCollection<ComboBox> _comboBox;
        private ObservableCollection<TextBoxWithFile> _textBoxFile;
       
        public IntegerViewModel(String label, CIntegerValue param)
        {
            if(param.type== validTypes.DirPathValue)
            {
                TextBoxWithFile t1 = new TextBoxWithFile(label, param.defaultValue, "images/folder.jpg", "folder");
                List<TextBoxWithFile> t = new List<TextBoxWithFile>();
                t.Add(t1);
                _textBoxFile = new ObservableCollection<TextBoxWithFile>(t);
            }
            else if (param.type==validTypes.FilePathValue)
            {
                TextBoxWithFile t1 = new TextBoxWithFile(label, param.defaultValue, "images/file.jpg", "file");
                List<TextBoxWithFile> t = new List<TextBoxWithFile>();
                t.Add(t1);
                _textBoxFile = new ObservableCollection<TextBoxWithFile>(t);
            }
            else if(param.type==validTypes.IntergerValue || param.type == validTypes.StringValue || param.type == validTypes.DecimalValue)
            {
                TextBox t1 = new TextBox(label, param.defaultValue,param.type);
                List<TextBox> t = new List<TextBox>();
                t.Add(t1);
                _textBox = new ObservableCollection<TextBox>(t);
            }
            else if (param.type == validTypes.EnumValue)
            {
                List<string> values = Data.Utility.getEnumItems(CIntegerValue.definitions[param.enumClas]);
                ComboBox cb = new ComboBox(label, values);
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
               NotifyOfPropertyChange(()=>ComboBox);
           }
       }
       public ObservableCollection<TextBoxWithFile> TextBoxFile
       { 
           get { return _textBoxFile; } set { } }
    }
        
    
    public class TextBox: PropertyChangedBase
    {
        private string label;
        private string defaultValue;
        private validTypes type;
        private string _textColor = "White";

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
        public string Default { get { return this.defaultValue; } 
            set { 
                    defaultValue = value;
                    NotifyOfPropertyChange(() => Default); 
                    
                } 
        }
        public TextBox(string label,string defaultValue, validTypes tipo)
        {
            this.label = label;
            this.defaultValue = defaultValue;
            type = tipo;
        }
        public string TextColor { get { return _textColor; } set { _textColor = value; NotifyOfPropertyChange(() => TextColor); } }
        public void Validate(object sender)
        {
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
        private string type;

        public TextBoxWithFile(string label,string def, string bt, string type):base(label,def,validTypes.FilePathValue)
        {
            buttonImage = bt;
            this.type = type;

        }
        public string ButtonImage { get { return buttonImage; } set { buttonImage = value; } }
        public string Type
        {
            get { return type; }
            set { type = value; }
           
        }
        public void FindPath()
        {
           
            if(type=="file")
            {
                Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();
                 openFileDialog.Filter = "Text Files (.txt)|*.txt|All Files (*.*)|*.*";
                 if (openFileDialog.ShowDialog() == true)
                    this.Default = openFileDialog.FileName;
            }  
            else if(type=="folder")
            {
                FolderBrowserDialog fbd = new FolderBrowserDialog();
                if (fbd.ShowDialog() == DialogResult.OK)
                    {
                        this.Default = fbd.SelectedPath;
                    }
                    
            }
                
            

        }
    }
    public class ComboBox 
    {
        private string label;
        private ObservableCollection<string> _comboValues;

            public ComboBox(string label, List<string> values)
            {
                this.label = label;
                _comboValues = new ObservableCollection<string>(values);             
                
            }
            public string Label { get { return label; } set { } }
            public ObservableCollection<string> ComboValues { get { return _comboValues; } set { } }
    }
}
