using AppXML.Models;
using AppXML.ViewModels;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;
using System.Runtime.InteropServices;
using System.Net.Sockets;
using System.Net;
using Herd;
using System.Threading;
using System.Runtime.CompilerServices;


namespace AppXML.Data
{
    public static class Utility
    {
        [DllImport(@"./RLSimionInterfaceDLL.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int getIOFiles(string xmlFilename, StringBuilder pBuffer, int bufferSize);
        
        private static Dictionary<IPEndPoint, int> myList ;
        //used to avoid readings of worl-denitions xml
        private static Dictionary<string, List<string>> xmlDic = new Dictionary<string, List<string>>();
        public static void DiscoveryCallback(IAsyncResult ar)
        {

            UdpClient u = (UdpClient)((UdpState)(ar.AsyncState)).client;
            IPEndPoint e = (IPEndPoint)((UdpState)(ar.AsyncState)).ip;

            try
            {
                
                Byte[] receiveBytes = u.EndReceive(ar, ref e);
                string receiveString = Encoding.ASCII.GetString(receiveBytes);
                if (!myList.ContainsKey(e))
                {
                    myList.Add(e, Int32.Parse(XElement.Parse(receiveString).Value));
                }
                u.BeginReceive(new AsyncCallback(DiscoveryCallback), ar.AsyncState);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.StackTrace);
            }
            
           
                

            
           

           
           

           
        }
        public static Dictionary<IPEndPoint,int> getSlaves(out int cores)
        {
            if(myList==null)
                myList = new Dictionary<IPEndPoint,int>();
            else
                myList.Clear();
            cores = 0;
            
            UdpClient m_discoverySocket;
            m_discoverySocket = new UdpClient();
            m_discoverySocket.EnableBroadcast = true;
            var RequestData = Encoding.ASCII.GetBytes(CJobDispatcher.m_discoveryMessage);
            
            //this sleep only made sense when we were debugging testherdagent and testshepherd at the same time
            //System.Threading.Thread.Sleep(1000); //so that the shepherd waits for the herd agent to be ready

            m_discoverySocket.Send(RequestData, RequestData.Length, new IPEndPoint(IPAddress.Broadcast, CJobDispatcher.m_discoveryPortHerd));

            UdpState u = new UdpState();
            IPEndPoint xxx = new IPEndPoint(0, CJobDispatcher.m_discoveryPortHerd);
            u.ip=xxx;
            u.client = m_discoverySocket; 
            m_discoverySocket.BeginReceive(DiscoveryCallback, u);
            
            //We wait 2 secs for herd agents to reply
            Thread.Sleep(2000);

            cores = myList.Values.ToList().Sum(od => od);
            if (myList != null && myList.Count > 1)
            {
                return (from entry in myList orderby entry.Value ascending select entry).ToDictionary(x => x.Key, x => x.Value);
            }
            return myList;
                   
                    
            
        }
        //public static void method(List<string> filenames)
        //{
           
        //    Shepherd shepherd;
        //    var TcpSocket = new TcpListener(IPAddress.Any, CJobDispatcher.m_comPortShepherd);
        //    TcpSocket.Start();
        //    using (TcpClient comSocket = TcpSocket.AcceptTcpClient())
        //    {
        //        using (NetworkStream netStream = comSocket.GetStream())
        //        {
        //            shepherd = new Shepherd(comSocket,netStream,"");
        //            //we can just block the thread by waiting until we receive something. Don't think we need to sleep
        //            //Thread.Sleep(20000);
                   

        //            //aborter.Send(RequestData2, RequestData2.Length, new IPEndPoint(IPAddress.Broadcast,8888));
        //            shepherd.ReceiveJobResult();
        //        }
        //    }
        //    TcpSocket.Stop();
        //    //var ServerResponseData = Client.Receive(ref ServerEp);
        //    //var ServerResponse = Encoding.ASCII.GetString(ServerResponseData);
        //    //Console.WriteLine("Received {0} from {1}", ServerResponse, ServerEp.Address.ToString());

        //    //m_discoverySocket.Close();
        //}
        [MethodImpl(MethodImplOptions.Synchronized)]
        public static void getInputsAndOutputs(string path, ref CJob job)
        {
            StringBuilder myResult = new StringBuilder(204800);
            object o = myResult;
            Monitor.Enter(o);
            
           
            //StringBuilder myResult = new StringBuilder(204800);
            int error = getIOFiles(path, myResult, 204800);
            if (error == -1)
            {
                return;
            }
            else
            {
                XDocument doc = XDocument.Parse(myResult.ToString());
                XElement[] inputFiles = doc
                .Descendants()
                .Where(e => e.Name == "Input")
                .ToArray();
                XElement[] outputFiles = doc
                .Descendants()
                .Where(e => e.Name == "Output")
                .ToArray();
                foreach (XElement e in inputFiles)
                {
                    if(!job.inputFiles.Contains(e.Value))
                        job.inputFiles.Add(e.Value);
                }
                foreach (XElement e in outputFiles)
                {
                    if(!job.outputFiles.Contains(e.Value))
                        job.outputFiles.Add(e.Value);
                }
            }
            Monitor.Exit(o);
        }

        public static List<string> getInputs(string path)
        {
             StringBuilder myResult = new StringBuilder(204800);
            int error = getIOFiles(path, myResult, 204800);
            if (error == -1)
            {
                return null;
            }
            else
            {
                XDocument doc = XDocument.Parse(myResult.ToString());
                XElement[] inputFiles = doc
                .Descendants()
                .Where(e => e.Name == "Input")
                .ToArray();
                List<string> returnList = new List<string>();
                foreach(XElement e in inputFiles)
                {
                    returnList.Add(e.Value);
                }
                return returnList;
            }
                   
        }

        public static Dictionary<string, List<string>> findIOProblems(List<string> pahts)
        {
           
            Dictionary<string, List<string>> result = new Dictionary<string, List<string>>();
            Dictionary<string, string> myDic = new Dictionary<string, string>();
            foreach(string path in pahts)
            {
                StringBuilder myResult = new StringBuilder(204800);
                int error = getIOFiles(path, myResult, 204800);
                if(error==-1)
                {
                    //mostrar error
                }
                else
                {
                    XDocument doc = XDocument.Parse(myResult.ToString());
                    /*XElement[] inputFiles = doc
                    .Descendants()
                    .Where(e => e.Name=="Input")
                    .ToArray();*/
                    XElement[] outputFiles = doc
                    .Descendants()
                    .Where(e => e.Name == "Output")
                    .ToArray();
                   /* foreach(XElement e in inputFiles)
                    {
                        Console.WriteLine(e.Value);
                    }*/
                    foreach (XElement e in outputFiles)
                    {
                        if(!myDic.ContainsKey(e.Value))
                            myDic.Add(e.Value, path);
                        else if(!myDic[e.Value].Equals(path))
                        {
                            if(result.ContainsKey(e.Value))
                            {
                                List<string> tmp =result[e.Value];
                                if(!tmp.Contains(path))
                                {
                                    tmp.Add(path);
                                    result[e.Value] = tmp;
                                }
                            }
                            else
                            result.Add(e.Value, new List<string> { myDic[e.Value], path });
                        }
                   
                    }
                }
                myResult.Clear();
            }      
            return result;
        }

        public static bool checkName(string name, TreeNode father)
        {
            try
            {
                XmlConvert.VerifyName(name);
                if(father.ChildNodes!=null)
                {
                    foreach (TreeNode son in father.ChildNodes)
                    {
                        if (son.Text.ToUpper().Equals(name.ToUpper()))
                        {
                            return false;
                        }
                    }
                }
                return true;
            }
            catch
            {
               return false;
            }
        }
        public static void fillTheClass(ClassViewModel cvm, XmlNode dataNode)
        {

            //here we have to set the new data such as: selected choice element, default values for every *.value, etc.
            cvm.setAsNull();
            bool dataSet = false;
            bool multiStarted = false;
            string currentMulti = "";
            int multiIndex = 0;
            bool res = false;
            if (cvm.ResumeClass != null)
            {
                cvm = cvm.ResumeClass;
                res = true;
            }
            foreach (XmlNode data in dataNode)
            {
                dataSet = false;
                string dataTag = data.Name;
                string branchTag = null;
                string[] splitedTag = null;
                //once we have the tag we have to find out if the branch has this tag somewhere
                if (!dataSet && cvm.Choice != null)
                {
                    branchTag = cvm.Choice.Tag;
                    splitedTag = branchTag.Split('/');
                    XmlNode tmp = data;
                    foreach (string tag in splitedTag)
                    {
                        if (tag == dataTag)
                        {
                            if (tag == splitedTag[splitedTag.Length - 1])
                            {
                                if (tmp.HasChildNodes)
                                {
                                    XmlNode choiceElementData = tmp.FirstChild;
                                    string name = choiceElementData.Name;
                                    cvm.Choice.setSelectedItem(name);
                                    fillTheClass(cvm.Choice.Class, choiceElementData);
                                    if (res)
                                        cvm.validate(false);
                                    dataSet = true;

                                }
                            }
                            else
                            {
                                if (data.HasChildNodes)
                                {
                                    tmp = data.FirstChild;
                                    dataTag = tmp.Name;
                                }

                            }
                        }
                    }
                    if (dataSet)
                        break;
                }
                foreach (ValidableAndNodeViewModel view in cvm.AllItems)
                {
                    if (!dataSet)
                    {
                        MultiXmlNodeRefViewModel itemMultiXml = view as MultiXmlNodeRefViewModel;
                        if (!dataSet && itemMultiXml != null)
                        {

                            string itemTag = itemMultiXml.Tag;
                            splitedTag = itemTag.Split('/');
                            XmlNode tmp = data;
                            foreach (string tag in splitedTag)
                            {
                                if (itemTag == dataTag)
                                {
                                    if (tag == splitedTag[splitedTag.Length - 1])
                                    {

                                        if (multiStarted == false || currentMulti != tag)
                                        {
                                            currentMulti = tag;
                                            multiIndex = 0;
                                            multiStarted = true;
                                            itemMultiXml.Header.SelectedOption = tmp.InnerText;
                                            dataSet = true;
                                            if (itemMultiXml.AddedXml != null)
                                                itemMultiXml.AddedXml.Clear();
                                            break;
                                        }
                                        else
                                        {
                                            if (itemMultiXml.AddedXml != null)
                                            {
                                                int index = itemMultiXml.AddedXml.Count;
                                                if (index == 0 || multiIndex >= index)
                                                    itemMultiXml.AddNew();
                                            }
                                            else
                                                itemMultiXml.AddNew();
                                            itemMultiXml.AddedXml[multiIndex].SelectedOption = tmp.InnerText;
                                            multiIndex++;
                                            dataSet = true;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        if (data.HasChildNodes)
                                        {
                                            tmp = data.FirstChild;
                                            dataTag = tmp.Name;

                                        }

                                    }
                                }
                            }
                            if (dataSet)
                                break;
                        }
                        XMLNodeRefViewModel itemXml = view as XMLNodeRefViewModel;
                        if (!dataSet && itemXml != null)
                        {
                            string itemTag = itemXml.Tag;
                            splitedTag = itemTag.Split('/');
                            XmlNode tmp = data;
                            foreach (string tag in splitedTag)
                            {
                                if (itemTag == dataTag)
                                {
                                    if (tag == splitedTag[splitedTag.Length - 1])
                                    {

                                        itemXml.SelectedOption = tmp.InnerText;
                                        dataSet = true;
                                        break;

                                    }
                                    else
                                    {
                                        if (data.HasChildNodes)
                                        {
                                            tmp = data.FirstChild;
                                            dataTag = tmp.Name;
                                        }

                                    }
                                }
                            }
                            if (dataSet)
                                break;
                        }
                        MultiValuedViewModel itemMulti = view as MultiValuedViewModel;
                        if (!dataSet && itemMulti != null)
                        {

                            string itemTag = itemMulti.Tag;
                            splitedTag = itemTag.Split('/');
                            XmlNode tmp = data;
                            foreach (string tag in splitedTag)
                            {
                                if (itemTag == dataTag)
                                {
                                    if (tag == splitedTag[splitedTag.Length - 1])
                                    {

                                        if (multiStarted == false || currentMulti != tag)
                                        {
                                            currentMulti = tag;
                                            multiIndex = 0;
                                            multiStarted = true;
                                            if (itemMulti.HeaderClass == null)
                                            {
                                                itemMulti.Header.Value = tmp.InnerText;
                                                if (itemMulti.Aded != null)
                                                    itemMulti.Aded.Clear();
                                            }
                                            else
                                            {
                                                fillTheClass(itemMulti.HeaderClass, tmp);
                                                if (itemMulti.AdedClasses != null)
                                                    itemMulti.AdedClasses.Clear();
                                            }
                                            dataSet = true;
                                            break;
                                        }
                                        else
                                        {
                                            if (itemMulti.HeaderClass == null)
                                            {
                                                //int index = itemMulti.Aded.Count;
                                                //if (index == -0 || multiIndex >= index)
                                                itemMulti.AddNew();
                                                itemMulti.Aded[multiIndex].Value = tmp.InnerText;
                                            }

                                            else
                                            {
                                                //int index = itemMulti.AdedClasses.Count;
                                                //if (index == 0 || multiIndex >= index)
                                                itemMulti.Add();
                                                fillTheClass(itemMulti.AdedClasses[multiIndex], tmp);

                                            }
                                            dataSet = true;
                                            multiIndex++;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        if (data.HasChildNodes)
                                        {
                                            tmp = data.FirstChild;
                                            dataTag = tmp.Name;
                                        }

                                    }
                                }
                            }
                            if (dataSet)
                                break;
                        }
                        IntegerViewModel itemInteger = view as IntegerViewModel;
                        if (!dataSet && itemInteger != null)
                        {

                            string itemTag = itemInteger.Tag;
                            splitedTag = itemTag.Split('/');
                            XmlNode tmp = data;
                            foreach (string tag in splitedTag)
                            {
                                if (itemTag == dataTag)
                                {
                                    if (tag == splitedTag[splitedTag.Length - 1])
                                    {

                                        itemInteger.Value = tmp.InnerText;
                                        dataSet = true;
                                        break;

                                    }
                                    else
                                    {
                                        if (data.HasChildNodes)
                                        {
                                            tmp = data.FirstChild;
                                            dataTag = tmp.Name;
                                        }

                                    }
                                }
                            }
                            if (dataSet)
                                break;
                        }
                        BranchViewModel itemBranch = view as BranchViewModel;
                        if (!dataSet && itemBranch != null)
                        {
                            string itemTag = itemBranch.Tag;
                            splitedTag = itemTag.Split('/');
                            XmlNode tmp = data;
                            foreach (string tag in splitedTag)
                            {
                                if (itemTag == dataTag)
                                {
                                    if (tag == splitedTag[splitedTag.Length - 1])
                                    {
                                        if (tmp.HasChildNodes)
                                        {
                                            itemBranch.IsNull = false;
                                            //item.Value = tmp.InnerText;
                                            if (itemBranch.Class.ItemName == null)
                                                fillTheClass(itemBranch.Class, tmp);
                                            else
                                            {
                                                fillTheClass(itemBranch.Class.ResumeClass, tmp);
                                                itemBranch.Class.validate(false);
                                            }
                                            dataSet = true;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        if (data.HasChildNodes)
                                        {
                                            tmp = data.FirstChild;
                                            dataTag = tmp.Name;
                                        }

                                    }
                                }
                            }
                        }
                        if (dataSet)
                            break;
                    }
                }

            }
            if (res)
                cvm.validate(false);
        }
        public static string findDifferences(XmlDocument childDocument,XmlDocument fatherDocument)
        {

            XDocument xdoc = XDocument.Parse(fatherDocument.OuterXml);
            //Dictionary<string,string> x = Utility.GetLeaves(xdoc);
            Dictionary<string, List<string>> father = Utility.getNodesWithMulti(xdoc);
            XDocument xdoc2 = XDocument.Parse(childDocument.OuterXml);
            //Dictionary<string, string> x2 = Utility.GetLeaves(xdoc2);
            Dictionary<string, List<string>> child = Utility.getNodesWithMulti(xdoc2);
            string lastMulti = " ";
            List<string> message = new List<string>();
            List<string> missingMessage = new List<string>();
            foreach (string key in child.Keys)
            {
                if (!father.ContainsKey(key))
                    missingMessage.Add(key + " has been added to this child");
                else
                {
                    List<string> list1 = child[key];
                    List<string> list2 = father[key];
                    if (list1.Count == list2.Count)
                    {
                        for (int i = 0; i < list2.Count; i++)
                        {
                            if (!list1[i].Equals(list2[i]))
                            {
                                message.Add(key);
                                int index = message.IndexOf(key);
                                if (index > 0)
                                {
                                    if (key.StartsWith(message[index - 1]))
                                    {
                                        message.RemoveAt(index - 1);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if (!key.StartsWith(lastMulti))
                        {
                            int dif = list1.Count - list2.Count;
                            if (dif > 0)
                            {
                                missingMessage.Add("This child has " + dif + " more " + key);
                            }
                            else
                            {
                                missingMessage.Add("This child has " + dif + " less " + key);
                                dif = -dif;
                            }
                            for (int i = 0; i < list1.Count - dif; i++)
                            {
                                if (list1[i].Equals(list2[i]))
                                    missingMessage.Add("The element number " + i + " has the same parameters");
                                else
                                    missingMessage.Add("The element number " + i + " has not the same parameters");
                            }
                            lastMulti = key;
                        }
                    }
                    father.Remove(key);
                }
            }
            foreach (string key in father.Keys)
            {
                missingMessage.Add("The child does not have " + key);
            }
            string result = "";
            foreach (string key in message)
                result += key + " is different\n";
            foreach (string sms in missingMessage)
                result += sms + "\n";
            if (result == "")
                return null;
            return result;

        }
        public static List<string> removeAndGetLeafs(string folder)
        {
            List<string> result = new List<string>();
            bool isLeaf = Directory.GetDirectories(folder).Length == 0;
            if(Directory.Exists(folder))
            {
                string root = null;
                string[] folders = Directory.GetDirectories(folder);
                if (folders == null || folders.Length == 0)
                {
                    string[] files = Directory.GetFiles(folder);
                    foreach (string file in files)
                    {
                        if (!file.EndsWith(".node"))
                            File.Delete(file);
                        else if (isLeaf)
                            result.Add(file);
                    }
                    return result;
                }
                else
                {
                    foreach(string item in folders)
                    {
                        if (item.EndsWith("root"))
                            root = removeAndGetLeafs(item)[0];
                        else
                            result.AddRange(removeAndGetLeafs(item));
                    }
                    
                }
                if (result.Count == 0 && root != null)
                    result.Add(root);
            }
            return result;
        }
        public static Dictionary<string, string> GetLeaves(XDocument doc)
        {
            var dict = doc
                .Descendants()
                //.Where(e => !e.HasElements)
                .ToDictionary(e => GetPath(e), e=> e.Value);
            return dict;
        }
        public static Dictionary<string, List<string>> getNodesWithMulti(XDocument doc)
        {
            var list = doc
                .Descendants()
                //.Where(e => e.HasElements)
                .ToArray();
            Dictionary<string, List<string>> result = new Dictionary<string, List<string>>();
             foreach(XElement e in list)
             {
                 string key = GetPath(e);
                 if (result.ContainsKey(key))
                 {
                     List<string> tmp = result[key];
                     tmp.Add(e.Value);
                     result[key]=tmp;
                 }
                 else
                 {
                     List<string> tmp = new List<string>();
                     tmp.Add(e.Value);
                     result.Add(key,tmp);
                 }
                     
             }
             return result;
        }
        private static string GetPath(XElement element)
        {
            var nodes = new List<string>();
            var node = element;
            while (node != null)
            {
                nodes.Add(node.Name.ToString());
                node = node.Parent;
            }

            return string.Join("/", Enumerable.Reverse(nodes));
        }


        public static string GetRelativePathTo(string absPath, string relTo)
        {
            string[] absDirs = absPath.Split('\\');
            string[] relDirs = relTo.Split('\\');
            // Get the shortest of the two paths 
            int len = absDirs.Length < relDirs.Length ? absDirs.Length : relDirs.Length;
            // Use to determine where in the loop we exited 
            int lastCommonRoot = -1; int index;
            // Find common root 
            for (index = 0; index < len; index++)
            {
                if (absDirs[index] == relDirs[index])
                    lastCommonRoot = index;
                else break;
            }
            // If we didn't find a common prefix then throw 
            if (lastCommonRoot == -1)
            {
                return relTo;
            }
            // Build up the relative path 
            StringBuilder relativePath = new StringBuilder();
            // Add on the .. 
            for (index = lastCommonRoot + 1; index < absDirs.Length; index++)
            {
                if (absDirs[index].Length > 0) relativePath.Append("..\\");
            }
            // Add on the folders 
            if(absDirs.Contains(relDirs[lastCommonRoot])&&(len-1==lastCommonRoot))
            {
                relativePath.Append("..\\");
            }
            for (index = lastCommonRoot + 1; index < relDirs.Length - 1; index++)
            {
                relativePath.Append(relDirs[index] + "\\");
            }
            relativePath.Append(relDirs[relDirs.Length - 1]);
            relativePath.Replace('\\','/');
            return relativePath.ToString();
        }
        public static Boolean validate(string value, validTypes type)
        {
            Boolean result = false;
            if (value == "0")
                return true;
            Regex regex;
            switch (type)
            {
                case validTypes.FilePathValue:
                    if(File.Exists(value))
                        return true;
                    else
                        return false;
                case validTypes.DirPathValue:
                    if (Directory.Exists(value))
                        return true;
                    else
                        return false;
                case validTypes.DecimalValue:
                    regex = new Regex(@"^(-|)((0\.\d+)|[1-9]\d*(\.\d+))$");
                    if (regex.IsMatch(value))
                    {
                        return true;
                    }
                    return validate(value, validTypes.IntergerValue);
                case validTypes.IntergerValue:
                    regex = new Regex(@"^(-|)[1-9][0-9]*$");
                    return regex.IsMatch(value);                    
            }
            return result;
        }
        public static List<string> getEnumItems(XmlNode father)
        {
            List<string> result = null;
            if (father != null && father.HasChildNodes)
            {
                result = new List<string>();
                foreach (XmlNode child in father.ChildNodes)
                {
                    result.Add(child.InnerText);
                }
            }
            return result;
        }
        public static Dictionary<string,XmlNode> getDefinitions(string filePath)
        {

            Dictionary<string, XmlNode> result = new Dictionary<string, XmlNode>();
            XmlDocument doc = new XmlDocument();
            doc.Load(filePath);
            if (doc.HasChildNodes)
            {
                XmlNode root = doc.ChildNodes[1];
                if (root.HasChildNodes)
                {

                    foreach (XmlNode node in root.ChildNodes)
                    {
                        if (node.Name.Equals("CLASS"))
                        {
                            string key = node.Attributes["Name"].Value;
                            result.Add(key, node);
                        }
                        else if(node.Name.Equals("ENUMERATION"))
                        {
                            //foreach(XmlNode child in node.ChildNodes)
                            {
                                string key = node.Attributes["Name"].Value;
                                result.Add(key, node);
                                //result.Add(child.Attributes["Name"].Value, child);
                            }
                        }

                    }

                }
            }
            return result;
        }
        public static CNode getRootNode(string filePath)
        {
            XmlDocument xmldoc = new XmlDocument();
            xmldoc.Load(filePath);
            XmlNode element = xmldoc.DocumentElement;
            CNode rootnode = CNode.getInstance(element);
            foreach (XmlNode node in element.ChildNodes)
            {
                CNode cn = CNode.getInstance(node);
                bool isOptional = false;
                if (node.Attributes["Optional"] != null)
                    isOptional = Convert.ToBoolean(node.Attributes["Optional"].Value);
                string comment = null;
                if (node.Attributes["Comment"] != null)
                    comment = node.Attributes["Comment"].Value;
                string tag = null;
                if (node.Attributes["XMLTag"]!=null)
                    tag = node.Attributes["XMLTag"].Value;
                if(cn is CBranch)
                    rootnode.AddBranch(new ViewModels.BranchViewModel(node.Attributes["Name"].Value, node.Attributes["Class"].Value,comment,isOptional,(rootnode as CApp).document,tag));
            }
            return rootnode;
        }
        public static XmlNode resolveTag(string _tag, XmlDocument _doc)
        {
            string[] tags = _tag.Split('/');
            if(tags.Count()>0)
            {
                XmlNode result = _doc.CreateElement(tags[0]);
                if(tags.Count()>1)
                {
                    XmlNode father = result;
                    for(int i=1;i<tags.Count();i++)
                    {
                        XmlNode nodo = _doc.CreateElement(tags[i]);
                        father.AppendChild(nodo);
                        father = nodo;
                    }

                }
                return result;
            }

            return null;
           
            
        }
        public static XmlNode getLastChild(XmlNode result)
        {
            if (result.HasChildNodes)
                return getLastChild(result.ChildNodes[0]);
            else
                return result;
        }
        public static List<string> getComboFromXML(string file, string action)
        {
            string xmlFile = null;
            if (CNode.XML.ContainsKey(file))
                xmlFile = CNode.XML[file];
            else
                return new List<string>();
            string key = xmlFile + action;
            if(xmlDic.ContainsKey(key))
                return xmlDic[xmlFile+action];
            else
            {
                List<string> result = new List<string>();
                XmlDocument doc = new XmlDocument();
                doc.Load(xmlFile);
                foreach(XmlNode child in doc.ChildNodes)
                {
                    if(child.Name =="World-Definition")
                    {
                        foreach(XmlNode type in child.ChildNodes)
                        {
                            if(type.Name == action)
                            {
                                foreach(XmlNode variable in type.ChildNodes)
                                {
                                    result.Add(variable.ChildNodes[0].InnerText);
                                }
                            }
                        }
                    }
                }
                
                xmlDic.Add(key,result);
                return result;
            }
        }
        public static void cleanAll()
        {
            xmlDic.Clear();
        }

        public static string getFileName(string Default)
        {
            if (Default == null)
                return null;
            char[] separators = {'/','\\'};
            string[] tmp = Default.Split(separators);
            return tmp[tmp.Length - 1];
        }
    }
}
