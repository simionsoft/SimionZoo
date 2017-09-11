using Badger.Data.NeuralNetwork.Tuple;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace Badger.Data.NeuralNetwork
{
    public class InputData
    {
        private InputData() { }
        public InputData(string id, string name, IntTupleBase shape, string description)
        {
            ID = id;
            Name = name;
            Shape = shape;
            Description = description;
        }

        [XmlAttribute]
        public string ID { get; set; }
        [XmlAttribute]
        public string Name { get; set; }

        public IntTupleBase Shape { get; set; }
        [XmlIgnore]
        public string Description { get; set; }
    }
}
