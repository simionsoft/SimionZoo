using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NewForm
{
    public class Branch
    {
        string name;
        string type;

        public Branch(string name, string type)
        {
            this.name = name;
            this.type = type;
        }

        public override string ToString()
        {
            return this.name;
        }
    }
}
