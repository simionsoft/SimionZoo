using System;
using System.Windows;

namespace Technewlogic.WpfDialogManagement.Contracts
{
	public interface IDialog
	{
		DialogMode Mode { get; }
		DialogResultState Result { get; }
		DialogCloseBehavior CloseBehavior { get; set; }

		Action Ok { get; set; }
		Action Cancel { get; set; }
		Action Yes { get; set; }
		Action No { get; set; }

		bool CanOk { get; set; }
		bool CanCancel { get; set; }
		bool CanYes { get; set; }
		bool CanNo { get; set; }

		string OkText { get; set; }
		string CancelText { get; set; }
		string YesText { get; set; }
		string NoText { get; set; }

		string Caption { get; set; }

		VerticalAlignment VerticalDialogAlignment { set; }
		HorizontalAlignment HorizontalDialogAlignment { set; }

		void Show();
		void Close();
	}
}