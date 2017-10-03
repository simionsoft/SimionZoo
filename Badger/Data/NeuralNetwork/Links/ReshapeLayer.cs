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
    public class ReshapeLayer : LinkBase
    {
        private ReshapeLayer() : base() { }
        public ReshapeLayer(Chain parent) : base(parent) { }

        public ReshapeLayer(Chain parent, string name) : base(parent, name)
        {
            Parameters.Add(new IntTuple4DParameter("4D Shape", this));
        }

        public override string TypeName { get { return "Reshape Layer"; } }

        public override bool IsInputCompatible
        {
            get
            {
                LinkBase previousLink = GetPreviousLink();
                if (previousLink == null)
                {
                    return false;
                }

                //TODO: add real code here!
                return true;
            }
        }

        public override int? GetTensorRank()
        {
            var itp = GetParameterByName<IntTuple4DParameter>("4D Shape");
            int rank = 0;
            if (itp.Value.x1 != 0)
                rank++;
            if (itp.Value.x2 != 0)
                rank++;
            if (itp.Value.x3 != 0)
                rank++;
            if (itp.Value.x4 != 0)
                rank++;

            return rank;
        }
    }
}
