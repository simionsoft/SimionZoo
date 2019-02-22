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
using System.IO;
using System.Reflection;
using System.Runtime.Serialization;
using System.Xml;
using System.Xml.Serialization;

namespace Xml.Serialization
{
    class XmlCallbackSerializer : XmlSerializer
    {
        public XmlCallbackSerializer(Type type) : base(type)
        {
        }

        public XmlCallbackSerializer(XmlTypeMapping xmlTypeMapping) : base(xmlTypeMapping)
        {
        }

        public XmlCallbackSerializer(Type type, string defaultNamespace) : base(type, defaultNamespace)
        {
        }

        public XmlCallbackSerializer(Type type, Type[] extraTypes) : base(type, extraTypes)
        {
        }

        public XmlCallbackSerializer(Type type, XmlAttributeOverrides overrides) : base(type, overrides)
        {
        }

        public XmlCallbackSerializer(Type type, XmlRootAttribute root) : base(type, root)
        {
        }

        public XmlCallbackSerializer(Type type, XmlAttributeOverrides overrides, Type[] extraTypes,
            XmlRootAttribute root, string defaultNamespace) : base(type, overrides, extraTypes, root, defaultNamespace)
        {
        }

        public XmlCallbackSerializer(Type type, XmlAttributeOverrides overrides, Type[] extraTypes,
            XmlRootAttribute root, string defaultNamespace, string location)
            : base(type, overrides, extraTypes, root, defaultNamespace, location)
        {
        }

        public new object Deserialize(Stream stream)
        {
            var result = base.Deserialize(stream);

            CheckForDeserializationCallbacks(result);

            return result;
        }

        public new object Deserialize(TextReader textReader)
        {
            var result = base.Deserialize(textReader);

            CheckForDeserializationCallbacks(result);

            return result;
        }

        public new object Deserialize(XmlReader xmlReader)
        {
            var result = base.Deserialize(xmlReader);

            CheckForDeserializationCallbacks(result);

            return result;
        }

        public new object Deserialize(XmlSerializationReader reader)
        {
            var result = base.Deserialize(reader);

            CheckForDeserializationCallbacks(result);

            return result;
        }

        public new object Deserialize(XmlReader xmlReader, string encodingStyle)
        {
            var result = base.Deserialize(xmlReader, encodingStyle);

            CheckForDeserializationCallbacks(result);

            return result;
        }

        public new object Deserialize(XmlReader xmlReader, XmlDeserializationEvents events)
        {
            var result = base.Deserialize(xmlReader, events);

            CheckForDeserializationCallbacks(result);

            return result;
        }

        public new object Deserialize(XmlReader xmlReader, string encodingStyle, XmlDeserializationEvents events)
        {
            var result = base.Deserialize(xmlReader, encodingStyle, events);

            CheckForDeserializationCallbacks(result);

            return result;
        }

        private void CheckForDeserializationCallbacks(object deserializedObject)
        {
            if (deserializedObject is null)
            {
                return;
            }

            var deserializationCallback = deserializedObject as IDeserializationCallback;

            if (deserializationCallback != null)
            {
                deserializationCallback.OnDeserialization(this);
            }

            var properties = deserializedObject.GetType().GetProperties(BindingFlags.Public | BindingFlags.Instance);

            foreach (var propertyInfo in properties)
            {
                if (Attribute.IsDefined(propertyInfo, typeof(XmlIgnoreAttribute)))
                {
                    continue;
                }

                if (propertyInfo.PropertyType.GetInterface(typeof(IEnumerable<>).FullName) != null)
                {
                    var collection = propertyInfo.GetValue(deserializedObject) as IEnumerable;

                    if (collection != null)
                    {
                        foreach (var item in collection)
                        {
                            CheckForDeserializationCallbacks(item);
                        }
                    }
                }
                else
                {
                    CheckForDeserializationCallbacks(propertyInfo.GetValue(deserializedObject));
                }
            }
        }
    }
}