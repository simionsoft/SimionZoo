using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.Data
{
    public enum PlotType
    {
        [Browsable(false)]
        Undefined = 0,
        [Description("Last evaluation episode")]
        LastEvaluation = 1,
        [Description("All evaluation episodes")]
        AllEvaluationEpisodes = 2,
        [Description("Evaluation averages")]
        AverageOfEachEvaluationEpisode = 4,
        [Description("All training episodes")]
        AllTrainingEpisodes = 8,
        //[Description("Training averages")]
        //AverageOfEachTrainingEpisode = 16,
        //[Description("All episodes")]
        //AllEpisodes = 32,
    };
}
