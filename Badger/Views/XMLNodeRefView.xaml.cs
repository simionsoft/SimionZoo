using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace AppXML.Views
{
    /// <summary>
    /// Lógica de interacción para XMLNodeRefView.xaml
    /// </summary>
    public partial class XMLNodeRefView : UserControl
    {
        public XMLNodeRefView()
        {
            InitializeComponent();
        }
    }
    public class ComboBoxToMaxItemWidthConverter : IValueConverter
    {

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            double maxWidth = 0;
            ComboBox cb = (ComboBox)value;
            foreach (var item in cb.Items)
            {
                ComboBoxItem cbItem = (ComboBoxItem)cb.ItemContainerGenerator.ContainerFromItem(item);
                if (cbItem.ActualWidth > maxWidth)
                    maxWidth = cbItem.ActualWidth;
            }
            return maxWidth;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
