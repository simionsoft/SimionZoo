using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;
using System.Windows;

namespace AppXML.ViewModels
{
        public class ShellViewModel: PropertyChangedBase {
        string name;
        public string Name {
            get { return name; }
            set {
                name = value;
                NotifyOfPropertyChange(() => Name);
                NotifyOfPropertyChange(() => CanSayHello);
            }
        }

        public bool CanSayHello {
            get { return !string.IsNullOrWhiteSpace(Name); }
        }

        public void SayHello() {
            MessageBox.Show(string.Format("Hello {0}!", Name)); //Don't do this in real life :)
        }
    }
    
}
