using Caliburn.Micro;
using Badger.Data.NeuralNetwork.Parameters;
using Badger.ViewModels.NeuralNetwork.Links;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.ViewModels.NeuralNetwork.Parameters
{
    class ParameterBaseViewModel : PropertyChangedBase
    {
        public LinkBaseViewModel Parent { get; } 
        public ParameterBaseViewModel(ParameterBase parameterBase, LinkBaseViewModel parent)
        {
            Parent = parent;
            ParameterBaseData = parameterBase;
        }

        public ParameterBase ParameterBaseData { get; }

        public static ParameterBaseViewModel CreateInstance(ParameterBase parameterBase, LinkBaseViewModel parent)
        {
            if (parameterBase is IntParameter)
                return new IntParameterViewModel(parameterBase as IntParameter, parent);
            if (parameterBase is ActivationFunctionParameter)
                return new ActivationFunctionParameterViewModel(parameterBase as ActivationFunctionParameter, parent);
            if (parameterBase is DoubleParameter)
                return new DoubleParameterViewModel(parameterBase as DoubleParameter, parent);
            if (parameterBase is InputDataParameter)
                return new InputDataParameterViewModel(parameterBase as InputDataParameter, parent);
            if (parameterBase is LinkConnectionListParameter)
                return new LinkConnectionListParameterViewModel(parameterBase as LinkConnectionListParameter, parent);
            if (parameterBase is IntTuple1DParameter)
                return new IntTuple1DParameterViewModel(parameterBase as IntTuple1DParameter, parent);
            if (parameterBase is IntTuple2DParameter)
                return new IntTuple2DParameterViewModel(parameterBase as IntTuple2DParameter, parent);
            if (parameterBase is IntTuple3DParameter)
                return new IntTuple3DParameterViewModel(parameterBase as IntTuple3DParameter, parent);
            if (parameterBase is IntTuple4DParameter)
                return new IntTuple4DParameterViewModel(parameterBase as IntTuple4DParameter, parent);

            return new ParameterBaseViewModel(parameterBase, parent);
        }

        public string Name
        {
            get { return ParameterBaseData.Name; }
        }
    }
}
