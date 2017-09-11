using Caliburn.Micro;
using Badger.Data.NeuralNetwork;
using Badger.Data.NeuralNetwork.Parameters;
using Badger.Data.NeuralNetwork.Tuple;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.ViewModels.NeuralNetwork
{
    class InputDataViewModel : PropertyChangedBase
    {
        public InputData InputData { get; }
        public InputDataViewModel(InputData inputData)
        {
            InputData = inputData;
        }

        public string ID
        {
            get { return InputData.ID; }
            set
            {
                InputData.ID = value;
                NotifyOfPropertyChange(() => ID);
            }
        }

        public string Name
        {
            get { return InputData.Name; }
            set
            {
                InputData.Name = value;
                NotifyOfPropertyChange(() => Name);
            }
        }

        public string Description
        {
            get { return InputData.Description; }
            set
            {
                InputData.Description = value;
                NotifyOfPropertyChange(() => Description);
            }
        }
        
        public IntTupleBase Shape
        {
            get { return InputData.Shape; }
        }

        public override bool Equals(object obj)
        {
            if (obj == null || !(obj is InputDataViewModel))
                return false;

            return (obj == this || (obj as InputDataViewModel).ID.Equals(ID));
        }

        public override int GetHashCode()
        {
            return 1213502048 + EqualityComparer<string>.Default.GetHashCode(ID);
        }
    }
}
