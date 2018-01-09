using Badger.Data.NeuralNetwork.Tuple;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace Badger.Converters
{
    public class IntTupleConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value.ToString();
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var sValue = value as string;
            var sParameter = parameter as string;
            if (sParameter is null)
            {
                sParameter = "()";
            }

            if (targetType == typeof(IntTupleBase))
            {
                switch (sParameter.Count(c => c == ','))
                {
                    default:
                    case 0:
                        targetType = typeof(IntTuple1D);
                        break;

                    case 1:
                        targetType = typeof(IntTuple2D);
                        break;

                    case 2:
                        targetType = typeof(IntTuple3D);
                        break;

                    case 3:
                        targetType = typeof(IntTuple4D);
                        break;
                }
            }

            var result = (IntTupleBase)Activator.CreateInstance(targetType) as IntTupleBase;
            try
            {
                result.FromString(sValue, sParameter);
            }
            catch (Exception ex) { }

            return result;
        }
    }
}
