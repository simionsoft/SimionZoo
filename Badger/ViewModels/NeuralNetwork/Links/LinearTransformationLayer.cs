using System;
using Badger.Data.NeuralNetwork.Parameters;

namespace Badger.Data.NeuralNetwork.Links
{
    [Serializable]
    public class LinearTransformationLayer : LinkBase
    {
        private LinearTransformationLayer() : base() { }
        public LinearTransformationLayer(Chain parent) : base(parent) { }

        public LinearTransformationLayer(Chain parent, string name) : base(parent, name)
        {
            Parameters.Add(new DoubleParameter("Scale", 1.0, this));
            Parameters.Add(new DoubleParameter("Offset", 0.0, this));
        }

        public override string TypeName { get { return "Linear Transformation Layer"; } }

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
            if (GetPreviousLink() == null)
                return null;
            else
                return GetPreviousLink().GetTensorRank();
        }
    }
}