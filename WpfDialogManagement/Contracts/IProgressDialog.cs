namespace Technewlogic.WpfDialogManagement.Contracts
{
	public interface IProgressDialog : IWaitDialog
	{
		int Progress { get; set; }
	}
}