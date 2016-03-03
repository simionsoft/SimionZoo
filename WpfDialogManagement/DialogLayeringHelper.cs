using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;

namespace Technewlogic.WpfDialogManagement
{
	class DialogLayeringHelper : IDialogHost
	{
		public DialogLayeringHelper(ContentControl parent)
		{
			_parent = parent;
		}

		private readonly ContentControl _parent;
		private readonly List<object> _layerStack = new List<object>();

		public bool HasDialogLayers { get { return _layerStack.Any(); } }

		#region Implementation of IDialogHost

		public void ShowDialog(DialogBaseControl dialog)
		{
			_layerStack.Add(_parent.Content);
			_parent.Content = dialog;
		}

		public void HideDialog(DialogBaseControl dialog)
		{
			if (_parent.Content == dialog)
			{
				var oldContent = _layerStack.Last();
				_layerStack.Remove(oldContent);
				_parent.Content = oldContent;
			}
			else
				_layerStack.Remove(dialog);
		}

		public FrameworkElement GetCurrentContent()
		{
			return _parent;
		}

		#endregion
	}
}