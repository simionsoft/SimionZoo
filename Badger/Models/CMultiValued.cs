using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.Models
{
    public class CMultiValued: CNode
    {
       
        public string klass;

        public CMultiValued(string n, string k)
        {
            name = n;
            klass = k;
        }
            
    }
}
