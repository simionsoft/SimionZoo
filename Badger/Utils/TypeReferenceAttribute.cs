using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.Utils
{
    public class TypeReferenceAttribute : Attribute
    {
        public Type ReferenceType { get; set; }

        public TypeReferenceAttribute(Type ReferenceType)
        {
            this.ReferenceType = ReferenceType;
        }
    }
}
