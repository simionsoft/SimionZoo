using System.Reflection;


namespace Herd.Files
{
    public class Version
    {
        public static string SimionZoo
        {
            get
            {
                System.Version version = Assembly.GetExecutingAssembly().GetName().Version;
                return version.Major + "." + version.Minor + "." + version.Build;
            }
        }
    }
}
