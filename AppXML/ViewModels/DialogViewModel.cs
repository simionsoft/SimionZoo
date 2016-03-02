using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;

namespace AppXML.ViewModels
{
    public class DialogViewModel : Screen
    {
        private string _title;
        private string _message;

        public string Title { get { return _title; } set { } }
        public string Message { get { return _message; } set { } }

        private string _cancelButtonVisibility;

        public string CancelButtonVisibility { get { return _cancelButtonVisibility; } set { } }

        public enum Result
        {
            OK,
            CANCEL,
            NotSet,

        };
        public enum DialogType
        {
            Info,
            YesNo,
        };

        private Result _result;

        public DialogViewModel(string title,string message,DialogType type)
        {
            _title = title;
            _message = message;
            _result = Result.NotSet;
            if (type == DialogType.YesNo)
                _cancelButtonVisibility = "Visible";
            else
                _cancelButtonVisibility = "Hidden";
        }

        public void OK()
        {
            _result = Result.OK;
            TryClose();
        }
        public void Cancel()
        {
            _result = Result.CANCEL;
            TryClose();
        }

        public Result DialogResult { get { return _result; } }
    }
    
}
