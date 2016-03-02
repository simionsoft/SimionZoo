using System.Windows.Threading;
using Technewlogic.WpfDialogManagement.Contracts;

namespace Technewlogic.WpfDialogManagement
{
	class CustomContentDialog : DialogBase, ICustomContentDialog
	{
		public CustomContentDialog(
			IDialogHost dialogHost, 
			DialogMode dialogMode,
			object content,
			Dispatcher dispatcher)
			: base(dialogHost, dialogMode, dispatcher)
		{
			SetContent(content);
		}
	}
}