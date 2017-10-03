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