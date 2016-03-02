using System;
using System.Windows;

namespace Technewlogic.WpfDialogManagement.Contracts
{
	/// <summary>
	/// Not used so far...
	/// </summary>
	public interface IDialogConfig
	{
		IDialogConfig Mode(DialogMode mode);
		IDialogConfig CloseBehavior(DialogCloseBehavior closeBehavior);

		IDialogConfig Ok(Action del);
		IDialogConfig Cancel(Action del);
		IDialogConfig Yes(Action del);
		IDialogConfig No(Action del);

		IDialogConfig OkText(string value);
		IDialogConfig CancelText(string value);
		IDialogConfig YesText(string value);
		IDialogConfig NoText(string value);

		IDialogConfig Caption(string value);

		IDialogConfig VerticalDialogAlignment(VerticalAlignment verticalAlignment);
		IDialogConfig HorizontalDialogAlignment(HorizontalAlignment horizontalAlignment);

		IDialog CreateDialog();
	}
}