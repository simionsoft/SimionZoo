using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace Technewlogic.WpfDialogManagement
{
	static class ImagingExtensions
	{
		public static void FromBitmapResource(this Image image, Type callingType, string relativePath)
		{
			var assemblyName = callingType.Assembly.FullName;
			var bi = new BitmapImage(
				new Uri(
					string.Format(
						"pack://application:,,,/{0};component/{1}",
						assemblyName,
						relativePath),
					UriKind.RelativeOrAbsolute));
			image.Source = bi;
		}

		public static Image CaptureImage(this FrameworkElement me, bool ensureSize = false)
		{
			//var width = ContentPanel.ActualWidth == 0 ? 1 : (int)ContentPanel.ActualWidth;
			//var height = ContentPanel.ActualHeight == 0 ? 1 : (int)ContentPanel.ActualHeight;
			var width = Convert.ToInt32(me.ActualWidth);
			width = width == 0 ? 1 : width;
			var height = Convert.ToInt32(me.ActualHeight);
			height = height == 0 ? 1 : height;

			// TODO: Multiplikation der DPI mit der aktuellen PresentationSource
			var bmp = new RenderTargetBitmap(
				width, height,
				96, 96,
				PixelFormats.Default);
			bmp.Render(me);

			var img = new Image
			{
				Source = bmp,
				Stretch = Stretch.None,
				Width = width - (ensureSize ? 1 : 0),
				Height = height - (ensureSize ? 1 : 0)
			};
			//img.Measure(new Size(width, height));
			//var imageSize = img.DesiredSize;
			//img.Arrange(
			//    new Rect(new Point(0, 0), imageSize));

			return img;
		}
	}
}
