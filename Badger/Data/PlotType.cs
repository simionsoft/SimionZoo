using System;
using System.ComponentModel;
using System.Runtime.Serialization;

namespace Badger.Data
{
    [DataContract][Flags]
    public enum ReportType
    {
        [Browsable(false),EnumMember]
        Undefined = 0,
        [Description("Last evaluation"),EnumMember]
        LastEvaluation = 1,
        [Description("All evaluations"),EnumMember]
        AllEvaluationEpisodes = 2,
        [Description("All training"),EnumMember]
        AllTrainingEpisodes = 4,
        [Description("Evaluation averages"),EnumMember]
        EvaluationAverages = 32
    };
}
