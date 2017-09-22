using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;
using Badger.Data.NeuralNetwork.Parameters;

namespace Badger.Data.NeuralNetwork.Links
{
    [Serializable]
    public class InputLayer : LinkBase
    {
        private InputLayer() : base() { }
        public InputLayer(Chain parent) : base(parent) { }

        public InputLayer(Chain parent, string name) : base(parent, name)
        {
            Parameters.Add(new InputDataParameter(this));
        }

        public override string TypeName { get { return "Input Layer"; } }

        public override bool IsInputCompatible
        {
            get
            {
                return true;
            }
        }

        public override int? GetTensorRank()
        {
            var idv = GetParameterByName<InputDataParameter>("Input Data").InputDataValue;
            if (idv == null)
                return null;

            return idv.Shape.Dimension;
        }
    }
}
