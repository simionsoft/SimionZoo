using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Schema;

namespace FormularioXML
{
    static class Program
    {
        /// <summary>
        /// Punto de entrada principal para la aplicación.
        /// </summary>
        [STAThread]
        static void Main()
        {
            //LOAD XSD
            List<Element> xsdElements = new List<Element>();
            Dictionary<string, ComplexType> myDic = new Dictionary<string, ComplexType>();

            loadXsd(ref xsdElements, ref myDic);

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            //Application.Run(new Form1());
            Application.Run(new Form2(xsdElements,myDic));


        }
        private static void loadXsd(ref List<Element> elements, ref Dictionary<string,ComplexType> dicTranslator)
        {
            XmlSchemaSet schemaSet = new XmlSchemaSet();
            schemaSet.ValidationEventHandler += new ValidationEventHandler(ValidationCallback);
            schemaSet.Add("http://www.w3.org/2001/XMLSchema", "RLSimion.xsd");
            schemaSet.Compile();

            XmlSchema xmlSchema = null;
            foreach (XmlSchema schema in schemaSet.Schemas())
            {
                xmlSchema = schema;
            }

            DataSet myDS = new DataSet();
            myDS.ReadXmlSchema("RLSimion.xsd");

            Dictionary<string, List<Element>> elementsWithComplexType = new Dictionary<string, List<Element>>();
            //Dictionary<string, ComplexType> dicTranslator = new Dictionary<string, ComplexType>();
            //List<Element> elements = new List<Element>();

            //Console.WriteLine("numero: " + xmlSchema.Items.Count);

            foreach (object item in xmlSchema.Items)
            {
                XmlSchemaElement schemaElement = item as XmlSchemaElement;
                XmlSchemaComplexType complexType = item as XmlSchemaComplexType;

                if (schemaElement != null)
                {
                    Console.Out.WriteLine("Schema Element: {0}:{1}", schemaElement.Name, schemaElement.SchemaTypeName.Name);
                    //añadir el elemento a la lista de elementos del xsd
                    //comprobar si es un tipo basico o complexType. Si es complexType guardarlos en elementsWithComplexType para resolverlo despues.
                    Element tmp_e = new Element(schemaElement.Name, schemaElement.SchemaTypeName.Name);

                    elements.Add(tmp_e);
                    if (schemaElement.ElementSchemaType is XmlSchemaComplexType)
                    {
                        List<Element> returnedList = new List<Element>();
                        //comprobar si existe una entrada para el tipo de datos complejo en elementwithcomplextype
                        if (elementsWithComplexType.Count == 0 || !elementsWithComplexType.TryGetValue(tmp_e.type, out returnedList))
                        {

                            List<Element> tmp = new List<Element>();
                            tmp.Add(tmp_e);
                            elementsWithComplexType.Add(tmp_e.type, tmp);
                        }
                        else
                        {
                            returnedList.Add(tmp_e);
                        }


                    }
                    XmlSchemaType schemaType = schemaElement.SchemaType;


                    XmlSchemaComplexType schemaComplexType = schemaType as XmlSchemaComplexType;

                    if (schemaComplexType != null)
                    {

                        XmlSchemaParticle particle = schemaComplexType.Particle;
                        XmlSchemaSequence sequence = particle as XmlSchemaSequence;

                        if (sequence != null)
                        {
                            foreach (XmlSchemaElement childElement in sequence.Items)
                            {

                                Console.Out.WriteLine("    Element/Type: {0}:{1}", childElement.Name,
                                                      childElement.SchemaTypeName.Name);
                            }

                        }
                        if (schemaComplexType.AttributeUses.Count > 0)
                        {
                            IDictionaryEnumerator enumerator = schemaComplexType.AttributeUses.GetEnumerator();

                            while (enumerator.MoveNext())
                            {
                                XmlSchemaAttribute attribute = (XmlSchemaAttribute)enumerator.Value;

                                Console.Out.WriteLine("      Attribute/Type: {0}", attribute.Name);
                            }
                        }
                    }
                }
                else if (complexType != null)
                {
                    Console.Out.WriteLine("Complex Type: {0}", complexType.Name);
                    ComplexType ct = new ComplexType(complexType.Name);
                    dicTranslator.Add(ct.name, ct);
                    OutputElements(complexType.Particle, ct);
                    if (complexType.AttributeUses.Count > 0)
                    {
                        IDictionaryEnumerator enumerator = complexType.AttributeUses.GetEnumerator();

                        while (enumerator.MoveNext())
                        {
                            //nuestros xsd no contiene atributos si los tuviera habria que tratarlos
                            XmlSchemaAttribute attribute = (XmlSchemaAttribute)enumerator.Value;
                            Console.Out.WriteLine("      Attribute/Type: {0}", attribute.Name);
                        }
                    }
                }
                Console.Out.WriteLine();
            }

            Console.Out.WriteLine();
            //al finalizar hay una:
            //                  lista de elementos dentro del xsd
            //                  una diccionario con todos los tipos de datos complejos accesible por el nombre del campo
            //                  una lista con los elementos que son de tipo complejo
            Console.WriteLine(dicTranslator.Count);

            //elementIn(ref elements, dicTranslator);
            Console.In.ReadLine();
            ComplexType hola = new ComplexType("hola");
            // dicTranslator.TryGetValue("critic", out hola );

            //Console.WriteLine();

            //Console.WriteLine(dicTranslator.TryGetValue(elements.ElementAt(2).complexType.elements.ElementAt(0).type, out hola));

            Console.WriteLine();

        }
        /*private static void elementIn(ref List<Element> elements, Dictionary<string, ComplexType> dic)
        {

            //to do: comprobar si algunn elemento es complejo si alguno lo he estara en la lista complexElement
            //los elementos complejos hay que recorrerlos de forma recursiva
            foreach (Element element in elements)
            {

                ComplexType tmp_ct;
                if (dic.TryGetValue(element.type, out tmp_ct))
                {
                    //to do: es un tipo de datos complejo hay que resolverlo y añadirlo al elemento. Este complexType estara resuelto
                    resolveComplex(ref tmp_ct, dic);
                    element.complexType = tmp_ct;
                }

            }

        }*/
        public static void resolveComplex(ref ComplexType elementToResolve, Dictionary<string, ComplexType> dic,ref List<string> complexName)
        {
            for (int index = 0; index < elementToResolve.elements.Count; index++)
            {
                Element element = elementToResolve.elements.ElementAt(index);
                ComplexType tmp_ct;

                if (dic.TryGetValue(element.type, out tmp_ct))
                {
                     //to do: hay qeu quitarlo de la lista element y ponerlo en la lista complex para resolverlo
                    tmp_ct.setFather(element);
                    elementToResolve.addComplexElemente(tmp_ct);
                    elementToResolve.elements.Remove(element);
                    index--;
                }
                /*
                   else if(element.type==null || element.type.Equals(""))
                   {
                       //to do: es un single
                   }*/
            }
            for (int i = 0; i < elementToResolve.getComplexSize(); i++)
            {
                ComplexType tmp_ct = elementToResolve.complexElements.ElementAt(i);
                resolveComplex(ref tmp_ct, dic, ref complexName);
            }


        }
        public static ComplexType resolveComplex(ComplexType complexToResolve, Dictionary<string, ComplexType> dic, ref List<string> complexName)
        {
            ComplexType result = complexToResolve.copy();

            for (int index = 0; index < result.elements.Count; index++)
            {
                Element element = result.elements.ElementAt(index);
                ComplexType tmp_ct;

                if (dic.TryGetValue(element.type, out tmp_ct))
                {
                    //to do: hay qeu quitarlo de la lista element y ponerlo en la lista complex para resolverlo
                    tmp_ct.elementName = element.name;
                    //tmp_ct.setFather(element);
                    result.addComplexElemente(resolveComplex(tmp_ct,dic, ref complexName));
                    result.complexElements.ElementAt(result.complexElements.Count - 1).setFather(element);
                    result.elements.Remove(element);
                    index--;
                }
                
            }
            return result;

        }
        private static void OutputElements(XmlSchemaParticle particle, ComplexType father)
        {
            XmlSchemaSequence sequence = particle as XmlSchemaSequence;
            XmlSchemaChoice choice = particle as XmlSchemaChoice;
            XmlSchemaAll all = particle as XmlSchemaAll;
            //to do: añadir restriciones a los arboles
            if (sequence != null)
            {
                Console.Out.WriteLine("  Sequence");

                for (int i = 0; i < sequence.Items.Count; i++)
                {
                    XmlSchemaElement childElement = sequence.Items[i] as XmlSchemaElement;
                    XmlSchemaSequence innerSequence = sequence.Items[i] as XmlSchemaSequence;
                    XmlSchemaChoice innerChoice = sequence.Items[i] as XmlSchemaChoice;
                    XmlSchemaAll innerAll = sequence.Items[i] as XmlSchemaAll;

                    if (childElement != null)
                    {
                        //to do: crear un element
                        //este element hay que añadirlo a dicTranslator(lastIndex).complexAttributes(complex creado)
                        //Console.WriteLine("elemento dentro de secuence");
                        Element tmp_e = new Element(childElement.Name, childElement.SchemaTypeName.Name);
                        if (childElement.MinOccurs == 0)
                            tmp_e.canBeNull();
                        if (childElement.MaxOccurs > 1 && childElement.MaxOccursString == null)
                            tmp_e.setMax((int)childElement.MaxOccurs);
                        if (childElement.MaxOccurs >1 && childElement.MaxOccursString!=null)
                            tmp_e.setMax(int.MaxValue);
                        if (childElement.ElementSchemaType is XmlSchemaSimpleType)
                        {

                            Restriction tmp_r = new Restriction(childElement.ElementSchemaType.BaseXmlSchemaType.TypeCode.ToString());
                            XmlSchemaSimpleType simple = childElement.ElementSchemaType as XmlSchemaSimpleType;


                            XmlSchemaSimpleType simple2 = childElement.ElementSchemaType as XmlSchemaSimpleType;
                            XmlSchemaSimpleTypeRestriction res = simple.Content as XmlSchemaSimpleTypeRestriction;
                            if (res is XmlSchemaSimpleTypeRestriction)
                            {
                                foreach (XmlSchemaEnumerationFacet a in res.Facets)
                                {
                                    int c = 0;
                                    if (a.Value.Equals("linear"))
                                    {
                                        tmp_r.addValidOption(RestrictionEnum.linear);
                                    }
                                    else if (a.Value.Equals("quadratic"))
                                    {
                                        tmp_r.addValidOption(RestrictionEnum.quadratic);
                                    }
                                }
                                tmp_e.setSimpleType(new SimpleType(tmp_r));
                            }


                        }
                        father.addElement(tmp_e);
                        Console.Out.WriteLine("    Element/Type: {0}:{1}", childElement.Name,
                                              childElement.SchemaTypeName.Name);
                    }

                    else OutputElements(sequence.Items[i] as XmlSchemaParticle, father);
                }
            }
            else if (choice != null)
            {
                Console.Out.WriteLine("  Choice");
                for (int i = 0; i < choice.Items.Count; i++)
                {
                    XmlSchemaElement childElement = choice.Items[i] as XmlSchemaElement;
                    XmlSchemaSequence innerSequence = choice.Items[i] as XmlSchemaSequence;
                    XmlSchemaChoice innerChoice = choice.Items[i] as XmlSchemaChoice;
                    XmlSchemaAll innerAll = choice.Items[i] as XmlSchemaAll;

                    if (childElement != null)
                    {
                        //to do: crear un complextype si es simple se crea complexx y solo se ponen atributos la ref a complex se deja a null
                        //este elemento del choice hay que añadirlo a myDic(lastIndex).choice(choice creado)
                        //Console.WriteLine("elemento dentro de choice");
                        Element tmp_e = new Element(childElement.Name, childElement.SchemaTypeName.Name);
                        father.addChoice(tmp_e);
                        Console.Out.WriteLine("    Element/Type: {0}:{1}", childElement.Name,
                                              childElement.SchemaTypeName.Name);
                    }
                    //se asume que lo choice no tiene objeto anidados
                    //else OutputElements(choice.Items[i] as XmlSchemaParticle);
                }

                Console.Out.WriteLine();
            }
            //nuestro xsd no tiene all
            /*
            else if (all != null)
            {
                Console.Out.WriteLine("  All");
                for (int i = 0; i < all.Items.Count; i++)
                {
                    XmlSchemaElement childElement = all.Items[i] as XmlSchemaElement;
                    XmlSchemaSequence innerSequence = all.Items[i] as XmlSchemaSequence;
                    XmlSchemaChoice innerChoice = all.Items[i] as XmlSchemaChoice;
                    XmlSchemaAll innerAll = all.Items[i] as XmlSchemaAll;

                    if (childElement != null)
                    {
                        Console.Out.WriteLine("    Element/Type: {0}:{1}", childElement.Name,
                                              childElement.SchemaTypeName.Name);
                    }
                    else OutputElements(all.Items[i] as XmlSchemaParticle);
                }
                Console.Out.WriteLine();
            }*/
        }
        static void ValidationCallback(object sender, ValidationEventArgs args)
        {
            if (args.Severity == XmlSeverityType.Warning)
                Console.Write("WARNING: ");
            else if (args.Severity == XmlSeverityType.Error)
                Console.Write("ERROR: ");

            Console.WriteLine(args.Message);
        }
    }
}
