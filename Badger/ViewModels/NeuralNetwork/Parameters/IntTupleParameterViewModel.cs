using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Badger.Data.NeuralNetwork.Parameters;
using Badger.Data.NeuralNetwork.Tuple;
using Badger.ViewModels.NeuralNetwork.Links;

namespace Badger.ViewModels.NeuralNetwork.Parameters
{
    class IntTuple1DParameterViewModel : ParameterBaseViewModel
    {
        public IntTuple1DParameter IntTuple1DParameterData { get; }
        public IntTuple1DParameterViewModel(IntTuple1DParameter intTuple1DParameter, LinkBaseViewModel parent) : base(intTuple1DParameter, parent)
        {
            IntTuple1DParameterData = intTuple1DParameter;
        }

        public IntTuple1D Value
        {
            get { return IntTuple1DParameterData.Value; }
            set
            {
                IntTuple1DParameterData.Value = value;
                NotifyOfPropertyChange(() => Value);
            }
        }
    }

    class IntTuple2DParameterViewModel : ParameterBaseViewModel
    {
        public IntTuple2DParameter IntTuple2DParameterData { get; }
        public IntTuple2DParameterViewModel(IntTuple2DParameter intTuple2DParameter, LinkBaseViewModel parent) : base(intTuple2DParameter, parent)
        {
            IntTuple2DParameterData = intTuple2DParameter;
        }

        public IntTuple2D Value
        {
            get { return IntTuple2DParameterData.Value; }
            set
            {
                IntTuple2DParameterData.Value = value;
                NotifyOfPropertyChange(() => Value);
            }
        }
    }

    class IntTuple3DParameterViewModel : ParameterBaseViewModel
    {
        public IntTuple3DParameter IntTuple3DParameterData { get; }
        public IntTuple3DParameterViewModel(IntTuple3DParameter intTuple3DParameter, LinkBaseViewModel parent) : base(intTuple3DParameter, parent)
        {
            IntTuple3DParameterData = intTuple3DParameter;
        }

        public IntTuple3D Value
        {
            get { return IntTuple3DParameterData.Value; }
            set
            {
                IntTuple3DParameterData.Value = value;
                NotifyOfPropertyChange(() => Value);
            }
        }
    }

    class IntTuple4DParameterViewModel : ParameterBaseViewModel
    {
        public IntTuple4DParameter IntTuple4DParameterData { get; }
        public IntTuple4DParameterViewModel(IntTuple4DParameter intTuple4DParameter, LinkBaseViewModel parent) : base(intTuple4DParameter, parent)
        {
            IntTuple4DParameterData = intTuple4DParameter;
        }

        public IntTuple4D Value
        {
            get { return IntTuple4DParameterData.Value; }
            set
            {
                IntTuple4DParameterData.Value = value;
                NotifyOfPropertyChange(() => Value);
            }
        }
    }
}
