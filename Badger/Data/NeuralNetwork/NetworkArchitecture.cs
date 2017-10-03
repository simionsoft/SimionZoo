using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace Badger.Data.NeuralNetwork
{
    public class NetworkArchitecture : IDeserializationCallback
    {
        public NetworkArchitecture(Problem problem) : this()
        {
            Problem = problem;
        }
        public NetworkArchitecture()
        {
            Chains = new ObservableCollection<Chain>();
        }

        public ObservableCollection<Chain> Chains { get; }

        [XmlIgnore]
        public Problem Problem { get; set; }

   
        public void OnDeserialization(object sender)
        {
            foreach (var chain in Chains)
            {
                chain.NetworkArchitecture = this;
            }
        }
    }
}
