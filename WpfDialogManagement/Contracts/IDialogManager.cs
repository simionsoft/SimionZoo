namespace Technewlogic.WpfDialogManagement.Contracts
{
	public interface IDialogManager
	{
		IMessageDialog CreateMessageDialog(string message, DialogMode dialogMode);
		IMessageDialog CreateMessageDialog(string message, string caption, DialogMode dialogMode);

		ICustomContentDialog CreateCustomContentDialog(object content, DialogMode dialogMode);
		ICustomContentDialog CreateCustomContentDialog(object content, string caption, DialogMode dialogMode);

		IProgressDialog CreateProgressDialog(DialogMode dialogMode);
		IProgressDialog CreateProgressDialog(string message, DialogMode dialogMode);
		IProgressDialog CreateProgressDialog(string message, string readyMessage, DialogMode dialogMode);

		IWaitDialog CreateWaitDialog(DialogMode dialogMode);
		IWaitDialog CreateWaitDialog(string message, DialogMode dialogMode);
		IWaitDialog CreateWaitDialog(string message, string readyMessage, DialogMode dialogMode);
	}
}