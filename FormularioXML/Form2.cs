using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;
using System.Xml.Linq;

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
        Dictionary<CheckBox, List<Control>> enable = new Dictionary<CheckBox, List<Control>>();
        private XmlDocument xmldocument= new XmlDocument();
        private XmlNode root;
        private XmlNode currentXmlNode;
        private Dictionary<Control, XmlNode> xmlDyc = new Dictionary<Control, XmlNode>();
        private Dictionary<Control, XmlNode> controlFatherInXmlDocu = new Dictionary<Control, XmlNode>();
       

        public Form2(List<Element> elements, Dictionary<string,ComplexType> myDic)
        {
            this.elements = elements;
            this.myDic = myDic;
            test = elements.ElementAt(0);
            root = xmldocument.CreateElement("RLSimion");
            xmldocument.AppendChild(root);
            initializeComponent();
            
        }

        private void validate(object sender, EventArgs e)
        {
            foreach(ComboBox combo in combos)
            {
               if(combo.SelectedIndex==-1 && combo.Enabled)
               {
                   MessageBox.Show("PLEASE SELECT THE WANTED CHOICE");
                   return;
               }
            }
            foreach(Control key in xmlDyc.Keys)
            {
                String value = key.Text;
                XmlNode node = xmlDyc[key];
                
                if(value.Equals("")&&key.Enabled)
                {                   
                    MessageBox.Show("PLEASE FILL ALL THE INPUTS");       
                    return;
                }
                else
                {
                    //falta hacer la validacion si es bool que cumpla, int o decimal
                    //
                    if(key is ComboBox&& !((ComboBox)key).Items.Contains(value))
                    {
                        MessageBox.Show("PLEASE USE COMBOBOX OPTIONS IN "+key.Name);
                        return;
                    }
                    if(key is TextBox && !validateTextBox(key.AccessibleDescription,value))
                    {
                        MessageBox.Show(key.Name+" VALUE IS NOT VALID. PLEASE USE THE CORRECT DATATYPE");
                        return;
                    }
                    node.InnerText = value;

                }
            }
            cleanNullTagsAndSave();
        }
        private void cleanNullTagsAndSave()
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "XML-File | *.xml";
            if(sfd.ShowDialog()== System.Windows.Forms.DialogResult.OK)
            {
                XElement doc = XElement.Load(new XmlNodeReader(xmldocument));
                doc.Descendants().Where(t => string.IsNullOrEmpty(t.Value)).Remove();
                doc.Save(sfd.FileName);
            }
            
        }
        private void cleanNullTagsAndSave(string path)
        {
            XElement doc = XElement.Load(new XmlNodeReader(xmldocument));
            doc.Descendants().Where(t => string.IsNullOrEmpty(t.Value)).Remove();
            doc.Save(path);
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
                        tmp_cb.Size = new Size(100, 25);
                        tmp_cb.Anchor = AnchorStyles.Left;
                        panel.Controls.Add(tmp_cb);

                    }
                    else if (e.type.Equals("boolean"))
                    {
                        ComboBox tmp_cb = new ComboBox();
                        tmp_cb.Items.Add("TRUE");
                        tmp_cb.Items.Add("FALSE");
                        tmp_cb.Size = new Size(100, 25);
                        tmp_cb.Anchor = AnchorStyles.Left;
                        panel.Controls.Add(tmp_cb);


                    }
                    else
                    {
                        TextBox tmp_t = new TextBox();
                        tmp_t.Size = new Size(100, 25);
                        tmp_t.Name = e.name;
                        tmp_t.AccessibleDescription = e.type;
                        tmp_t.Anchor = AnchorStyles.Left;
                        panel.Controls.Add(tmp_t);
                    }
                    
                    

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
                tmp_l.Text = complex.name;
                tmp_l.Anchor = AnchorStyles.Left;
                panel.Controls.Add(tmp_l);

                ComboBox tmp_cb = new ComboBox();
                tmp_cb.Size = new System.Drawing.Size(100, 13);
                tmp_cb.Anchor = AnchorStyles.Left;
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
            panel.Size = new System.Drawing.Size(300, 581);
            panel.TabIndex = 0;
            panel.AutoScroll = true;
            panel.Paint += new System.Windows.Forms.PaintEventHandler(this.tableLayoutPanel1_Paint);

            Label title = new Label();
            title.AutoSize = true;
            title.Name = "Title";
            title.Size = new System.Drawing.Size(35, 13);
            title.Text = "RLSimion";
            title.Anchor = AnchorStyles.Left;
            panel.Controls.Add(title);

            Label title2 = new Label();
            title2.AutoSize = true;
            title2.Name = "Title";
            title2.Size = new System.Drawing.Size(35, 13);
            title2.Text = "";
            title2.Anchor = AnchorStyles.Left;
            panel.Controls.Add(title2);
    
            foreach (Element e in elements)
            {

                XmlNode elementNode = xmldocument.CreateElement(e.name);
                root.AppendChild(elementNode);
                this.currentXmlNode = elementNode;

                Label tmp_l = new Label();
                tmp_l.AutoSize = true;
                tmp_l.Name = e.type;
                tmp_l.Size = new System.Drawing.Size(35, 13);
                tmp_l.Text = e.name;
                tmp_l.Anchor = AnchorStyles.Left;
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
                    tmp_name.Text = "";// e.name;
                    panel.Controls.Add(tmp_name);
                    //antes de resolverlo hay que poner nombre del tipo y de la instancia ya que puede haber mas de una
                    //resolveComplexType(tmp_ct, panel, myDic, ref x_index, ref y_index, 5);
                    panel.Controls.AddRange(getAllControls(e.type).ToArray());
                    

                }
                else
                {
                    // es un tipo simple

                }
                
                                
            }

            Button validate = new Button();
            validate.Size= new Size(100, 35);
            validate.Text = "SAVE";
            validate.Location = new Point(500, 300);
            validate.Click += new EventHandler(this.validate);
            this.Controls.Add(validate);

            Button load = new Button();
            load.Size = new Size(100, 35);
            load.Text = "LOAD";
            load.Location = new Point(500, 340);
            load.Click += new EventHandler(this.load);
            this.Controls.Add(load);

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

        private void load(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "XML-File | *.xml";
            if(ofd.ShowDialog() == DialogResult.OK)
            {
                String fileName = ofd.FileName;
                XmlDocument doc = new XmlDocument();
                doc.Load(fileName);
                XmlNode rl = doc.LastChild;
                foreach(XmlNode node in rl.ChildNodes)
                {
                    fillForm(node);
                }
               
            }

        }
        private void fillForm(XmlNode node)
        {
            
            if(node.HasChildNodes)
            {
                Console.WriteLine(node.Name);
                var children = node.ChildNodes;
                int i = 0;
                foreach (XmlNode nodo in node.ChildNodes)
                {
                    if( i<children.Count-1 && children[i].Name.Equals(children[i+1].Name))
                    {
                        
                        //es un unbounnd hay qeu darle al botton de add
                        Console.WriteLine("multi");
                    }
                    i++;
                    fillForm(nodo);
                }
            }
            else
            {
                Console.WriteLine(node.InnerText);
            }

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
                //eliminar el child viejo del xml
                controlFatherInXmlDocu[sender as Control].RemoveChild(controlFatherInXmlDocu[sender as Control].ChildNodes.Item(controlFatherInXmlDocu[sender as Control].ChildNodes.Count-1));
               
            }
            else
            {
                Control next = null;
                if(panel.Controls.IndexOf(sender as Control)+1 < panel.Controls.Count)
                   next = panel.Controls[panel.Controls.IndexOf(sender as Control) + 1];
                choice.Add(sender as Control, next);
            }
            XmlNode child = this.xmldocument.CreateElement(tmp.Text);
            this.controlFatherInXmlDocu[sender as Control].AppendChild(child);
            currentXmlNode = child;
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
                if (panel.Controls[finish] is ComboBox && combos.Contains(panel.Controls[finish] as ComboBox))
                    combos.Remove(panel.Controls[finish] as ComboBox);
                if (xmlDyc.ContainsKey(panel.Controls[finish]))
                    xmlDyc.Remove(panel.Controls[finish]);
                panel.Controls.RemoveAt(finish);
            }

        }
        private void button_Click(object sender, EventArgs e)
        {
            //corregir hay que añadir en el sitio correcto no al final
            panel.SuspendLayout();
            Control button = sender as Control;
            int buttonIndex = panel.Controls.IndexOf(button);
            Label copy=null;
       
            String value = button.Name;
            bool complex = false;
            if(myDic.ContainsKey(value))
            {

                
                complex = true;
                currentXmlNode = controlFatherInXmlDocu[sender as Control].ParentNode;
                XmlNode node = xmldocument.CreateElement(controlFatherInXmlDocu[sender as Control].Name);
                //el nodo hay que ponerlo en la posicion correcto no al final
                currentXmlNode.InsertBefore(node, controlFatherInXmlDocu[sender as Control]);
                /*var child = currentXmlNode.ChildNodes;
                foreach(XmlNode nodo in child)
                {
                    if(nodo.Name.Equals(node.Name))
                    {
                        currentXmlNode.InsertBefore(node, nodo);
                        break;
                    }
                }*/
                //currentXmlNode.AppendChild(node);    
                currentXmlNode = node;
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
                controlFatherInXmlDocu.Add(delete, node);

            }
            else
            {
                Label original = panel.Controls[buttonIndex-2] as Label; 
                copy = new Label();
                copy.Size = original.Size;
                copy.Anchor = original.Anchor;
                copy.Name = original.Name;
                copy.Text = original.Text;
                
                TextBox originalTextBox = panel.Controls[buttonIndex - 1] as TextBox;
                TextBox copy2 = new TextBox();
                copy2.Size = originalTextBox.Size;
                copy2.AccessibleDescription = original.AccessibleDescription;
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

                //fijar el padre

                XmlNode father = controlFatherInXmlDocu[sender as Control];
                XmlElement child = xmldocument.CreateElement(copy2.Name);
                father.AppendChild(child);
                xmlDyc.Add(copy2, child);
               
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
                controlFatherInXmlDocu.Add(delete, child);
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
            if(controlFatherInXmlDocu.ContainsKey(sender as Control))
            {
                XmlNode g = controlFatherInXmlDocu[sender as Control];
                g.ParentNode.RemoveChild(g);
            }
            else
            {

            }
            //xmldocument.RemoveChild(controlFatherInXmlDocu[sender as Control]);
            
            panel.AutoScroll = false;
            panel.AutoScroll = true;
            panel.ResumeLayout();
           
           
            
        }
        private Button createDeleteButtonAt(int index, bool complex)
        {
            Button delete = new Button();
            delete.Text = "DELETE";
            delete.Size = new Size(100, 25);
            delete.Anchor = AnchorStyles.Left;
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

                TextBox textBox = new TextBox();
                textBox.Name=typeName;
                textBox.AccessibleDescription = typeName;
                textBox.Size= new Size(100,25);
                textBox.Anchor = AnchorStyles.Left;
                controls.Add(textBox);

                //hay qeu añadir el elemento al arbol
                //XmlNode child = xmldocument.CreateElement(typeName);
                //currentXmlNode.AppendChild(child);
                xmlDyc.Add(textBox, currentXmlNode);
            }
            return controls;
        }
        private void getAllControls(ref List<Control> list, ComplexType complex, List<string> lista)
        {
            XmlNode father = currentXmlNode;
            //to do: añadir label con el nombre del campo complejo para saber que se esta rellenando
            CheckBox nullCheckBox = null;

            //añadir etiquita para saber que se esta rellenando
            Label complexHeadLine = new Label();
            complexHeadLine.AutoSize = true;
            complexHeadLine.Name = complex.name;
            complexHeadLine.Size = new System.Drawing.Size(35, 13);
            complexHeadLine.Text = complex.elementName;
            complexHeadLine.Anchor = AnchorStyles.Left;
            list.Add(complexHeadLine);
            Button buttonAdd = null;
            List<Control> controls = null;
            if (complex.getMax() == 1)
            {
                Label complexHeadLine2 = new Label();
                complexHeadLine2.AutoSize = true;
                complexHeadLine2.Name = complex.elementName;
                complexHeadLine2.Size = new System.Drawing.Size(35, 13);
                complexHeadLine2.Text = "";// complex.elementName;
                list.Add(complexHeadLine2);
            }
            else
            {
                //complexHeadLine.Text =  complex.elementName;
                buttonAdd = new Button();
                buttonAdd.Size = new Size(100, 25);
                buttonAdd.Text = "ADD";
                buttonAdd.Name = complex.name;
                buttonAdd.Anchor = AnchorStyles.Left;
                buttonAdd.Click += new System.EventHandler(this.button_Click);
                controlFatherInXmlDocu.Add(buttonAdd, currentXmlNode);
                list.Add(buttonAdd);

            }
            if(complex.getMin()==0)
            {
                controls = new List<Control>();
                if(buttonAdd!=null)
                    controls.Add(buttonAdd);
                nullCheckBox = new CheckBox();
                nullCheckBox.Name = "NULL";
                nullCheckBox.Text = "IS NULL";
                nullCheckBox.Checked = false;
                nullCheckBox.Anchor = AnchorStyles.Left;
                nullCheckBox.Click += new EventHandler(this.checkBox);
                list.Add(nullCheckBox);
                Label empty = new Label();
                list.Add(empty);

            }
            if (complex.elements != null)
            {
                foreach (Element e in complex.elements)
                {
                    XmlNode child = xmldocument.CreateElement(e.name);
                    currentXmlNode.AppendChild(child);
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
                        tmp_cb.Size = new Size(100, 25);
                        tmp_cb.Anchor = AnchorStyles.Left;
                        list.Add(tmp_cb);
                        xmlDyc.Add(tmp_cb, child);
                        if (controls != null)
                            controls.Add(tmp_cb);
                    }
                    else if (e.type.Equals("boolean"))
                    {
                        ComboBox tmp_cb = new ComboBox();
                        tmp_cb.Items.Add("TRUE");
                        tmp_cb.Items.Add("FALSE");
                        tmp_cb.Size = new Size(100, 25);
                        tmp_cb.Anchor = AnchorStyles.Left;
                        list.Add(tmp_cb);
                        xmlDyc.Add(tmp_cb, child);
                        if (controls != null)
                            controls.Add(tmp_cb);

                    }
                    else
                    {
                       
                        //to do: mirar si puede ser null o no. Cambiar tipo por maskedType y añadir restriccion de typo
                        //restriccion numeric o string
                        TextBox tmp_t = new TextBox();
                        tmp_t.Size = new Size(100, 25);
                        tmp_t.Name = e.name;
                        tmp_t.AccessibleDescription = e.type;
                        tmp_t.Anchor = AnchorStyles.Left;
                        list.Add(tmp_t);
                        xmlDyc.Add(tmp_t, child);
                        if (controls != null)
                            controls.Add(tmp_t);
                        Button buttonAdd2=null;
                        if(e.max>1)
                        {
                            buttonAdd2 = new Button();
                            buttonAdd2.Size = new Size(100, 25);
                            buttonAdd2.Text = "ADD";
                            buttonAdd2.Anchor = AnchorStyles.Left;
                            buttonAdd2.Click += new System.EventHandler(this.button_Click);
                            list.Add(buttonAdd2);
                            if (controls != null)
                                controls.Add(buttonAdd2);
                            Label emptyLabel = new Label();
                            list.Add(emptyLabel);
                            controlFatherInXmlDocu.Add(buttonAdd2, currentXmlNode);

                        }
                        if (e.min == 0)
                        {
                            List<Control> control = new List<Control>();
                            control.Add(tmp_t);
                            if (buttonAdd != null)
                                control.Add(buttonAdd2);
                            CheckBox nullCheckBox2 = new CheckBox();
                            nullCheckBox2.Name = "NULL";
                            nullCheckBox2.Click += new EventHandler(this.checkBox);
                            nullCheckBox2.Text = "IS NULL";
                            nullCheckBox2.Checked = false;
                            nullCheckBox2.Anchor = AnchorStyles.Left;
                            if (controls != null)
                                controls.Add(nullCheckBox2);
                            list.Add(nullCheckBox2);
                            enable.Add(nullCheckBox2, control);
                            Label empty = new Label();
                            list.Add(empty);
                        }

                    }
                   
                    // hay qeu comprobar si maxocur es mayor que 1 para saber si hay que añadir botones de añadir o borrar ultimo.

                }

            }
            if (complex.complexElements != null)
            {

                
                foreach (ComplexType e in complex.complexElements)
                {
                    XmlNode complexChild = xmldocument.CreateElement(e.elementName);
                    currentXmlNode.AppendChild(complexChild);
                    currentXmlNode = complexChild;                  
                    if (lista.Count > 0)
                    {
                        
                        e.elementName = lista.ElementAt(0);
                        lista.RemoveAt(0);
                    }
                    getAllControls(ref list,e,lista);
                    currentXmlNode = father;
                    //complexChild.InnerText = "prueba";
                    
                }
                if(controls!=null)
                {
                    int index = list.IndexOf(controls[controls.Count - 1]);
                    for(;index<list.Count;index++)
                    {
                        Control c = list[index];
                        if(!(c is Label))
                        {
                            controls.Add(c);
                        }
                    }
                }
            }
            if (complex.choice != null)
            {
                //hay que añadir un choice
                Label tmp_l = new Label();
                tmp_l.AutoSize = true;
                tmp_l.Name = "CHOICE";
                tmp_l.Size = new System.Drawing.Size(35, 13);
                tmp_l.Text = complex.name;
                tmp_l.Anchor = AnchorStyles.Left;
                list.Add(tmp_l);

                ComboBox tmp_cb = new ComboBox();
                tmp_cb.Size = new System.Drawing.Size(100, 13);
                tmp_cb.Anchor = AnchorStyles.Left;
                foreach (Element e in complex.choice)
                {
                    tmp_cb.Items.Add(e.type);
                }
                tmp_cb.SelectedIndexChanged += new EventHandler(this.comboBox_SelectedIndexChanged);
                Size oo = tmp_cb.Size;
                if (controls != null)
                    controls.Add(tmp_cb);
                list.Add(tmp_cb);
                controlFatherInXmlDocu.Add(tmp_cb, currentXmlNode);
                this.combos.Add(tmp_cb);
                
            }
            if (controls != null)
                enable.Add(nullCheckBox, controls);
            // hay que comprobar si es maxocur diferente de uno para añadir boton de añadir
           
        }

        private void checkBox(object sender, EventArgs e)
        {
            CheckBox checkBox = sender as CheckBox;
            if(checkBox.Checked)
            {
                List<Control> t = enable[sender as CheckBox];
                foreach(Control control in enable[checkBox])
                {
                    control.Enabled = false;
                }
            }
            else
            {
                foreach (Control control in enable[checkBox])
                {
                    if(control is CheckBox && control.Name.Equals("NULL"))
                    {
                        CheckBox tmp = control as CheckBox;
                        if(tmp.Checked)
                        {
                            this.checkBox(tmp, null);
                        }
                      
                    }
                    control.Enabled = true;
                }
            }
        }

        private bool validateTextBox(string type,string value)
        {
            Regex regex;
            switch (type)
            {
                case "decimal":
                    regex = new Regex(@"^(-|)((0\.\d+)|[1-9]\d*(\.\d+))$");
                    if(regex.IsMatch(value))
                    {
                        return true;
                    }
                    return validateTextBox("int",value);
                case "integer":
                    regex = new Regex(@"^(-|)[1-9][0-9]*$");
                    return regex.IsMatch(value);
                default:
                    return true;

            }
             
            
        }

    }
}
