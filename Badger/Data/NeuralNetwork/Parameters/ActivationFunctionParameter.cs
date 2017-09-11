using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Badger.Data.NeuralNetwork.Links;

namespace Badger.Data.NeuralNetwork.Parameters
{
    public class ActivationFunctionParameter : ParameterBase<ActivationFunction>
    {
        private ActivationFunctionParameter() { }
        public ActivationFunctionParameter(string name, LinkBase link) : base(name, link) { }
        public ActivationFunctionParameter(string name, ActivationFunction value, LinkBase link) : base(name, value, link) { }
    }

    public enum ActivationFunction
    {
        [Description("Sigmoid")]
        sigmoid = 0,
        [Description("Exponential Linear Unit")]
        elu,
        [Description("Scaled Exponential Linear Unit")]
        selu,
        [Description("Softplus")]    
        softplus,
        [Description("Softsign")]
        softsign,
        [Description("Rectified Linear Unit")]
        relu,
        [Description("Tanh")]
        tanh,
        [Description("Hard Sigmoid")]
        hard_sigmoid,
        [Description("Softmax")]
        softmax,
        [Description("Linear")]
        linear
    }
}
