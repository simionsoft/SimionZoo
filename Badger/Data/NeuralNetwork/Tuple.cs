using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace Badger.Data.NeuralNetwork.Tuple
{

    [XmlInclude(typeof(IntTuple1D))]
    [XmlInclude(typeof(IntTuple2D))]
    [XmlInclude(typeof(IntTuple3D))]
    [XmlInclude(typeof(IntTuple4D))]
    [Serializable]
    public abstract class IntTupleBase
    {
        public abstract void FromString(string value, string delimiter);
        public abstract int Dimension { get; }
    }
    [Serializable]
    [XmlRoot("IntTuple1D")]
    public class IntTuple1D : IntTupleBase
    {
        public IntTuple1D() { }
        public IntTuple1D(int x1)
        {
            this.x1 = x1;
        }

        public int x1 { get; set; } = 0;

        public override int Dimension { get { return 1; } }

        public override string ToString()
        {
            return $"({x1})";
        }

        public override void FromString(string value, string delimiter)
        {
            value = value.Replace("(", "").Replace(")", "");
            x1 = int.Parse(value);
        }
    }

    [Serializable]
    public class IntTuple2D : IntTupleBase
    {
        public IntTuple2D() { }
        public IntTuple2D(int x1, int x2)
        {
            this.x1 = x1;
            this.x2 = x2;
        }

        public int x1 { get; set; } = 0;
        public int x2 { get; set; } = 0;

        public override int Dimension { get { return 2; } }

        public override string ToString()
        {
            return $"({x1}, {x2})";
        }
        public override void FromString(string value, string delimiter)
        {
            value = value.Replace("(", "").Replace(")", "");
            var values = value.Split(delimiter.ToCharArray());
            x1 = int.Parse(values[0]);
            x2 = int.Parse(values[1]);
        }
    }

    [Serializable]
    public class IntTuple3D : IntTupleBase
    {
        public IntTuple3D() { }
        public IntTuple3D(int x1, int x2, int x3)
        {
            this.x1 = x1;
            this.x2 = x2;
            this.x3 = x3;
        }

        public int x1 { get; set; } = 0;
        public int x2 { get; set; } = 0;
        public int x3 { get; set; } = 0;

        public override int Dimension { get { return 3; } }

        public override string ToString()
        {
            return $"({x1}, {x2}, {x3})";
        }

        public override void FromString(string value, string delimiter)
        {
            value = value.Replace("(", "").Replace(")", "");
            var values = value.Split(delimiter.ToCharArray());
            x1 = int.Parse(values[0]);
            x2 = int.Parse(values[1]);
            x3 = int.Parse(values[2]);
        }
    }

    [Serializable]
    public class IntTuple4D : IntTupleBase
    {
        public IntTuple4D() { }
        public IntTuple4D(int x1, int x2, int x3, int x4)
        {
            this.x1 = x1;
            this.x2 = x2;
            this.x3 = x3;
            this.x4 = x4;
        }

        public int x1 { get; set; } = 0;
        public int x2 { get; set; } = 0;
        public int x3 { get; set; } = 0;
        public int x4 { get; set; } = 0;

        public override int Dimension { get { return 4; } }

        public override string ToString()
        {
            return $"({x1}, {x2}, {x3}, {x4})";
        }

        public override void FromString(string value, string delimiter)
        {
            value = value.Replace("(", "").Replace(")", "");
            var values = value.Split(delimiter.ToCharArray());
            x1 = int.Parse(values[0]);
            x2 = int.Parse(values[1]);
            x3 = int.Parse(values[2]);
            x4 = int.Parse(values[3]);
        }
    }
}
