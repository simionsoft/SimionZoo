using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace Badger.Data
{
    public static class Serialiazer
    {
        /// <summary>
        /// Perform a deep Copy of the object.
        /// </summary>
        /// <typeparam name="T">The type of object being copied.</typeparam>
        /// <param name="source">The object instance to copy.</param>
        /// <returns>The copied object.</returns>
        public static T Clone<T>(T source)
        {
            // Don't serialize a null object, simply return the default for that object
            if (Object.ReferenceEquals(source, null))
            {
                return default(T);
            }

            IFormatter formatter = new BinaryFormatter();
            Stream stream = new MemoryStream();
            using (stream)
            {
                DataContractSerializer ser = new DataContractSerializer(typeof(T));

                ser.WriteObject(stream, source);
                stream.Seek(0, SeekOrigin.Begin);
                return (T)ser.ReadObject(stream);
            }
        }
        public static void WriteObject<T>(string filename, T src)
        {
            FileStream writer = new FileStream(filename, FileMode.Create);
            DataContractSerializer ser =
                new DataContractSerializer(typeof(T));
            ser.WriteObject(writer, src);
            writer.Close();
        }
        public static T ReadObject<T>(string filename)
        {
            FileStream reader = new FileStream(filename, FileMode.Open);
            DataContractSerializer ser =
                new DataContractSerializer(typeof(T));
            T result= (T) ser.ReadObject(reader);
            reader.Close();
            return result;
        }
    }
    public static class ExtensionMethods
    {
        public static int CountListType<T>(this IEnumerable<T> list, Type type)
        {
            return list.Where(x => x.GetType() == type).Count();
        }

        public static int MaxIndex<T>(this IEnumerable<T> sequence) where T : IComparable<T>
        {
            int maxIndex = -1;
            T maxValue = default(T); // Immediately overwritten anyway

            int index = 0;
            foreach (T value in sequence)
            {
                if (value.CompareTo(maxValue) > 0 || maxIndex == -1)
                {
                    maxIndex = index;
                    maxValue = value;
                }
                index++;
            }
            return maxIndex;
        }
    }
}
