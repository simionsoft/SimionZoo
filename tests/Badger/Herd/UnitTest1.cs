using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Herd;

namespace HerdTest
{
    [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void Herd_AvailableMemoryNormalization()
        {
            HerdAgentInfo herdAgent = new HerdAgentInfo();
            herdAgent.AddProperty(PropNames.TotalMemory, "1024Mb");
            Assert.AreEqual("1Gb", herdAgent.Memory);

            herdAgent.AddProperty(PropNames.TotalMemory, "1024Kb");
            Assert.AreEqual("0Gb", herdAgent.Memory);

            herdAgent.AddProperty(PropNames.TotalMemory, "1024Gb");
            Assert.AreEqual("1024Gb", herdAgent.Memory);

            herdAgent.AddProperty(PropNames.TotalMemory, "1046Mb");
            Assert.AreEqual("1Gb", herdAgent.Memory);

            herdAgent.AddProperty(PropNames.TotalMemory, "512Mb");
            Assert.AreEqual("0.5Gb", herdAgent.Memory);

            herdAgent.AddProperty(PropNames.TotalMemory, Convert.ToString(1024 * 1024 * 1024));
            Assert.AreEqual("1Gb", herdAgent.Memory);

            herdAgent.AddProperty(PropNames.TotalMemory, Convert.ToString(1024 * 1024));
            Assert.AreEqual("0Gb", herdAgent.Memory);
        }

        [TestMethod]
        public void Herd_ProcessorLoadNormalization()
        {
            HerdAgentInfo herdAgent = new HerdAgentInfo();
            herdAgent.AddProperty(PropNames.ProcessorLoad, "12,344332");
            Assert.AreEqual("12.34%", herdAgent.ProcessorLoad);

            herdAgent.AddProperty(PropNames.ProcessorLoad, "12.3");
            Assert.AreEqual("12.3%", herdAgent.ProcessorLoad);

            herdAgent.AddProperty(PropNames.ProcessorLoad, "12.344332");
            Assert.AreEqual("12.34%", herdAgent.ProcessorLoad);

            herdAgent.AddProperty(PropNames.ProcessorLoad, "12");
            Assert.AreEqual("12%", herdAgent.ProcessorLoad);

        }
    }
}
