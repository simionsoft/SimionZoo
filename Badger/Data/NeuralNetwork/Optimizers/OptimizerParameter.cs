using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.Data.NeuralNetwork.Optimizers
{
    public class OptimizerParameter<K, V>
    {
        public OptimizerParameter() { }
        public OptimizerParameter(K key, V value)
        {
            Key = key;
            Value = value;
        }

        public K Key { get; set; }
        private V _value;
        public V Value
        {
            get
            {
                return _value;
            }
            set
            {
                _value = value;
            }
        }
    }
}
