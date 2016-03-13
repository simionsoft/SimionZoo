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
    /// Lógica de interacción para RightTreeView.xaml
    /// </summary>
    public partial class RightTreeView : UserControl
    {
        public RightTreeView()
        {
            InitializeComponent();
        }
        void OnLoaded(object sender, RoutedEventArgs e)
        {
            // Prevent the TreeView from responding to the keyboard.
            // Since there's no sane way to set a TreeView's SelectedItem,
            // we can't customize the keyboard navigation logic. :(
            this.Tree.PreviewKeyDown += delegate(object obj, KeyEventArgs args) { args.Handled = true; };

            // Put some data into the TreeView.
            //this.PopulateTreeView();
        }
    }
}
