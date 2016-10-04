using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media;

namespace Badger.Views.Help
{

    public class ComboBoxToMaxItemWidthConverter : IValueConverter
    {

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            double maxWidth = 100;
            ObservableCollection<string> items = value as ObservableCollection<string>;
            if(items!=null)
            {
                foreach (string item in items)
                {
                    ComboBoxItem cbItem = new ComboBoxItem();
                    cbItem.Content = item;
                    if (cbItem.ActualWidth > maxWidth)
                        maxWidth = cbItem.ActualWidth;
                }
            }
            
            return maxWidth;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
