using Badger.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.Data.NeuralNetwork.Optimizers
{
    public enum OptimizerType
    {
        [Description("SGD")]
        [TypeReference(typeof(OptimizerSGD))]
        SGD,
        [Description("Momentum SGD")]
        [TypeReference(typeof(OptimizerMomentumSGD))]
        MomentumSGD,
        [Description("Nesterov")]
        [TypeReference(typeof(OptimizerNesterov))]
        Nesterov,
        [Description("FSAdaGrad")]
        [TypeReference(typeof(OptimizerFSAdaGrad))]
        FSAdaGrad,
        [Description("Adam")]
        [TypeReference(typeof(OptimizerAdam))]
        Adam,
        [Description("AdaGrad")]
        [TypeReference(typeof(OptimizerAdaGrad))]
        AdaGrad,
        [TypeReference(typeof(OptimizerRMSProp))]
        [Description("RMSProp")]
        RMSProp,
        [TypeReference(typeof(OptimizerAdaDelta))]
        [Description("AdaDelta")]
        AdaDelta,
    }
}
