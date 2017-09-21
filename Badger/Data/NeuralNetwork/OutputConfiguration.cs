using Badger.Data.NeuralNetwork.Parameters;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.Data.NeuralNetwork
{
    public class OutputConfiguration
    {
        public OutputConfiguration() { }
        public OutputConfiguration(LinkConnection linkConnection)
        {
            LinkConnection = linkConnection;
        }

        public LinkConnection LinkConnection { get; set; }
    }
}
