using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.Data.NeuralNetwork.Links
{
    public enum LinkType
    {
        [Description("Input Layer")]
        InputLayer,
        [Description("Activation Layer")]
        ActivationLayer,
        [Description("1D Convolution Layer")]
        Convolution1DLayer,
        [Description("2D Convolution Layer")]
        Convolution2DLayer,
        [Description("3D Convolution Layer")]
        Convolution3DLayer,
        [Description("Dense Layer")]
        DenseLayer,
        [Description("Dropout Layer")]
        DropoutLayer,
        [Description("Flatten Layer")]
        FlattenLayer,
        [Description("Reshape Layer")]
        ReshapeLayer,
        [Description("Merge Layer")]
        MergeLayer,
    }
}
