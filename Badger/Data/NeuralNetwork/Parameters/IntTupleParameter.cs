using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Badger.Data.NeuralNetwork.Links;
using System.Xml.Serialization;
using Badger.Data.NeuralNetwork.Tuple;

namespace Badger.Data.NeuralNetwork.Parameters
{
    [Serializable]
    public class IntTuple1DParameter : ParameterBase<IntTuple1D>
    {
        private IntTuple1DParameter() { }
        public IntTuple1DParameter(string name, LinkBase link) : base(name, new IntTuple1D(), link) { }
        public IntTuple1DParameter(string name, IntTuple1D value, LinkBase link) : base(name, value, link) { }
    }

    [Serializable]
    public class IntTuple2DParameter : ParameterBase<IntTuple2D>
    {
        private IntTuple2DParameter() { }
        public IntTuple2DParameter(string name, LinkBase link) : base(name, new IntTuple2D(), link) { }
        public IntTuple2DParameter(string name, IntTuple2D value, LinkBase link) : base(name, value, link) { }
    }

    [Serializable]
    public class IntTuple3DParameter : ParameterBase<IntTuple3D>
    {
        private IntTuple3DParameter() { }
        public IntTuple3DParameter(string name, LinkBase link) : base(name, new IntTuple3D(), link) { }
        public IntTuple3DParameter(string name, IntTuple3D value, LinkBase link) : base(name, value, link) { }
    }

    [Serializable]
    public class IntTuple4DParameter : ParameterBase<IntTuple4D>
    {
        private IntTuple4DParameter() { }
        public IntTuple4DParameter(string name, LinkBase link) : base(name, new IntTuple4D(), link) { }
        public IntTuple4DParameter(string name, IntTuple4D value, LinkBase link) : base(name, value, link) { }
    }

}
