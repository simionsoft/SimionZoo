using System;
using System.Threading;
using System.Windows.Threading;
using Technewlogic.WpfDialogManagement.Contracts;

namespace Technewlogic.WpfDialogManagement
{
	class WaitProgressDialog : DialogBase, IProgressDialog
	{
		#region Factory

		public static IWaitDialog CreateWaitDialog(
			IDialogHost dialogHost,
			DialogMode dialogMode,
			Dispatcher dispatcher)
		{
			IWaitDialog dialog = null;
			dispatcher.Invoke(
				new Action(() => dialog = new WaitProgressDialog(
					dialogHost, dialogMode, true, dispatcher)),
				DispatcherPriority.DataBind);
			return dialog;
		}

		public static IProgressDialog CreateProgressDialog(
			IDialogHost dialogHost,
			DialogMode dialogMode,
			Dispatcher dispatcher)
		{
			IProgressDialog dialog = null;
			dispatcher.Invoke(
				new Action(() => dialog = new WaitProgressDialog(
					dialogHost, dialogMode, false, dispatcher)),
				DispatcherPriority.DataBind);
			return dialog;
		}

		#endregion

		private WaitProgressDialog(
			IDialogHost dialogHost,
			DialogMode dialogMode,
			bool showWaitAnimation,
			Dispatcher dispatcher)
			: base(dialogHost, dialogMode, dispatcher)
		{
			_waitProgressDialogControl = new WaitProgressDialogControl(showWaitAnimation);
			SetContent(_waitProgressDialogControl);
		}

		private readonly WaitProgressDialogControl _waitProgressDialogControl;
		private bool _isReady;

		#region Implementation of IMessageDialog

		public string Message
		{
			get
			{
				var text = string.Empty;
				InvokeUICall(
					() => text = _waitProgressDialogControl.DisplayText);
				return text;
			}
			set
			{
				InvokeUICall(
					() => _waitProgressDialogControl.DisplayText = value);
			}
		}

		#endregion

		#region Implementation of IWaitDialog

		public Action WorkerReady { get; set; }
		public bool CloseWhenWorkerFinished { get; set; }

		private string _readyMessage;
		public string ReadyMessage
		{
			get { return _readyMessage; }
			set
			{
				_readyMessage = value;
				if (_isReady)
					InvokeUICall(
						() => _waitProgressDialogControl.DisplayText = value);
			}
		}

		private readonly ManualResetEvent _beginWork =
			new ManualResetEvent(false);

		public void Show(Action workerMethod)
		{
			ThreadPool.QueueUserWorkItem(o =>
			{
				try
				{
					_beginWork.WaitOne(-1);

					workerMethod();

					InvokeUICall(() =>
					{
						_isReady = true;

						if (WorkerReady != null)
							WorkerReady();

						if (CloseWhenWorkerFinished)
						{
							Close();
							return;
						}

						_waitProgressDialogControl.DisplayText = ReadyMessage;
						_waitProgressDialogControl.Finish();

						DialogBaseControl.RemoveButtons();
						DialogBaseControl.AddOkButton();
					});
				}
				catch (Exception ex)
				{
					InvokeUICall(() =>
					{
						Close();
						throw ex;
					});
				}
			});

			Show();

			_beginWork.Set();
		}

		public new void InvokeUICall(Action uiWorker)
		{
			base.InvokeUICall(uiWorker);
		}

		#endregion

		#region Implementation of IProgressDialog

		public int Progress
		{
			get
			{
				var progress = 0;
				InvokeUICall(
					() => progress = _waitProgressDialogControl.Progress);
				return progress;
			}
			set
			{
				InvokeUICall(
					() => _waitProgressDialogControl.Progress = value);
			}
		}

		#endregion
	}
}