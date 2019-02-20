

namespace Herd.Files
{
    public class LoadOptions
    {
        public enum ExpUnitSelection { All, OnlyFinished, OnlyUnfinished };

        
        public delegate void PerExperimentFunction(Experiment experiment) ;
        public delegate void PerExperimentalUnitFunction(ExperimentalUnit experimentalUnit) ;


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
