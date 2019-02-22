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


namespace Badger.ViewModels
{
    public class SelectableTreeItem : PropertyChangedBase
    {
        private bool m_bIsSelected = true;

        public bool IsSelected
        {
            get { return m_bIsSelected; }
            set
            {
                m_bIsSelected = value; NotifyOfPropertyChange(() => IsSelected);
                TraverseAction(false, (element) => { element.IsSelected = value; });
            }
        }

        private bool m_bIsCheckVisible;

        public bool IsCheckVisible
        {
            get { return m_bIsCheckVisible; }
            set
            {
                m_bIsCheckVisible = value; NotifyOfPropertyChange(() => IsCheckVisible);
                TraverseAction(false, (element) => { element.IsCheckVisible = value; });
            }
        }

        private bool m_bIsVisible = true;
        public bool IsVisible
        {
            get { return m_bIsVisible; }
            set { m_bIsVisible = value; NotifyOfPropertyChange(() => IsVisible); }
        }

        public void LocalTraverseAction(System.Action<SelectableTreeItem> action) { action(this); }

        public virtual void TraverseAction(bool doActionLocally, System.Action<SelectableTreeItem> action) { }
    }
}
