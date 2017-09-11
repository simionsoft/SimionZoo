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
    public class DropoutLayer : LinkBase
    {
        private DropoutLayer() : base() { }
        public DropoutLayer(Chain parent) : base(parent) { }

        public DropoutLayer(Chain parent, string name) : base(parent, name)
        {
            Parameters.Add(new IntParameter("Rate", this));
        }

        public override string TypeName { get { return "Dropout Layer"; } }

        public override bool IsInputCompatible
        {
            get
            {
                LinkBase previousLink = GetPreviousLink();
                if (previousLink == null)
                {
                    return false;
                }

                return true;
            }
        }

        public override int? GetTensorRank()
        {
            return GetPreviousLink()?.GetTensorRank();
        }
    }
}
