using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;
using Badger.Data.NeuralNetwork.Links;

namespace Badger.Data.NeuralNetwork.Parameters
{
    public abstract class ParameterBase
    {
        protected ParameterBase() { }
        protected ParameterBase(LinkBase link)
        {
            Parent = link;
        }
        public ParameterBase(string name, LinkBase link) : this(link)
        {
            Name = name;
        }
        [XmlAttribute]
        public string Name { get; set; }
        [XmlIgnore]
        public LinkBase Parent { get; set; }
    }

    [XmlInclude(typeof(ActivationFunctionParameter))]
    [XmlInclude(typeof(IntParameter))]
    [XmlInclude(typeof(DoubleParameter))]
    [XmlInclude(typeof(IntTuple1DParameter))]
    [XmlInclude(typeof(IntTuple2DParameter))]
    [XmlInclude(typeof(IntTuple3DParameter))]
    [XmlInclude(typeof(IntTuple4DParameter))]
    [XmlInclude(typeof(InputDataParameter))]
    [XmlInclude(typeof(LinkConnectionListParameter))]
    [Serializable]
    public abstract class ParameterBase<T> : ParameterBase
    {
        protected ParameterBase() { }
        protected ParameterBase(LinkBase link) : base(link) { }
        public ParameterBase(string name, T value, LinkBase link) : this(name, link)
        {
            Value = value;
        }
        public ParameterBase(string name, LinkBase link) : base(name, link) { }

        public T Value { get; set; }
    }
}
