using System.ComponentModel;


namespace Badger.Data
{
    public enum PlotType
    {
        [Browsable(false)]
        Undefined = 0,
        [Description("Last evaluation")]
        LastEvaluation = 1,
        [Description("All evaluations")]
        AllEvaluationEpisodes = 2,
        //[Description("Average of each evaluation episode")]
        //AverageOfEachEvaluationEpisode = 4,
        [Description("All training")]
        AllTrainingEpisodes = 8,
        //[Description("Average of each training episode")]
        //AverageOfEachTrainingEpisode = 16,
        [Description("Evaluation averages")]
        EvaluationAverages = 32
    };
}
