using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.Models
{
    public class ChoiceElement
    {
        public string name;
        public string XML;
        public string clas;
        public string tag;

        public override string ToString()
        {
            return name;
        }
        public ChoiceElement(string name, string clas)
        {
            this.name = name;
            this.clas = clas;
        }
        
    }
}
