using GongSolutions.Wpf.DragDrop;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;
using Badger.Data.NeuralNetwork.Links;

namespace Badger.Data.NeuralNetwork.Parameters
{
    public class LinkConnectionListParameter : ParameterBase<ObservableCollection<LinkConnection>>, IDeserializationCallback
    {
        protected LinkConnectionListParameter()
        {
            RawValue.CollectionChanged += RawValue_CollectionChanged;
            _collectionsUpdating = true;
        }

        public LinkConnectionListParameter(string name, LinkBase link) : base(name, new ObservableCollection<LinkConnection>(), link)
        {
            Value.CollectionChanged += Value_CollectionChanged;
            RawValue.CollectionChanged += RawValue_CollectionChanged;
        }
        public LinkConnectionListParameter(string name, ObservableCollection<LinkConnection> value, LinkBase link) : base(name, value, link)
        {
            Value.CollectionChanged += Value_CollectionChanged;
            RawValue.CollectionChanged += RawValue_CollectionChanged;
        }

        private ObservableCollection<LinkConnection> _value = new ObservableCollection<LinkConnection>();
        public new ObservableCollection<LinkConnection> Value
        {
            get
            {
                return _value;
            }
            set
            {
                if (_value != null)
                    _value.CollectionChanged -= Value_CollectionChanged;
                _value = value;
                _value.CollectionChanged += Value_CollectionChanged;
            }
        }

        [XmlIgnore]
        private ObservableCollection<Links.LinkBase> _rawValue = new ObservableCollection<LinkBase>();
        public ObservableCollection<Links.LinkBase> RawValue
        {
            get
            {
                return _rawValue;
            }
            set
            {
                if (_rawValue != null)
                    _rawValue.CollectionChanged -= RawValue_CollectionChanged;
                _rawValue = value;
                _rawValue.CollectionChanged += RawValue_CollectionChanged;
            }
        }
        [XmlIgnore]
        private bool _collectionsUpdating = false;
        private void Value_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (_collectionsUpdating)
                return;
            _collectionsUpdating = true;

            if (e.OldItems != null)
            {
                //remove these items
                foreach (LinkConnection conItem in e.OldItems)
                {
                    //find corresponding link connection
                    foreach (var item in RawValue)
                    {
                        if (conItem.TargetID.Equals(item.ID))
                        {
                            //remove it
                            RawValue.Remove(item);
                            break;
                        }
                    }
                }
            }
            if (e.NewItems != null)
            {
                //add these items
                foreach (LinkConnection item in e.NewItems)
                {
                    RawValue.Add(item.Target);
                }
            }

            _collectionsUpdating = false;
        }
        private void RawValue_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (_collectionsUpdating)
                return;
            _collectionsUpdating = true;

            if (e.OldItems != null)
            {
                //remove these items
                foreach (LinkBase item in e.OldItems)
                {
                    //find corresponding link connection
                    foreach (var conItem in Value)
                    {
                        if (conItem.TargetID.Equals(item.ID))
                        {
                            //remove it
                            Value.Remove(conItem);
                            break;
                        }
                    }
                }
            }
            if (e.NewItems != null)
            {
                //add these items
                foreach (LinkBase item in e.NewItems)
                {
                    Value.Add(new LinkConnection(item));
                }
            }

            _collectionsUpdating = false;
        }

        public void OnDeserialization(object sender)
        {
            foreach (var item in Value)
            {
                bool itemFound = false;
                //find target object using the ID
                foreach (var chain in Parent.ParentChain.NetworkArchitecture.Chains)
                {
                    foreach (var link in chain.ChainLinks)
                    {
                        if (link.ID.Equals(item.TargetID))
                        {
                            item.Target = link;
                            itemFound = true;
                            break;
                        }
                    }
                    if (itemFound)
                        break;
                }
            }
            _collectionsUpdating = false;
        }
    }

    [Serializable]
    public class LinkConnection
    {
        private LinkConnection() { }
        public LinkConnection(Links.LinkBase target)
        {
            Target = target;
            TargetID = target.ID;
        }

        [XmlIgnore]
        public Links.LinkBase Target { get; set; }

        [XmlAttribute]
        public string TargetID { get; set; }
    }

}
