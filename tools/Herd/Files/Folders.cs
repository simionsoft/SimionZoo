using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Herd.Files
{
    public class Folders
    {
        public const string binDir = "bin";
        public const string experimentDir = "experiments";
        public const string imageDir = "images";
        public const string appConfigRelativeDir = "../config/apps";
        public const string experimentRelativeDir = "../" + experimentDir;
        public const string imageRelativeDir = "../" + imageDir;
        public const string badgerLogFile = "badger-log.txt";
        public const string tempRelativeDir = "../temp/";
        public const string logViewerExePath = "../" + binDir + "/SimionLogViewer.exe";
    }
}
