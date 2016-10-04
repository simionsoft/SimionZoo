using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Dynamic;
using System.Windows;
using Caliburn.Micro;
using Badger.ViewModels;
using System.Windows.Forms;

namespace Badger.Data
{
    class CaliburnUtility
    {
        public static DialogViewModel.Result showWarningDialog(string message, string windowHeader)
        {
            DialogViewModel dvm = new DialogViewModel(null, message, DialogViewModel.DialogType.Info);
            dynamic settings = new ExpandoObject();
            settings.WindowStyle = WindowStyle.ThreeDBorderWindow;
            settings.ShowInTaskbar = true;
            settings.Title = windowHeader;

            new WindowManager().ShowDialog(dvm, null, settings);

            return dvm.DialogResult;
        }

        public static void showVMDialog(PropertyChangedBase viewmodel, string windowHeader)
        {
            WindowManager windowManager = new WindowManager();
            dynamic settings = new ExpandoObject();
            settings.WindowStyle = WindowStyle.ThreeDBorderWindow;
            settings.ShowInTaskbar = true;
            settings.Title = windowHeader;
            settings.WindowState = WindowState.Normal;
            settings.ResizeMode = ResizeMode.CanMinimize;

            windowManager.ShowDialog(viewmodel, null, settings);
        }

        public static string selectFolder(string initialDirectory)
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();
            fbd.SelectedPath = initialDirectory;

            if (fbd.ShowDialog() == DialogResult.OK)
            {
                return fbd.SelectedPath;
            }
            return "";
        }
    }
}
