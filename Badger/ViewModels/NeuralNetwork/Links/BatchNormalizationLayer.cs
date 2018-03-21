using System;


namespace Badger.Data.NeuralNetwork.Links
{
    [Serializable]
    public class BatchNormalizationLayer : LinkBase
    {
        private BatchNormalizationLayer() : base() { }
        public BatchNormalizationLayer(Chain parent) : base(parent) { }

        public BatchNormalizationLayer(Chain parent, string name) : base(parent, name) { }

        public override string TypeName { get { return "Batch normalization Layer"; } }

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
            if (GetPreviousLink() is null)
                return null;
            return GetPreviousLink().GetTensorRank();
        }
    }
}