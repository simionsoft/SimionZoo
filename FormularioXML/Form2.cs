using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FormularioXML
{
    public partial class Form2 : Form
    {
        List<Element> elements;
        Element test;
        Dictionary<string, ComplexType> myDic;
        List<Label> labels = new List<Label>();
        List<TextBox> inputs = new List<TextBox>();
        List<ComboBox> combos = new List<ComboBox>();
        Dictionary<Button, List<Button>> addDelete = new Dictionary<Button, List<Button>>();
        Dictionary<Button, Button> buttons = new Dictionary<Button, Button>();
        Dictionary<Control, Control> choice = new Dictionary<Control, Control>();
       

        public Form2(List<Element> elements, Dictionary<string,ComplexType> myDic)
        {
            this.elements = elements;
            this.myDic = myDic;
            test = elements.ElementAt(0);
            initializeComponent();
            
        }

        private void label2_Click(object sender, EventArgs e)
        {

        }
        private void tableLayoutPanel1_Paint(object sender, PaintEventArgs e)
        {

        }
        private void Form2_Load(object sender, EventArgs e)
        {

        }       
        private void resolveComplexType(ComplexType complex, TableLayoutPanel panel, Dictionary<string,ComplexType> myDic, ref int x_index, ref int y_index, int tab)
        {
            //to do: añadir label con el nombre del campo complejo para saber que se esta rellenando
            List<string> nameOfComplex = new List<string>();
            Program.resolveComplex(ref complex,myDic, ref nameOfComplex);

            //añadir etiquita para saber que se esta rellenando
            Label complexHeadLine= new Label();
            complexHeadLine.AutoSize = true;
            complexHeadLine.Name = complex.name;
            complexHeadLine.Size = new System.Drawing.Size(35, 13);
            complexHeadLine.Text = complex.name;
            panel.Controls.Add(complexHeadLine);

            Label complexHeadLine2 = new Label();
            complexHeadLine2.AutoSize = true;
            complexHeadLine2.Name = complex.elementName;
            complexHeadLine2.Size = new System.Drawing.Size(35, 13);
            complexHeadLine2.Text = complex.elementName;
            panel.Controls.Add(complexHeadLine2);

            if(complex.elements!=null)
            {
                foreach (Element e in complex.elements)
                {
                    //to do: añadir el elemento al panel. Hay que comprobar si es simpleType antes de añadir
                    //si es simple type hay que añadir combox para seleccionar s
                    //si tiene type un textbox para introducir el tipo
                    Label tmp_l = new Label();
                    tmp_l.AutoSize = true;
                    tmp_l.Name = e.name;
                    tmp_l.Size = new System.Drawing.Size(35, 13);
                    tmp_l.Text = e.name;
                    tmp_l.Anchor = AnchorStyles.Left;
                    panel.Controls.Add(tmp_l);
                    if((e.type==null || e.type.Equals(""))&& e.simpleType!=null)
                    {

                        ComboBox tmp_cb = new ComboBox();
                        foreach (RestrictionEnum res in e.simpleType.restricction.validOptions)
                        {
                            tmp_cb.Items.Add(res.ToString());
                        }
                        tmp_cb.Size = new Size(100, 13);
                        tmp_cb.Anchor = AnchorStyles.Right;
                        panel.Controls.Add(tmp_cb);

                    }
                    else if (e.type.Equals("boolean"))
                    {
                        ComboBox tmp_cb = new ComboBox();
                        tmp_cb.Items.Add("TRUE");
                        tmp_cb.Items.Add("FALSE");
                        tmp_cb.Size = new Size(100, 13);
                        tmp_cb.Anchor = AnchorStyles.Right;
                        panel.Controls.Add(tmp_cb);


                    }
                    else
                    {
                        MaskedTextBox tmp_t = new MaskedTextBox();
                        tmp_t.Size = new Size(100, 13);
                        tmp_t.Name = e.name;
                        tmp_t.Anchor = AnchorStyles.Right;
                        panel.Controls.Add(tmp_t);
                    }
                    x_index++;
                    

                }
                
            }
            if(complex.complexElements!=null)
            {
                int i = 0;
                foreach(ComplexType e in complex.complexElements)
                {
                    if(nameOfComplex.Count>0)
                        e.elementName = nameOfComplex.ElementAt(i);
                    resolveComplexType(e, panel, myDic, ref x_index, ref y_index, tab+5);
                    i++;
                }
            }
            if(complex.choice!=null)
            {
                //hay que añadir un choice
                Label tmp_l = new Label();
                tmp_l.AutoSize = true;
                tmp_l.Name = "CHOICE";
                tmp_l.Size = new System.Drawing.Size(35, 13);
                tmp_l.Text = "CHOICE";
                tmp_l.Anchor = AnchorStyles.Left;
                panel.Controls.Add(tmp_l);

                ComboBox tmp_cb = new ComboBox();
                tmp_cb.Size = new System.Drawing.Size(100, 13);
                tmp_cb.Anchor = AnchorStyles.Right;
                foreach (Element e in complex.choice)
                {
                    tmp_cb.Items.Add(e.name);
                }
                tmp_cb.SelectedIndexChanged += new EventHandler(this.comboBox_SelectedIndexChanged);

                panel.Controls.Add(tmp_cb);
            }

        }
        private void initializeComponent()
        {
            int x_index = 0;
            int y_index = 0;
            this.components = new System.ComponentModel.Container();
            panel = new System.Windows.Forms.TableLayoutPanel();  
            panel.ColumnCount = 2;
            panel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            panel.Location = new System.Drawing.Point(34, 29);
            panel.Name = "tableLayoutPanel1";
            panel.Size = new System.Drawing.Size(310, 581);
            panel.TabIndex = 0;
            panel.AutoScroll = true;
            panel.Paint += new System.Windows.Forms.PaintEventHandler(this.tableLayoutPanel1_Paint);
            
    
            foreach (Element e in elements)
            {
                /*
                TableLayoutPanel elementPanel = new System.Windows.Forms.TableLayoutPanel();
                elementPanel.ColumnCount = 2;
                elementPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
                elementPanel.Name = "panel" + e.name;
                elementPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
                elementPanel.Size = new System.Drawing.Size(350, 250);
                elementPanel.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowOnly;
                elementPanel.AutoSize = true;
                */
                Label tmp_l = new Label();
                tmp_l.AutoSize = true;
                tmp_l.Name = e.type;
                tmp_l.Size = new System.Drawing.Size(35, 13);
                tmp_l.Text = e.type;
                ComplexType tmp_ct;
                //hay que comprobar si son tipo simple, simplextype o cumpuestos si son simple hay que poner un text box si son complejos añadir el complejo.
                if (e.type == null || e.type.Equals(""))
                {
                    //es un tipo simpleType
                }
                else if (myDic.ContainsKey(e.type))
                {
                    //es un tipo complejo
                    //Program.resolveComplex(ref tmp_ct, myDic);
                    panel.Controls.Add(tmp_l);
                    y_index++;
                    Label tmp_name = new Label();
                    tmp_name.AutoSize = true;
                    tmp_name.Size = new System.Drawing.Size(35, 13);
                    tmp_name.Text = e.name;
                    panel.Controls.Add(tmp_name);
                    //antes de resolverlo hay que poner nombre del tipo y de la instancia ya que puede haber mas de una
                    //resolveComplexType(tmp_ct, panel, myDic, ref x_index, ref y_index, 5);
                    panel.Controls.AddRange(getAllControls(e.type).ToArray());
                    int i = 0;

                }
                else
                {
                    // es un tipo simple

                }
                //myPanels.Add(elementPanel);
                //panel.Controls.Add(elementPanel);
                
            }





            // 
            // contextMenuStrip1
            // 
            // 
            // Form2
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1032, 622);
            this.Controls.Add(panel);
            this.Name = "Form2";
            this.Text = "Form2";
            this.Load += new System.EventHandler(this.Form2_Load);
            panel.ResumeLayout(false);
            panel.PerformLayout();
            this.ResumeLayout(false);
        }
        private void movePanelItems(int row, int offset)
        {
            int last = panel.Controls.Count;
            int currentRow = last / 2;
            while(row<=currentRow)
            {
                Control tmp_c1 = panel.Controls[last -2];
                Control tmp_c2 = panel.Controls[last -1];

                
                
                panel.Controls.Add(tmp_c1, 0, currentRow +offset);
                panel.Controls.Add(tmp_c2, 1, currentRow +offset);

                
                currentRow--;
                last-=2;

            }
            

           


        }
        private void comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            panel.SuspendLayout();
            ComboBox tmp = sender as ComboBox;  
            if(choice.ContainsKey(tmp))
            {
                //el choice ha sido usado y hay que borrar todo lo que hay antes de poner escribrir lo nuevo
                Control tmp_c=new Control();
                choice.TryGetValue(tmp, out tmp_c);
                if(tmp_c==null)
                {
                    int index = panel.Controls.IndexOf(tmp);
                    removeControls(index, panel.Controls.Count-1);
                }
                else
                {
                    int deleteUntil = panel.Controls.IndexOf(tmp_c)-1;
                    int index = panel.Controls.IndexOf(tmp);
                    removeControls(index, deleteUntil);
                }              
            }
            else
            {
                Control next = null;
                if(panel.Controls.IndexOf(sender as Control)+1 < panel.Controls.Count)
                   next = panel.Controls[panel.Controls.IndexOf(sender as Control) + 1];
                choice.Add(sender as Control, next);
            }
            List<Control> lista = this.getAllControls(tmp.Text);
            addControlsAt(lista, panel.Controls.IndexOf(tmp) + 1);
            panel.AutoScroll = false;
            panel.AutoScroll = true;
            panel.ResumeLayout();
            
           
        }
        private void addControlsAt(List<Control> list, int index)
        {
            panel.Controls.AddRange(list.ToArray());
            foreach (Control control in list)
            {
                panel.Controls.SetChildIndex(control, index);
                index++;
            }

        }
        private void removeControls(int start, int finish)
        {
            for (; start < finish; finish--)
            {
                panel.Controls.RemoveAt(finish);
            }

        }
        private void button_Click(object sender, EventArgs e)
        {
            panel.SuspendLayout();
            Control button = sender as Control;
            int buttonIndex = panel.Controls.IndexOf(button);
            Label copy=null;
            String value = button.Name;
            bool complex = false;
            if(myDic.ContainsKey(value))
            {
                complex = true;
                List<Control> list = getAllControls(value);
                panel.Controls.AddRange(list.ToArray());

                foreach(Control control in list)
                {
                    buttonIndex++;
                    panel.Controls.SetChildIndex(control, buttonIndex);
                }
                Label empty = new Label();
                empty.Name = "empty";
                panel.Controls.Add(empty);
                panel.Controls.SetChildIndex(empty,buttonIndex+1);
                Button delete = createDeleteButtonAt(buttonIndex + 2, complex);
                if(!addDelete.ContainsKey(sender as Button))
                {
                    List<Button> deleteList = new List<Button>();
                    deleteList.Add(delete);
                    addDelete.Add(sender as Button, deleteList);
                }
                else
                {
                    List<Button> deleteList = addDelete[sender as Button];
                    deleteList.Add(delete);
                    addDelete[sender as Button] = deleteList;
                }
                buttons.Add(delete, sender as Button);

            }
            else
            {
                Label original = panel.Controls[buttonIndex-2] as Label; 
                copy = new Label();
                copy.Size = original.Size;
                copy.Anchor = original.Anchor;
                copy.Name = original.Name;
                copy.Text = original.Text;
                
                MaskedTextBox originalTextBox = panel.Controls[buttonIndex - 1] as MaskedTextBox;
                MaskedTextBox copy2 = new MaskedTextBox();
                copy2.Size = originalTextBox.Size;
                copy2.Anchor = originalTextBox.Anchor;
                copy2.Name = originalTextBox.Name;

                panel.Controls.Add(copy);
                panel.Controls.Add(copy2);

                panel.Controls.SetChildIndex(copy, buttonIndex+2);
                panel.Controls.SetChildIndex(copy2, buttonIndex+3);

                
                Label empty = new Label();
                empty.Name = "empty";
                panel.Controls.Add(empty);
                Button delete = createDeleteButtonAt(buttonIndex + 4, complex);
                panel.Controls.SetChildIndex(empty, buttonIndex + 5);
               
                if (!addDelete.ContainsKey(sender as Button))
                {
                    List<Button> deleteList = new List<Button>();
                    deleteList.Add(delete);
                    addDelete.Add(sender as Button, deleteList);
                }
                else
                {
                    List<Button> deleteList = addDelete[sender as Button];
                    deleteList.Add(delete);
                    addDelete[sender as Button] = deleteList;
                }
                buttons.Add(delete, sender as Button);
            }
            panel.ResumeLayout();
        }
        private void delete_Click(object sender, EventArgs e)
        {
            
            panel.SuspendLayout();
            Button delete = sender as Button;
            Button add = buttons[delete];
            List<Button> deleteButtons = addDelete[add];
            int deleteIndexInButtonsList = deleteButtons.IndexOf(delete);
            if(deleteIndexInButtonsList+1==deleteButtons.Count)
            {
                removeControls(panel.Controls.IndexOf(add) , panel.Controls.IndexOf(delete));
            }
            else
            {
                Button previous = deleteButtons[deleteIndexInButtonsList + 1];
                removeControls(panel.Controls.IndexOf(previous), panel.Controls.IndexOf(delete));
            }
            deleteButtons.Remove(delete);
            addDelete[add] = deleteButtons;
            panel.AutoScroll = false;
            panel.AutoScroll = true;
            panel.ResumeLayout();
           
           
            
        }
        private Button createDeleteButtonAt(int index, bool complex)
        {
            Button delete = new Button();
            delete.Text = "DELETE";
            delete.Size = new Size(100, 25);
            delete.Anchor = AnchorStyles.Right;
            delete.Name = "Delete";
            delete.Click += new System.EventHandler(this.delete_Click);
            if(!complex)
            {
                delete.Anchor = AnchorStyles.Left;
                panel.Controls.Add(delete);
                panel.Controls.SetChildIndex(delete, index);
            }
            else
            {
                panel.Controls.Add(delete);
                panel.Controls.SetChildIndex(delete, index);
            }
            return delete;

        }
        private int count(String typeName)
        {
            if (myDic.ContainsKey(typeName))
            {
                ComplexType count;
                myDic.TryGetValue(typeName, out count);
                return this.count(count, myDic);
            }
            else
                return 1;
        }
        private int count(ComplexType complex, Dictionary<string, ComplexType> myDic)
        {
            int number = 0;
            //to do: añadir label con el nombre del campo complejo para saber que se esta rellenando
            List<string> nameOfComplex = new List<string>();
            Program.resolveComplex(ref complex, myDic, ref nameOfComplex);
            number++;

            if (complex.elements != null)
            {
                number += complex.elements.Count;

            }
            if (complex.complexElements != null)
            {
            
                foreach (ComplexType e in complex.complexElements)
                {
                   
                   number+= count(e, myDic);
                    
                }
            }
            if (complex.choice != null)
            {
                number++;
            }
            return number;
        }
        private List<Control> getAllControls(String typeName)
        {
            List<Control> controls = new List<Control>();
            if(myDic.ContainsKey(typeName))
            {
                ComplexType complex;
                myDic.TryGetValue(typeName, out complex);
                complex = complex.copy();
                List<string> list = new List<string>();
                complex = Program.resolveComplex(complex, myDic, ref list);
                //Program.resolveComplex(ref complex, myDic, ref list);
                getAllControls(ref controls,complex,list);
            }
            else
            {
                Label label = new Label();
                label.AutoSize = true;
                label.Name = typeName;
                label.Size = new System.Drawing.Size(35, 13);
                label.Text = typeName;
                label.Anchor = AnchorStyles.Left;
                controls.Add(label);

                MaskedTextBox textBox = new MaskedTextBox();
                textBox.Name=typeName;
                textBox.Size= new Size(100,13);
                textBox.Anchor = AnchorStyles.Right;
                controls.Add(textBox);
            }
            return controls;
        }
        private void getAllControls(ref List<Control> list, ComplexType complex, List<string> lista)
        {
            //to do: añadir label con el nombre del campo complejo para saber que se esta rellenando
            

            //añadir etiquita para saber que se esta rellenando
            Label complexHeadLine = new Label();
            complexHeadLine.AutoSize = true;
            complexHeadLine.Name = complex.name;
            complexHeadLine.Size = new System.Drawing.Size(35, 13);
            complexHeadLine.Text = complex.name;
            list.Add(complexHeadLine);
            if (complex.getMax() <= 1)
            {
                Label complexHeadLine2 = new Label();
                complexHeadLine2.AutoSize = true;
                complexHeadLine2.Name = complex.elementName;
                complexHeadLine2.Size = new System.Drawing.Size(35, 13);
                complexHeadLine2.Text = complex.elementName;
                list.Add(complexHeadLine2);
            }
            else
            {
                complexHeadLine.Text += ":" + complex.elementName;
                Button buttonAdd = new Button();
                buttonAdd.Size = new Size(100, 25);
                buttonAdd.Text = "ADD";
                buttonAdd.Name = complex.elementName;
                buttonAdd.Anchor = AnchorStyles.Right;
                buttonAdd.Click += new System.EventHandler(this.button_Click);

                list.Add(buttonAdd);

            }
            if (complex.elements != null)
            {
                foreach (Element e in complex.elements)
                {
                    //to do: añadir el elemento al panel. Hay que comprobar si es simpleType antes de añadir
                    //si es simple type hay que añadir combox para seleccionar s
                    //si tiene type un textbox para introducir el tipo
                    Label tmp_l = new Label();
                    tmp_l.AutoSize = true;
                    tmp_l.Name = e.name;
                    tmp_l.Size = new System.Drawing.Size(35, 13);
                    tmp_l.Text = e.name;
                    tmp_l.Anchor = AnchorStyles.Left;
                    list.Add(tmp_l);
                    if ((e.type == null || e.type.Equals("")) && e.simpleType != null)
                    {

                        ComboBox tmp_cb = new ComboBox();
                        foreach (RestrictionEnum res in e.simpleType.restricction.validOptions)
                        {
                            tmp_cb.Items.Add(res.ToString());
                        }
                        tmp_cb.Size = new Size(100, 13);
                        tmp_cb.Anchor = AnchorStyles.Right;
                        list.Add(tmp_cb);

                    }
                    else if (e.type.Equals("boolean"))
                    {
                        ComboBox tmp_cb = new ComboBox();
                        tmp_cb.Items.Add("TRUE");
                        tmp_cb.Items.Add("FALSE");
                        tmp_cb.Size = new Size(100, 13);
                        tmp_cb.Anchor = AnchorStyles.Right;
                        list.Add(tmp_cb);


                    }
                    else
                    {
                        //to do: mirar si puede ser null o no. Cambiar tipo por maskedType y añadir restriccion de typo
                        //restriccion numeric o string
                        MaskedTextBox tmp_t = new MaskedTextBox();
                        tmp_t.Size = new Size(100, 13);
                        tmp_t.Name = e.name;
                        tmp_t.Anchor = AnchorStyles.Right;
                        list.Add(tmp_t);
                        if(e.max>1)
                        {
                            Button buttonAdd = new Button();
                            buttonAdd.Size = new Size(100, 25);
                            buttonAdd.Text = "ADD";
                            buttonAdd.Anchor = AnchorStyles.Left;
                            buttonAdd.Click += new System.EventHandler(this.button_Click);
                            list.Add(buttonAdd);
                            Label emptyLabel = new Label();
                            list.Add(emptyLabel);

                        }

                    }
                   
                    // hay qeu comprobar si maxocur es mayor que 1 para saber si hay que añadir botones de añadir o borrar ultimo.

                }

            }
            if (complex.complexElements != null)
            {

                
                foreach (ComplexType e in complex.complexElements)
                {
                    
                    if (lista.Count > 0)
                    {
                        e.elementName = lista.ElementAt(0);
                        lista.RemoveAt(0);
                    }
                    getAllControls(ref list,e,lista);
                    
                }
            }
            if (complex.choice != null)
            {
                //hay que añadir un choice
                Label tmp_l = new Label();
                tmp_l.AutoSize = true;
                tmp_l.Name = "CHOICE";
                tmp_l.Size = new System.Drawing.Size(35, 13);
                tmp_l.Text = "CHOICE";
                tmp_l.Anchor = AnchorStyles.Left;
                list.Add(tmp_l);

                ComboBox tmp_cb = new ComboBox();
                tmp_cb.Size = new System.Drawing.Size(100, 13);
                tmp_cb.Anchor = AnchorStyles.Right;
                foreach (Element e in complex.choice)
                {
                    tmp_cb.Items.Add(e.name);
                }
                tmp_cb.SelectedIndexChanged += new EventHandler(this.comboBox_SelectedIndexChanged);

                list.Add(tmp_cb);
            }
            // hay que comprobar si es maxocur diferente de uno para añadir boton de añadir
           
        }

    }
}
