using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AppXML.ViewModels
{
    public class ProcessStateViewModel: Caliburn.Micro.PropertyChangedBase
    {
        private string label;
        private int status;
        private string sms;
        public string pipeName{get;set;}

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
            this.pipeName = (label.Split('/').Last()).Split('.').First();
        }
    }
}
