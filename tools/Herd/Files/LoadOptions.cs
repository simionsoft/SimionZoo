

namespace Herd.Files
{
    public class LoadOptions
    {
        public enum ExpUnitSelection { All, OnlyFinished, OnlyUnfinished };

        
        public delegate void PerExperimentFunction(LoggedExperiment experiment) ;
        public delegate void PerExperimentalUnitFunction(LoggedExperimentalUnit experimentalUnit) ;


        public PerExperimentFunction OnExpLoaded;
        public PerExperimentalUnitFunction OnExpUnitLoaded;
        
        public ExpUnitSelection Selection;
        public bool LoadVariablesInLog;

        public LoadOptions()
        {
            //default values
            OnExpLoaded = null;
            OnExpUnitLoaded = null;
            Selection = ExpUnitSelection.All;
            LoadVariablesInLog = false;
        }
    }
}
