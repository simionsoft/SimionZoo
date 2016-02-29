using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AppXML.ViewModels
{
    public class XMLNodeRefViewModel:Caliburn.Micro.PropertyChangedBase
    {
        private string _label;
        private ObservableCollection<string> _options;
        private string _XMLFile;
        private string _action;

        public XMLNodeRefViewModel(string label, string file, string action )
        {
            this._label = label;
            this._XMLFile = file;
            this._action = action;

            List<string> names =AppXML.Data.Utility.getComboFromXML(file, action);
            _options = new ObservableCollection<string>(names);
            AppXML.Models.CApp.addView(this);
            
       }
        public void update()
        {
            List<string> names = AppXML.Data.Utility.getComboFromXML(_XMLFile, _action);
            _options = new ObservableCollection<string>(names);
            NotifyOfPropertyChange(() => Options);
        }
        public ObservableCollection<string> Options { get { return _options; } set { } }
        public string Label { get { return _label; } set { } }
    }
}
