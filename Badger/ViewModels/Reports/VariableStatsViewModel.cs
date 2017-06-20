using System;
using System.Collections.ObjectModel;
using System.IO;
using Caliburn.Micro;
using Badger.Simion;

namespace Badger.ViewModels
{
  
    public class VariableStatsViewModel: PropertyChangedBase
    {
        
        private ObservableCollection<GroupStatsViewModel> m_groups = new ObservableCollection<GroupStatsViewModel>();
        public ObservableCollection<GroupStatsViewModel> groups { get { return m_groups; } set { } }

        private string m_variable;
        public string variable
        {
            get { return m_variable; }
            set { m_variable = value;  NotifyOfPropertyChange(() => variable); }
        }

        public VariableStatsViewModel(string variableName)
        {
            m_variable = variableName;
        }

        public void addGroup(GroupStatsViewModel newGroup)
        {
            groups.Add(newGroup);
        }

        private GroupStatsViewModel m_selectedGroup;
        public GroupStatsViewModel selectedGroup
        {
            get { return m_selectedGroup; }
            set { m_selectedGroup = value; NotifyOfPropertyChange(() => selectedGroup); }
        }
        
        public void export(StreamWriter fileWriter, string leftSpace)
        {
            fileWriter.WriteLine(leftSpace + "<" + XMLConfig.statVariableTag + " " + XMLConfig.nameAttribute + "=\"" +  variable + "\">");
            
            foreach(GroupStatsViewModel group in groups)
            {
                fileWriter.WriteLine(leftSpace + "  <" + XMLConfig.statVariableItemTag 
                    + " " + XMLConfig.groupIdAttribute + "=\"" + group.groupId + "\" "
                    + XMLConfig.trackIdAttribute + "=\"" + group.trackId + "\">");
                group.export(fileWriter, leftSpace + "    ");
                fileWriter.WriteLine(leftSpace + "  </" + XMLConfig.statVariableItemTag + ">");
            }
            
            fileWriter.WriteLine(leftSpace + "</" + XMLConfig.statVariableTag + ">");
        }
        
        
    }
}
