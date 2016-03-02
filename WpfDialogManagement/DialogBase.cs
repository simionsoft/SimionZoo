using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Threading;
using Technewlogic.WpfDialogManagement.Contracts;

namespace Technewlogic.WpfDialogManagement
{
	abstract class DialogBase : IDialog, INotifyPropertyChanged
	{
		protected DialogBase(
			IDialogHost dialogHost,
			DialogMode dialogMode,
			Dispatcher dispatcher)
		{
			_dialogHost = dialogHost;
			_dispatcher = dispatcher;
			Mode = dialogMode;
			CloseBehavior = DialogCloseBehavior.AutoCloseOnButtonClick;

			OkText = "Ok";
			CancelText = "Cancel";
			YesText = "Yes";
			NoText = "No";

			switch (dialogMode)
			{
				case DialogMode.None:
					break;
				case DialogMode.Ok:
					CanOk = true;
					break;
				case DialogMode.Cancel:
					CanCancel = true;
					break;
				case DialogMode.OkCancel:
					CanOk = true;
					CanCancel = true;
					break;
				case DialogMode.YesNo:
					CanYes = true;
					CanNo = true;
					break;
				case DialogMode.YesNoCancel:
					CanYes = true;
					CanNo = true;
					CanCancel = true;
					break;
				default:
					throw new ArgumentOutOfRangeException("dialogMode");
			}

		}

		private readonly IDialogHost _dialogHost;
		private readonly Dispatcher _dispatcher;
		private object _content;

		protected DialogBaseControl DialogBaseControl { get; private set; }

		protected void SetContent(object content)
		{
			_content = content;
		}

		protected void InvokeUICall(Action del)
		{
			_dispatcher.Invoke(del, DispatcherPriority.DataBind);
		}

		#region Implementation of IDialog

		public DialogMode Mode { get; private set; }
		public DialogResultState Result { get; set; }
		public DialogCloseBehavior CloseBehavior { get; set; }

		public Action Ok { get; set; }
		public Action Cancel { get; set; }
		public Action Yes { get; set; }
		public Action No { get; set; }

		private bool _canOk;
		public bool CanOk
		{
			get { return _canOk; }
			set
			{
				_canOk = value;
				OnPropertyChanged("CanOk");
			}
		}

		private bool _canCancel;
		public bool CanCancel
		{
			get { return _canCancel; }
			set
			{
				_canCancel = value;
				OnPropertyChanged("CanCancel");
			}
		}

		private bool _canYes;
		public bool CanYes
		{
			get { return _canYes; }
			set
			{
				_canYes = value;
				OnPropertyChanged("CanYes");
			}
		}

		private bool _canNo;
		public bool CanNo
		{
			get { return _canNo; }
			set
			{
				_canNo = value;
				OnPropertyChanged("CanNo");
			}
		}

		public string OkText { get; set; }
		public string CancelText { get; set; }
		public string YesText { get; set; }
		public string NoText { get; set; }

		public string Caption { get; set; }

		private VerticalAlignment? _verticalDialogAlignment;
		public VerticalAlignment VerticalDialogAlignment
		{
			set
			{
				if (DialogBaseControl == null)
					_verticalDialogAlignment = value;
				else
					DialogBaseControl.VerticalDialogAlignment = value;
			}
		}

		private HorizontalAlignment? _horizontalDialogAlignment;
		public HorizontalAlignment HorizontalDialogAlignment
		{
			set
			{
				if (DialogBaseControl == null)
					_horizontalDialogAlignment = value;
				else
					DialogBaseControl.HorizontalDialogAlignment = value;
			}
		}

		public void Show()
		{
			if (DialogBaseControl != null)
				throw new Exception("The dialog can only be shown once.");

			InvokeUICall(() =>
				{
					DialogBaseControl = new DialogBaseControl(_dialogHost.GetCurrentContent(), this);
					DialogBaseControl.SetCustomContent(_content);
					if (_verticalDialogAlignment.HasValue)
						DialogBaseControl.VerticalDialogAlignment = _verticalDialogAlignment.Value;
					if (_horizontalDialogAlignment.HasValue)
						DialogBaseControl.HorizontalDialogAlignment = _horizontalDialogAlignment.Value;
					_dialogHost.ShowDialog(DialogBaseControl);
				});
		}

		public void Close()
		{
			// Dialog wird angezeigt?
			if (DialogBaseControl == null)
				return;

			// Callbacks abhängen
			Ok = null;
			Cancel = null;
			Yes = null;
			No = null;

			InvokeUICall(
				() =>
				{
					_dialogHost.HideDialog(DialogBaseControl);
					DialogBaseControl.SetCustomContent(null);
				});
		}

		#endregion

		public event PropertyChangedEventHandler PropertyChanged;
		private void OnPropertyChanged(string propertyName)
		{
			if (PropertyChanged != null)
				PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
		}
	}
}