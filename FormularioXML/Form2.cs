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
        Dictionary<Control, Control> choice = new Dictionary<Control, Control>();


        public Form2(List<Element> elements, Dictionary<string,ComplexType> myDic)
        {
            this.elements = elements;
            this.myDic = myDic;
            test = elements.ElementAt(0);
            initializeComponent();
            /* ejemplo para desplazar celdar 
            Control c1 = tableLayoutPanel1.GetControlFromPosition(0, 1);
            Control c2 = tableLayoutPanel1.GetControlFromPosition(0, 2);
            //TableLayoutPanelCellPosition cell1 = tlp.GetCellPosition(c1);
            tableLayoutPanel1.SetCellPosition(c1, new TableLayoutPanelCellPosition(0, 5));
            tableLayoutPanel1.SetCellPosition(c2, new TableLayoutPanelCellPosition(0, 6));
            */
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
                        TextBox tmp_t = new TextBox();
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
            panel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            panel.Size = new System.Drawing.Size(300, 581);
            panel.TabIndex = 0;
            //panel.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            //panel.AutoSize = true;
            
            panel.AutoScroll = true;
            panel.Paint += new System.Windows.Forms.PaintEventHandler(this.tableLayoutPanel1_Paint);
            
            int index = 5;
            foreach (Element e in elements)
            {
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
                else if (myDic.TryGetValue(e.type, out tmp_ct))
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
                    resolveComplexType(tmp_ct, panel, myDic, ref x_index, ref y_index, 5);

                }
                else
                {
                    // es un tipo simple

                }

                
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
                    int index = panel.Controls.IndexOf(tmp)+1;
                    for(int i = index+1; i<=panel.Controls.Count;)
                    {
                        panel.Controls.RemoveAt(index);
                    }
                }
                else
                {
                    int deleteUntil = panel.Controls.IndexOf(tmp_c);
                    int index = panel.Controls.IndexOf(tmp)+1;                 
                    for (; index<deleteUntil; deleteUntil--)
                    {
                        panel.Controls.RemoveAt(index);
                    }
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
            panel.Controls.AddRange(lista.ToArray());
            int position = panel.Controls.IndexOf(tmp) + 1;
            foreach(Control control in lista)
            {
                panel.Controls.SetChildIndex(control, position);
                position++;
            }
            //panel.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            //panel.AutoSize = false;
            panel.AutoScroll = false;
            panel.AutoScroll = true;
            panel.ResumeLayout();
            
            //hay que hacer saber el numero de elemnto que hay qeu añadir al panel, hacer sitio para los elementos y añadirlos
            //movePanelItems(cell1.Row + 1, count(tmp.SelectedText));
           //primero vamos a guardar los controles que estan despues de chocie
            /*List<Control> tmp_controls = new List<Control>();
            for(int index = end+1;index<panel.Controls.Count;)
            {
                tmp_controls.Add(panel.Controls[index]);
               // panel.Controls.RemoveAt(index);
                break;
            }
            Button boton = new Button();
            panel.Controls.Add(boton);
            panel.Controls.SetChildIndex(boton, 5);*/
            
            //panel.Controls.RemoveAt(5);
            //int tt= 6;
            //Control[] ccc = lista.ToArray();
            //panel.Controls.AddRange(lista.ToArray());
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

                TextBox textBox = new TextBox();
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

            Label complexHeadLine2 = new Label();
            complexHeadLine2.AutoSize = true;
            complexHeadLine2.Name = complex.elementName;
            complexHeadLine2.Size = new System.Drawing.Size(35, 13);
            complexHeadLine2.Text = complex.elementName;
            list.Add(complexHeadLine2);

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
                        TextBox tmp_t = new TextBox();
                        tmp_t.Size = new Size(100, 13);
                        tmp_t.Name = e.name;
                        tmp_t.Anchor = AnchorStyles.Right;
                        list.Add(tmp_t);
                    }
                   


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

        }

    }
}
