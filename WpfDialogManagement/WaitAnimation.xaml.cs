namespace Technewlogic.WpfDialogManagement
{
	partial class WaitAnimation
	{
		public WaitAnimation()
		{
			InitializeComponent();

			/*

			NameScope.SetNameScope(this, new NameScope());

			double coordinateSystemOffset = 50d;
			TimeSpan animationDuration = TimeSpan.FromMilliseconds(1500);
			double numberOfLines = 15d;
			double angleStepSize = 360d / numberOfLines;
			double timeStepSize = animationDuration.TotalMilliseconds / numberOfLines;

			for (int i = 0; i < numberOfLines; i++)
			{
				double currentAngle = angleStepSize * i;

				// the brush
				SolidColorBrush lineBrush = new SolidColorBrush();
				lineBrush.Color = Colors.Transparent;
				string lineBrushName = "lineBrush_" + i.ToString();
				this.RegisterName(lineBrushName, lineBrush);

				// the animation
				ColorAnimation colorAnimation = new ColorAnimation();
				colorAnimation.From = Colors.Gray;
				colorAnimation.To = Colors.WhiteSmoke;
				colorAnimation.Duration = new Duration(animationDuration);
				colorAnimation.RepeatBehavior = RepeatBehavior.Forever;
				colorAnimation.BeginTime = TimeSpan.FromMilliseconds(timeStepSize * i);

				// Configure the animation to target the brush's Color property.
				Storyboard.SetTargetName(colorAnimation, lineBrushName);
				Storyboard.SetTargetProperty(
					colorAnimation,
					new PropertyPath(SolidColorBrush.ColorProperty));

				// Create a storyboard to contain the animation.
				Storyboard colorAnimatedStoryboard = new Storyboard();
				colorAnimatedStoryboard.Children.Add(colorAnimation);

				// the line
				Line l = new Line();
				l.Stroke = lineBrush;
				l.StrokeThickness = LineThickness;

				var innerPoint = GetPointOnCircle(currentAngle, 20, coordinateSystemOffset);
				var outerPoint = GetPointOnCircle(currentAngle, 50, coordinateSystemOffset);

				l.X1 = innerPoint.X;
				l.Y1 = innerPoint.Y;
				l.X2 = outerPoint.X;
				l.Y2 = outerPoint.Y;

				canvas.Children.Add(l);

				l.Loaded += new RoutedEventHandler((s, e) =>
					colorAnimatedStoryboard.Begin(l));
			}
			 
			*/
		}

		/*
		
		#region LineThickness

		public double LineThickness
		{
			get { return (double)GetValue(LineThicknessProperty); }
			set { SetValue(LineThicknessProperty, value); }
		}

		// Using a DependencyProperty as the backing store for LineThickness.  This enables animation, styling, binding, etc...
		public static readonly DependencyProperty LineThicknessProperty =
			DependencyProperty.Register("LineThickness", typeof(double), typeof(WaitAnimation), new UIPropertyMetadata(3d));

		#endregion

		private Point GetPointOnCircle(
			double degrees,
			double radius,
			double coordinateSystemOffset)
		{
			double rad = GetRad(degrees);
			double x = radius * Math.Cos(rad) + coordinateSystemOffset;
			double y = radius * Math.Sin(rad) + coordinateSystemOffset;

			return new Point(x, y);
		}

		private double GetRad(double degrees)
		{
			return 2 * Math.PI * degrees / 360d;
		}
		 
		*/
	}
}
