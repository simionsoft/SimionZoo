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

using System.IO;
using System.Dynamic;
using System.Windows;
using Caliburn.Micro;
using Badger.ViewModels;
using System.Windows.Forms;

namespace Badger.Data
{
    class CaliburnUtility
    {
        /// <summary>
        ///     Setup common settings for pop-up windows whether its a warning dialog, a simple
        ///     dialog or a new independent emergent window.
        /// </summary>
        /// <param name="windowHeader">Title of the window.</param>
        /// <returns>The common settings</returns>
        private static dynamic SetupPopupWindow(string windowHeader)
        {
            dynamic settings = new ExpandoObject();
            settings.WindowStyle = WindowStyle.ThreeDBorderWindow;
            settings.ShowInTaskbar = true;
            settings.Title = windowHeader;
            return settings;
        }

        /// <summary>
        ///     Show a warning dialog, normally when something didn't workout and the user shall
        ///     be notified about the issue.
        /// </summary>
        /// <param name="message">Message to show in the body of the dialog window.</param>
        /// <param name="windowHeader">Title of the dialog window.</param>
        /// <returns>A warning dialog.</returns>
        public static DialogViewModel.Result ShowWarningDialog(string message, string windowHeader)
        {
            DialogViewModel dvm = new DialogViewModel(null, message, DialogViewModel.DialogType.Info);
            var settings = SetupPopupWindow(windowHeader);
            settings.ResizeMode = ResizeMode.NoResize;
            new WindowManager().ShowDialog(dvm, null, settings);

            return dvm.DialogResult;
        }

        /// <summary>
        ///     Show a pop-up window, can be a dialog, which once showed up does not allow interaction 
        ///     with the background window. It also can be an independent window, which does allow
        ///     interaction with any other window of the application.
        /// </summary>
        /// <param name="viewModel">ViewModel to be shown in the pop-up window</param>
        /// <param name="windowHeader">Title of the window.</param>
        /// <param name="isDialog">Whether its a dialog or a window.</param>
        public static void ShowPopupWindow(PropertyChangedBase viewModel, string windowHeader, bool isDialog = true)
        {
            WindowManager windowManager = new WindowManager();
            var settings = SetupPopupWindow(windowHeader);

            settings.WindowState = WindowState.Normal;
            settings.ResizeMode = ResizeMode.CanResize;
            settings.SizeToContent = SizeToContent.Manual;

            if (isDialog)
                windowManager.ShowDialog(viewModel, null, settings);
            else {
                windowManager.ShowWindow(viewModel, null, settings);
            }
        }

        /// <summary>
        ///     Show an emergent dialog to allow users to select directory paths.
        /// </summary>
        /// <param name="initialDirectory">Where everything starts.</param>
        /// <returns>A selected path or a empty string if something goes wrong.</returns>
        public static string SelectFolder(string initialDirectory)
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();
            fbd.SelectedPath = Path.Combine(Directory.GetCurrentDirectory(), initialDirectory);

            if (fbd.ShowDialog() == DialogResult.OK)
                return fbd.SelectedPath;

            return "";
        }
    }
}
