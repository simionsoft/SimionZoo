
using System.Collections.Generic;
using System.IO;
using Caliburn.Micro;
using Badger.Simion;

namespace Badger.ViewModels
{
    //this class is used to register all the forks hanging from an experiment or a fork node
    public class ForkRegistry : PropertyChangedBase
    {
        private List<ForkedNodeViewModel> m_forks = new List<ForkedNodeViewModel>();

        public List<ForkedNodeViewModel> Forks { get { return m_forks; } }

        public void Add(ForkedNodeViewModel fork)
        {
            m_forks.Add(fork);
        }

        public void Remove(ForkedNodeViewModel fork)
        {
            m_forks.Remove(fork);
        }

        public bool Validate(string name)
        {
            int count = 0;
            foreach (ForkedNodeViewModel fork in m_forks)
            {
                if (fork.alias == name)
                    ++count;
            }
            if (count == 1)
                return true;
            return false;
        }

        //this method saves the forks hierarchically
        //public void saveForksToStream(StreamWriter writer,string leftSpace)
        //{
        //    foreach(ForkedNodeViewModel fork in m_forks)
        //    {
        //        writer.WriteLine(leftSpace + "<" + XMLConfig.forkTag + " "
        //        + XMLConfig.nameAttribute + "=\"" + fork.name.TrimEnd(' ') + "\" " 
        //        + XMLConfig.aliasAttribute + "=\"" + fork.alias + "\">");

        //        //save the values
        //        foreach(ForkValueViewModel value in fork.children)
        //        {
        //            writer.WriteLine()
        //        }

        //        fork.childForkList.saveForksToStream(writer, leftSpace + "  ");

        //        writer.WriteLine(leftSpace + "</" + XMLConfig.forkTag + ">");
        //    }
        //}
    }
}
