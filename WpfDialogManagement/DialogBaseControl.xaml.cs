using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

namespace Technewlogic.WpfDialogManagement
{
	/// <summary>
	/// Interaction logic for DialogBaseControl.xaml
	/// </summary>
	partial class DialogBaseControl : INotifyPropertyChanged
	{
		public DialogBaseControl(FrameworkElement originalContent, DialogBase dialog)
		{
			Caption = dialog.Caption;

			InitializeComponent();

			var backgroundImage = originalContent.CaptureImage();
			backgroundImage.Stretch = System.Windows.Media.Stretch.Fill;
			BackgroundImageHolder.Content = backgroundImage;

			_dialog = dialog;
			CreateButtons();
		}

		private readonly DialogBase _dialog;

		public string Caption { get; private set; }

		public Visibility CaptionVisibility
		{
			get
			{
				return string.IsNullOrWhiteSpace(Caption)
					? Visibility.Collapsed
					: Visibility.Visible;
			}
		}

		private VerticalAlignment _verticalDialogAlignment = VerticalAlignment.Center;
		public VerticalAlignment VerticalDialogAlignment
		{
			get { return _verticalDialogAlignment; }
			set
			{
				_verticalDialogAlignment = value;
				OnPropertyChanged("VerticalDialogAlignment");
			}
		}

		private HorizontalAlignment _horizontalDialogAlignment = HorizontalAlignment.Center;
		public HorizontalAlignment HorizontalDialogAlignment
		{
			get { return _horizontalDialogAlignment; }
			set
			{
				_horizontalDialogAlignment = value;
				OnPropertyChanged("HorizontalDialogAlignment");
			}
		}

		public void SetCustomContent(object content)
		{
			CustomContent.Content = content;
		}

		private void CreateButtons()
		{
			switch (_dialog.Mode)
			{
				case DialogMode.None:
					break;
				case DialogMode.Ok:
					AddOkButton();
					break;
				case DialogMode.Cancel:
					AddCancelButton();
					break;
				case DialogMode.OkCancel:
					AddOkButton();
					AddCancelButton();
					break;
				case DialogMode.YesNo:
					AddYesButton();
					AddNoButton();
					break;
				case DialogMode.YesNoCancel:
					AddYesButton();
					AddNoButton();
					AddCancelButton();
					break;
				default:
					throw new ArgumentOutOfRangeException();
			}
		}

		public void AddNoButton()
		{
			AddButton(_dialog.NoText, GetCallback(_dialog.No, DialogResultState.No), false, true, "CanNo");
		}

		public void AddYesButton()
		{
			AddButton(_dialog.YesText, GetCallback(_dialog.Yes, DialogResultState.Yes), true, false, "CanYes");
		}

		public void AddCancelButton()
		{
			AddButton(_dialog.CancelText, GetCallback(_dialog.Cancel, DialogResultState.Cancel), false, true, "CanCancel");
		}

		public void AddOkButton()
		{
			AddButton(_dialog.OkText, GetCallback(_dialog.Ok, DialogResultState.Ok), true, true, "CanOk");
		}

		private void AddButton(
			string buttonText,
			Action callback,
			bool isDefault,
			bool isCancel,
			string bindingPath)
		{
			var btn = new Button
			{
				Content = buttonText,
				MinWidth = 80,
				MaxWidth = 150,
				IsDefault = isDefault,
				IsCancel = isCancel,
				Margin = new Thickness(5)
			};

			var enabledBinding = new Binding(bindingPath) { Source = _dialog };
			btn.SetBinding(IsEnabledProperty, enabledBinding);

			btn.Click += (s, e) => callback();

			ButtonsGrid.Columns++;
			ButtonsGrid.Children.Add(btn);
		}

		internal void RemoveButtons()
		{
			ButtonsGrid.Children.Clear();
		}

		private Action GetCallback(
			Action dialogCallback,
			DialogResultState result)
		{
			_dialog.Result = result;
			Action callback = () =>
			{
				if (dialogCallback != null)
					dialogCallback();
				if (_dialog.CloseBehavior == DialogCloseBehavior.AutoCloseOnButtonClick)
					_dialog.Close();
			};

			return callback;
		}

		public event PropertyChangedEventHandler PropertyChanged;
		private void OnPropertyChanged(string propertyName)
		{
			if (PropertyChanged != null)
				PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
		}
	}
}
