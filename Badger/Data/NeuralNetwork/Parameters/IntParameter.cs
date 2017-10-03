using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Badger.Data.NeuralNetwork.Links;

namespace Badger.Data.NeuralNetwork.Parameters
{
    public class IntParameter : ParameterBase<int>
    {
        private IntParameter() { }
        public IntParameter(string name, LinkBase link) : base(name, link) { }
        public IntParameter(string name, int value, LinkBase link) : base(name, value, link) { }
    }
}
