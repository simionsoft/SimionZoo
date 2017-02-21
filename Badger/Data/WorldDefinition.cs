using System;
using System.Collections.Generic;
using System.Xml;
using Badger.Simion;

namespace Badger.Data
{
    public class WorldVar
    {
        string m_name;
        public string name { get { return m_name; } }
        double m_min;
        public double min { get { return m_min; } }
        double m_max;
        public double max { get { return m_max; } }
        string m_unit;
        public string unit { get { return m_unit; } }
        public WorldVar(string name,double min,double max,string unit)
        {
            m_name = name;
            m_min = min;
            m_max = max;
            m_unit = unit;
        }
        public WorldVar(XmlNode varDefinition)
        {
            m_name = varDefinition.Attributes[XMLConfig.nameAttribute].Value;
            m_min = double.Parse(varDefinition.Attributes[XMLConfig.minValueAttribute].Value);
            m_max = double.Parse(varDefinition.Attributes[XMLConfig.maxValueAttribute].Value);
            m_unit = varDefinition.Attributes[XMLConfig.unitAttribute].Value;
        }
    }
    public class StateVar : WorldVar
    {
        public StateVar(string name,double min,double max, string unit)
            : base(name,min,max,unit)
        {}
        public StateVar(XmlNode varDefinition) : base(varDefinition) { }
    }
    public class ActionVar : WorldVar
    {
        public ActionVar(string name, double min, double max, string unit)
            : base(name, min, max, unit)
        { }
        public ActionVar(XmlNode varDefinition) : base(varDefinition) { }
    }
    class WorldDefinition
    {
        List<StateVar> m_stateVars = new List<StateVar>();
        List<ActionVar> m_actionVars = new List<ActionVar>();
        List<string> m_constants = new List<string>();

        public void addStateVar(StateVar var) { m_stateVars.Add(var); }
        public void addActionVar(ActionVar var) { m_actionVars.Add(var); }
        public void addConstant(string name) { m_constants.Add(name); }

        public void getStateVarNameList(ref List<string> varNameList)
        {
            varNameList.Clear();
            foreach (StateVar var in m_stateVars) varNameList.Add(var.name);
        }
        public void getActionVarNameList(ref List<string> varNameList)
        {
            varNameList.Clear();
            foreach (ActionVar var in m_actionVars) varNameList.Add(var.name);
        }
        public void getConstantNameList(ref List<string> varNameList)
        {
            varNameList.Clear();
            foreach (string name in m_constants) varNameList.Add(name);
        }
    }
}
