//using GongSolutions.Wpf.DragDrop;
//using System;
//using System.Collections;
//using System.Collections.Generic;
//using System.Collections.ObjectModel;
//using System.ComponentModel;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using System.Windows;
//using System.Windows.Controls;
//using System.Windows.Input;
//using System.Xml;
//using System.Xml.Serialization;
//using Badger.Data.NeuralNetwork;
//using Badger.Data.NeuralNetwork.Links;
//using Badger.Data.NeuralNetwork.Parameters;
//using Xml.Serialization;
//using Badger.Data.NeuralNetwork.Tuple;

//namespace Badger.ViewModels.NeuralNetwork
//{
//    public class MainWindowViewModel2 : INotifyPropertyChanged
//    {
//        public MainWindowViewModel2()
//        {
//            foreach (var item in GetSampleInputData())
//                Problem.Inputs.Add(item);

//            Problem.NetworkArchitecture.Chains.Add(new Chain(Problem.NetworkArchitecture, "123"));
//            Problem.NetworkArchitecture.Chains.Add(new Chain(Problem.NetworkArchitecture, "456"));
//            Problem.NetworkArchitecture.Chains[0].ChainLinks.Add(new DenseLayer(Problem.NetworkArchitecture.Chains[0], "dense1"));

//        }

//        public static List<InputData> GetSampleInputData()
//        {
//            var result = new List<InputData>();
//            result.Add(new Data.Parameters.InputData("State-Images", "State Images", new IntTuple3D(84, 84, 4), "The state conisting out of the last 4 images."));
//            result.Add(new Data.Parameters.InputData("State-RAM", "State RAM", new IntTuple1D(1024 * 1024), "The state conisting out of the current RAM."));
//            return result;
//        }

//        public event PropertyChangedEventHandler PropertyChanged;
//        protected virtual void OnPropertyChanged(string propertyName)
//        {
//            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
//        }

//        private Problem _problem = new Problem();
//        public Problem Problem
//        {
//            set
//            {
//                _problem = value;
//                OnPropertyChanged("Problem");
//            }
//            get
//            {
//                return _problem;
//            }
//        }

//        public DragDropHandler DDHandler { get; set; } = new DragDropHandler();

//        public class DragDropHandler : IDropTarget
//        {
//            private DefaultDropHandler _defaultDropHandler = new DefaultDropHandler();
//            public void DragOver(IDropInfo dropInfo)
//            {
//                _defaultDropHandler.DragOver(dropInfo);
//            }

//            public void Drop(IDropInfo dropInfo)
//            {
//                if (dropInfo.Data == dropInfo.TargetItem)
//                    return;

//                var targetChain = ((ItemsControl)dropInfo.VisualTarget).DataContext as Chain;
//                var item = dropInfo.Data as LinkBase;

//                if (targetChain is null)
//                {
//                    ((ObservableCollection<LinkBase>)((ItemsControl)dropInfo.VisualTarget).ItemsSource).Add(item);
//                }
//                else
//                {
//                    item.ParentChain.ChainLinks.Remove(item);
//                    item.ParentChain = targetChain;
//                    item.ParentChain.ChainLinks.Insert(dropInfo.InsertIndex, item);
//                }

//                item.ParentChain.NetworkArchitecture.ValidateInputCompatibility();
//            }
//        }
//    }
//}
