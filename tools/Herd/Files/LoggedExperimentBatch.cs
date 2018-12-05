using System.Collections.Generic;
using System.Xml;

namespace Herd.Files
{
    public class LoggedExperimentBatch
    {
        public List<LoggedExperiment> Experiments = new List<LoggedExperiment>();

        public LoggedExperimentBatch(string batchFilename, LoadOptions loadOptions)
        {
            XmlDocument batchDoc = new XmlDocument();
            batchDoc.Load(batchFilename);
            XmlElement fileRoot = batchDoc.DocumentElement;

            if (fileRoot != null && fileRoot.Name == XMLTags.ExperimentBatchNodeTag)
            {
                foreach (XmlNode experiment in fileRoot.ChildNodes)
                {
                    if (experiment.Name == XMLTags.ExperimentNodeTag)
                         Experiments.Add(new LoggedExperiment(experiment, Utils.GetDirectory(batchFilename), loadOptions));
                }
            }
        }
    }
}
