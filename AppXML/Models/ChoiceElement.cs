using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AppXML.Models
{
    public class ChoiceElement
    {
        public string name;
        public string XML;
        public string clas;

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
