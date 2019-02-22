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

using System;
using System.Linq;
using System.Reflection;
using OxyPlot;

namespace Badger.ViewModels
{
    public class SafePlotModel : PlotModel, IPlotModel
    {
        private static readonly Type BaseType = typeof(SafePlotModel).BaseType;
        private static readonly MethodInfo BaseAttachMethod = BaseType
            .GetMethods(BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.DeclaredOnly)
            .Where(methodInfo => methodInfo.IsFinal && methodInfo.IsPrivate)
            .FirstOrDefault(methodInfo => methodInfo.Name.EndsWith(nameof(IPlotModel.AttachPlotView)));

        void IPlotModel.AttachPlotView(IPlotView plotView)
        {
            //because of issue https://github.com/oxyplot/oxyplot/issues/497 
            //only one view can ever be attached to one plotmodel
            //we have to force detach previous view and then attach new one
            if (plotView != null && PlotView != null && !Equals(plotView, PlotView))
            {
                BaseAttachMethod.Invoke(this, new object[] { null });
                BaseAttachMethod.Invoke(this, new object[] { plotView });
            }
            else
            {
                BaseAttachMethod.Invoke(this, new object[] { plotView });
            }
        }
    }
}