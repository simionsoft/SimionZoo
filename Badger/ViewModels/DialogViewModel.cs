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
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class DialogViewModel : Screen
    {
        private string _title;
        private string _message;

        public string Title { get { return _title; } set { } }
        public string Message { get { return _message; } set { } }

        private string _cancelButtonVisibility;

        public string CancelButtonVisibility { get { return _cancelButtonVisibility; } set { } }

        private string _answerVisibility;
        public string AnswerVisibility { get { return _answerVisibility; } set { } }
        public enum Result
        {
            OK,
            CANCEL,
            NotSet,

        };
        public enum DialogType
        {
            Info,
            YesNo,
            Answer,
        };

        private Result _result;

        public DialogViewModel(string title,string message,DialogType type)
        {
            _title = title;
            _message = message;
            _result = Result.NotSet;
            if (type == DialogType.YesNo)
                _cancelButtonVisibility = "Visible";
            else
                _cancelButtonVisibility = "Hidden";
            if (type == DialogType.Answer)
                _answerVisibility = "Visible";
            else
                _answerVisibility = "Hidden";
        }

        public void OK()
        {
            _result = Result.OK;
            TryClose();
        }
        public void Cancel()
        {
            _result = Result.CANCEL;
            TryClose();
        }

        public Result DialogResult { get { return _result; } }
        public string Text { get; set; }
    }
    
}
