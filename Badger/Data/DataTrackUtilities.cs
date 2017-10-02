using System;
using System.Collections.Generic;
using Caliburn.Micro;
using Badger.ViewModels;
using Badger.Simion;

namespace Badger.Data
{
    class DataTrackUtilities
    {
        public static List<Report> FromLoggedVariables(BindableCollection<LoggedVariableViewModel> loggedVariables)
        {
            List<Report> trackParameterList = new List<Report>();
            foreach(LoggedVariableViewModel variable in loggedVariables)
            {
                if (variable.IsSelected)
                {
                    foreach (PlotType type in Enum.GetValues(typeof(PlotType)))
                    {
                        if ((type & variable.SelectedPlotType) != PlotType.Undefined)
                        {
                            Report trackParameters = new Report(variable.name
                                , type, variable.SelectedProcessFunc);
                            trackParameterList.Add(trackParameters);
                        }
                    }
                }
            }
            return trackParameterList;
        }
    }
}
