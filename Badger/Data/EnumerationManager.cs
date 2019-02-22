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

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Badger.Data
{
    class EnumerationManager
    {
        public static Array GetValues(Type enumeration)
        {
            Array wArray = Enum.GetValues(enumeration);
            ArrayList wFinalArray = new ArrayList();
            foreach (Enum wValue in wArray)
            {
                FieldInfo fi = enumeration.GetField(wValue.ToString());
                if (null != fi)
                {
                    BrowsableAttribute[] wBrowsableAttributes = fi.GetCustomAttributes(typeof(BrowsableAttribute), true) as BrowsableAttribute[];
                    if (wBrowsableAttributes.Length > 0)
                    {
                        //  If the Browsable attribute is false
                        if (wBrowsableAttributes[0].Browsable == false)
                        {
                            // Do not add the enumeration to the list.
                            continue;
                        }
                    }

                    DescriptionAttribute[] wDescriptions = fi.GetCustomAttributes(typeof(DescriptionAttribute), true) as DescriptionAttribute[];
                    if (wDescriptions.Length > 0)
                    {
                        wFinalArray.Add(wDescriptions[0].Description);
                    }
                    else
                        wFinalArray.Add(wValue);
                }
            }

            return wFinalArray.ToArray();
        }
    }
}
