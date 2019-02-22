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

using System.Xml;
using System.Collections.Generic;

using Caliburn.Micro;

using Herd.Files;

namespace Badger.ViewModels
{

    public class LoggedForkValueViewModel: SelectableTreeItem
    {
        ForkValue m_model;

        public string Value { get { return m_model.Value; } set { m_model.Value = value; } }

        private BindableCollection<LoggedForkViewModel> m_forks = new BindableCollection<LoggedForkViewModel>();
        public BindableCollection<LoggedForkViewModel> Forks
        {
            get { return m_forks; }
            set { m_forks = value; NotifyOfPropertyChange(() => Forks); }
        }

        //this is used to hide the space given to display children forks in case there is none
        private bool m_bHasForks = false;
        public bool HasChildrenForks
        {
            get { return m_bHasForks; }
            set { m_bHasForks = value; NotifyOfPropertyChange(() => HasChildrenForks); }
        }

        public LoggedForkViewModel Parent { get; set; }
        public List<LoggedExperimentalUnitViewModel> ExpUnits { get; }
            = new List<LoggedExperimentalUnitViewModel>();

        public LoggedForkValueViewModel(ForkValue model, LoggedForkViewModel parent)
        {
            m_model = model;
            Parent = parent;

            //hide the area used to display children forks?
            HasChildrenForks = Forks.Count != 0;
        }

        public override void TraverseAction(bool doActionLocally,System.Action<SelectableTreeItem> action)
        {
            if (doActionLocally) LocalTraverseAction(action);
            foreach (LoggedForkViewModel fork in Forks)
                fork.TraverseAction(doActionLocally, action);
            foreach (LoggedExperimentalUnitViewModel expUnit in ExpUnits)
                expUnit.LocalTraverseAction(action);
        }
    }
}
