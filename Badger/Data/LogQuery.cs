using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.Data
{
    public class LogQuery
    {
        public const string FunctionNone = "";
        public const string FunctionMax = "Max";
        public const string FunctionMin = "Min";
        public const string FunctionAvg = "Avg";

        public const string FromAll = "*";
        public const string FromSelection = "Selection";

        private string m_function = "";
        private string m_variable = "";
        private string m_from = "";
        private List<string> m_groupBy = new List<string>();

        public LogQuery(string function, string variable, string from, List<string> groupBy)
        {
            m_function = function;
            m_variable = variable;
            m_from = from;
            m_groupBy = groupBy;
        }

        public override string ToString()
        {
            string query = "";
            query = "SELECT ";
            if (m_function != "") query += m_function + "(" + m_variable + ")";
            else query += m_variable;
            query+= " FROM " + m_from;
            if (m_groupBy.Count != 0)
            {
                query += " GROUP BY ";
                for (int i= 0; i<m_groupBy.Count-1; i++) query += m_groupBy[i];
                if (m_groupBy.Count > 0) query += m_groupBy[m_groupBy.Count - 1];
            }
            return query;
        }
    }
}
