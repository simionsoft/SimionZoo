/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

using Caliburn.Micro;
using GongSolutions.Wpf.DragDrop;
using Badger.Data.NeuralNetwork.Parameters;
using Badger.ViewModels.NeuralNetwork.Links;
using Badger.ViewModels.NeuralNetwork.Parameters;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Badger.Data.NeuralNetwork.Problem;
using Badger.Data.NeuralNetwork;

namespace Badger.ViewModels.NeuralNetwork
{
    class OutputConfigurationViewModel : PropertyChangedBase, IDropTarget
    {
        public OutputConfiguration OutputConfigurationData { get; }
        public OutputConfigurationViewModel(OutputConfiguration outputConfiguration)
        {
            OutputConfigurationData = outputConfiguration;

            if (OutputConfigurationData.LinkConnection != null && OutputConfigurationData.LinkConnection.Target != null)
                OutputLinkConnection = new LinkConnectionViewModel(OutputConfigurationData.LinkConnection);
        }

        private LinkConnectionViewModel _outputLinkConnection;
        public LinkConnectionViewModel OutputLinkConnection
        {
            get
            {
                return _outputLinkConnection;
            }
            set
            {
                _outputLinkConnection = value;
                NotifyOfPropertyChange(() => OutputLinkConnection);
            }
        }

        public void DragOver(IDropInfo dropInfo)
        {
            if (dropInfo.Data is LinkBaseViewModel)
            {
                dropInfo.Effects = System.Windows.DragDropEffects.Copy;
                dropInfo.NotHandled = false;
                dropInfo.DropTargetAdorner = DropTargetAdorners.Insert;
            }
        }

        public void Drop(IDropInfo dropInfo)
        {
            var item = dropInfo.Data as LinkBaseViewModel;

            if (item == null)
                return;

            OutputConfigurationData.LinkConnection = new LinkConnection(item.LinkBaseData);
            OutputLinkConnection = new LinkConnectionViewModel(OutputConfigurationData.LinkConnection); 
        }
    }
}
