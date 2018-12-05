

namespace Herd.Files
{
    public class LoadOptions
    {
        public delegate void UpdateFunction();

        public UpdateFunction OnExpUnitLoaded = null;
        public bool LoadFinishedExpUnits = true;
        public bool LoadUnfinishedExpUnits = true;
        public bool LoadVariablesInLog = true;
    }
}
