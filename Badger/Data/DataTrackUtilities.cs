using System;
using System.Collections.Generic;
using Caliburn.Micro;
using Badger.ViewModels;
using Badger.Simion;

namespace Badger.Data
{
    class DataTrackUtilities
    {
        public static List<ReportParams> FromLoggedVariables(BindableCollection<LoggedVariableViewModel> loggedVariables)
        {
            List<ReportParams> trackParameterList = new List<ReportParams>();
            foreach(LoggedVariableViewModel variable in loggedVariables)
            {
                if (variable.bIsSelected)
                {
                    foreach (PlotType type in Enum.GetValues(typeof(PlotType)))
                    {
                        if ((type & variable.SelectedPlotType) != PlotType.Undefined)
                        {
                            ReportParams trackParameters = new ReportParams(variable.name
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
