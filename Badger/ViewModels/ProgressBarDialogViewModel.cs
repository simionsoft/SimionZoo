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
    public class ProgressBarDialogViewModel : Screen
    {
        private string m_title;
        private string m_message;

        public string Title { get { return m_title; } set { } }
        public string Message { get { return m_message; } set { } }

        public ProgressBarDialogViewModel(string title,string message)
        {
            m_title = title;
            m_message = message;
        }

        private bool m_bCancelled = false;
        public bool Cancelled { get { return m_bCancelled; } }

        public void Cancel()
        {
            m_bCancelled = true;
            TryClose();
        }

        private double m_progress = 0.0;
        public double Progress { get { return m_progress; } set { m_progress = value; NotifyOfPropertyChange(() => Progress); } }

        public void UpdateProgressBar(double value)
        {
            Progress = value;
        }
    }
    
}
