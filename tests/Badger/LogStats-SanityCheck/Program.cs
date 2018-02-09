using System;
using System.Collections.Generic;
using System.Linq;
using Badger.Simion;

namespace LogStats_SanityCheck
{
    class EpisodeStats
    {
        public double r { get; } = 0;
        public double E_p { get; } = 0;
        double min_r = double.MaxValue, max_r= double.MinValue;
        double min_E_p = double.MaxValue, max_E_p= double.MinValue;
        public EpisodeStats(EpisodesData episode)
        {
            foreach (StepData step in episode.steps)
            {
                double current_E_p = Math.Abs(step.data[4]);
                E_p += current_E_p;
                if (current_E_p > max_E_p) max_E_p = current_E_p;
                if (current_E_p < min_E_p) min_E_p = current_E_p;
                double current_r = step.data[19];
                r += current_r;
                if (current_r > max_r) max_r = current_r;
                if (current_r < min_r) min_r = current_r;
                double ratio = (-current_r + 1) / current_E_p;
                double shouldBe = (1 - (current_E_p / 1000));
                if (Math.Abs(current_r-shouldBe)>0.001)
                    Console.WriteLine("Reward discrepancy: " + (current_r-shouldBe));
            }
            E_p /= episode.steps.Count;
            r /= episode.steps.Count;
        }
        public void Print()
        {
            Console.WriteLine("E_p= " + E_p + " [" + min_E_p + "," + max_E_p + "] -> r=" + r + "[" + min_r + "," + max_r + "]" );
        }
    }
    class Program
    {
        //static EpisodeStats GetLastEpisodeStats(string logfile)
        //{
        //    SimionLog log;
        //    EpisodesData lastEpisode;

        //    log = new SimionLog();
        //    log.LoadBinaryLog(logfile);

        //    if (log.BinFileLoadSuccess)
        //    {
        //        lastEpisode = log.EvaluationEpisodes[log.TotalNumEpisodes - 1];

        //        return new EpisodeStats(lastEpisode);
        //    }
        //    else Console.WriteLine("Error reading file");
        //    return null;
        //}
        static void Main(string[] args)
        {
            //List<EpisodeStats> stats = new List<EpisodeStats>();
            //string baseDir = "..\\..\\..\\..\\..\\experiments\\a\\";

            //EpisodeStats testStats = GetLastEpisodeStats(baseDir + "2mass-reg-controllers-0-0-0-0-0-0\\2mass-reg-controllers-0-0-0-0-0-0.simion.log.bin");

            //foreach (string file in Directory.EnumerateFiles(baseDir, "*.log.bin", SearchOption.AllDirectories))
            //{
            //    EpisodeStats episodeStats= GetLastEpisodeStats( file );
            //    if (episodeStats != null)
            //        stats.Add(episodeStats);
            //}
            //for (int i= 0; i<stats.Count-1; ++i)
            //{
            //    EpisodeStats s = stats[i];
            //    EpisodeStats s_p = stats[i + 1];
            //    if ((s.r > s_p.r && s.E_p > s_p.E_p) || (s.r < s_p.r && s.E_p < s_p.E_p))
            //    {
            //        Console.WriteLine("Error: " + i);
            //        s.Print();
            //        s_p.Print();
            //    }
            //}
        }
    }
}
