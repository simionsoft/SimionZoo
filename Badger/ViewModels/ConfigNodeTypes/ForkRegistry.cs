/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/


using System.Collections.Generic;

using Caliburn.Micro;


namespace Badger.ViewModels
{
    /// <summary>
    /// Class to register all the forks hanging from an experiment or a fork node 
    /// </summary>
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
        //        writer.WriteLine(leftSpace + "<" + XMLTags.forkTag + " "
        //        + XMLTags.nameAttribute + "=\"" + fork.name.TrimEnd(' ') + "\" " 
        //        + XMLTags.aliasAttribute + "=\"" + fork.alias + "\">");

        //        //save the values
        //        foreach(ForkValueViewModel value in fork.children)
        //        {
        //            writer.WriteLine()
        //        }

        //        fork.childForkList.saveForksToStream(writer, leftSpace + "  ");

        //        writer.WriteLine(leftSpace + "</" + XMLTags.forkTag + ">");
        //    }
        //}
    }
}
