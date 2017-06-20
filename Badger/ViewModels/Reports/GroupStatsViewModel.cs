using Caliburn.Micro;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.ViewModels
{
    public class GroupStatsViewModel : PropertyChangedBase
    {
        public string groupId { get; set; }
        public string trackId { get; set; }

        private ObservableCollection<EpisodeStatsViewModel> m_episodes = new ObservableCollection<EpisodeStatsViewModel>();
        public ObservableCollection<EpisodeStatsViewModel> episodes { get { return m_episodes; } set { } }

        private string m_episodeId;
        public string episodeId
        {
            get { return m_episodeId; }
            set { m_episodeId = value; NotifyOfPropertyChange(() => episodeId); }
        }

        public GroupStatsViewModel(string id, string group)
        {
            trackId = id;
            groupId = group;
        }

        public void addNewEpisodeStats(EpisodeStatsViewModel newStat)
        {
            m_episodes.Add(newStat);
        }

        public void export(StreamWriter fileWriter, string leftSpace)
        {
            foreach (EpisodeStatsViewModel episode in episodes)
            {
                episode.export(fileWriter, leftSpace);
            }
        }

    }
}
