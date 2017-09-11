using Badger.Data.NeuralNetwork.Links;
using Badger.Data.NeuralNetwork.Optimizers;
using Badger.Utils;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace Badger.Data.NeuralNetwork
{
    public class OptimizerSetting
    {
        private OptimizerSetting() { }
        public OptimizerSetting(OptimizerType value)
        {
            Value = value;
        }

        private OptimizerBase _optimizer;
        public OptimizerBase Optimizer
        {
            get
            {
                return _optimizer;
            }
            set
            {
                _optimizer = value;
                if (_optimizer != null)
                {
                    if (_optimizer.Type != Value)
                        Value = _optimizer.Type;
                }
            }
        }

        private OptimizerType _value;

        [XmlIgnore]
        public OptimizerType Value
        {
            get
            {
                return _value;
            }
            set
            {
                _value = value;
                var type = value.GetType();
                type = ((TypeReferenceAttribute)type.GetMember(value.ToString())[0].GetCustomAttributes(typeof(TypeReferenceAttribute), false)[0]).ReferenceType;
                if (Optimizer == null || (Optimizer != null && Optimizer.GetType() != type))
                {
                    Optimizer = (OptimizerBase)Activator.CreateInstance(type);
                    Optimizer.InitializeParameters();
                }
            }
        }
    }
}