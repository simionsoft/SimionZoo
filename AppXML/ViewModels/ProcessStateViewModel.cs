using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace AppXML.ViewModels
{
    public class ProcessStateViewModel: Caliburn.Micro.PropertyChangedBase
    {
        private string label;
        private int status;
        private string sms;
        public string pipeName{get;set;}
        private string messages;

        public string Messages { get { return messages; } set { messages = value; NotifyOfPropertyChange(() => Messages); } }
        public string OpenVisible { get { if (Messages != null && Messages != "") return "Visible"; else return "Hidden"; } set { } }

        public void addMessage(string newMessage)
        {
            messages += newMessage + "\n";
            NotifyOfPropertyChange(() => Messages);
            NotifyOfPropertyChange(() => OpenVisible);
        }

        public string Label
        {
            get { return label; }
            set
            {
                label = value;
                NotifyOfPropertyChange(() => Label);
            }
        }
        public string SMS
        {
            get { return sms; }
            set
            {
                sms = value;
                NotifyOfPropertyChange(() => SMS);
            }
        }
        public int Status
        {
            get { return status; }
            set 
            {
                status = value;
                NotifyOfPropertyChange(() => Status);
            }
        }
        
        public ProcessStateViewModel(string label)
        {
            this.label = label;
            this.status = 0;
            this.sms = "Waiting";
            this.messages = null;
            this.pipeName = (label.Split('\\').Last()).Split('.').First();
        }
        public void OpenSMS()
        {
            DialogViewModel dvm = new DialogViewModel("Process info", this.Messages, DialogViewModel.DialogType.Info);
            dynamic settings = new ExpandoObject();
            settings.WindowStyle = WindowStyle.ThreeDBorderWindow;
            settings.ShowInTaskbar = true;
            settings.Title = "Process info";
            new WindowManager().ShowWindow(dvm, null, settings);
        }
    }
}
