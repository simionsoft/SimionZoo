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
                    foreach (ReportType type in Enum.GetValues(typeof(ReportType)))
                    {
                        if ((type & variable.SelectedPlotType) != ReportType.Undefined)
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
