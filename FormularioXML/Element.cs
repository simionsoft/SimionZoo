using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FormularioXML
{
    public class Element
    {
        public string name;
        public string type;
        public SimpleType simpleType;
        public ComplexType complexType = null;
        public int min = 1;
        public int max = 1;

        public void canBeNull()
        {
            min = 0;
        }
        public bool isNullable()
        {
            return min == 0;
        }
        public void setMax(int max)
        {
            this.max = max;
        }
      
        public Element(string name,string type)
        {
            this.name = name;
            this.type = type;
        }
        public void setSimpleType(SimpleType simpleType)
        {
            this.simpleType = simpleType;
        }
       
    }
    public class SimpleType
    {
        public Restriction restricction;
        public SimpleType(Restriction res)
        {
            this.restricction = res;
        }
    }
    public class Restriction
    {
        public string baseType;
        public List<RestrictionEnum> validOptions=new List<RestrictionEnum>();

        public Restriction(string baseType)
        {
            this.baseType = baseType;
            validOptions = new List<RestrictionEnum>();
        }
        public void addValidOption(RestrictionEnum re)
        {
            validOptions.Add(re);
        }

    }
    public enum RestrictionEnum
    {
        linear,
        quadratic,
    };
    public class ComplexType
    {
        public string elementName;
        public string name;
        public List<Element> elements;
        public List<Attribute> attributes;
        public List<Element> choice;
        public List<ComplexType> complexElements;
        
        public ComplexType copy()
        {
            ComplexType copy = new ComplexType(this.name);
            copy.elementName = elementName;
            if (this.elements != null)
            {
                foreach (Element e in this.elements)
                {
                    copy.addElement(e);
                }
            }
            if (this.choice != null)
            {
                foreach (Element e in this.choice)
                {
                    copy.addChoice(e);
                }
            }
            return copy;

        }
        public int getComplexSize()
        {
            if (complexElements == null)
                return 0;
            else
                return complexElements.Count;
        }
         
        public ComplexType(string name)
        {
            this.name = name;
        }
        public void addElement(Element e)
        {
            if (elements == null)
                elements = new List<Element>();
            elements.Add(e);

        }
        public void addAttribute(Attribute a)
        {
            if (attributes == null)
                attributes = new List<Attribute>();
            attributes.Add(a);
        }
        public void addChoice(Element e)
        {
            if (choice == null)
                choice = new List<Element>();
            choice.Add(e);
        }
        public void addComplexElemente(ComplexType ct)
        {
            if (complexElements == null)
                complexElements = new List<ComplexType>();
            complexElements.Add(ct);
        }
    }
    public class Attribute
    {
        public string name;
        public AttibuteType type;

        public Attribute(string name, AttibuteType type)
        {
            this.name = name;
            this.type = type;
        }
        
    }
    public enum AttibuteType
    {
        Decimal,
        String,
        Integer,
        Boolean,
    };

}
