using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Herd
{
    public static class WinApi
    {

        [DllImport("advapi32.dll", CharSet = CharSet.Auto, SetLastError = true)]

        public static extern Boolean CreateProcessAsUser
         (
             IntPtr hToken,
             String lpApplicationName,
             String lpCommandLine,
             IntPtr lpProcessAttributes,
             IntPtr lpThreadAttributes,
             Boolean bInheritHandles,
             Int32 dwCreationFlags,
             IntPtr lpEnvironment,
             String lpCurrentDirectory,
             ref STARTUPINFO lpStartupInfo,
             out PROCESS_INFORMATION lpProcessInformation
         );

        [StructLayout(LayoutKind.Sequential)]

        public struct STARTUPINFO
        {
            public Int32 cb;

            public String lpReserved;

            public String lpDesktop;

            public String lpTitle;

            public Int32 dwX;

            public Int32 dwY;

            public Int32 dwXSize;

            public Int32 dwYSize;

            public Int32 dwXCountChars;

            public Int32 dwYCountChars;

            public Int32 dwFillAttribute;

            public Int32 dwFlags;

            public Int16 wShowWindow;

            public Int16 cbReserved2;

            public IntPtr lpReserved2;

            public IntPtr hStdInput;

            public IntPtr hStdOutput;

            public IntPtr hStdError;

        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SECURITY_ATTRIBUTES
        {
            public int nLength;
            public unsafe byte* lpSecurityDescriptor;
            public int bInheritHandle;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct STARTUPINFOEX
        {
            public STARTUPINFO StartupInfo;
            public IntPtr lpAttributeList;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct PROCESS_INFORMATION
        {
            public IntPtr hProcess;
            public IntPtr hThread;
            public int dwProcessId;
            public int dwThreadId;
        }


        [DllImport("advapi32", SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        public static extern bool SaferCreateLevel(uint dwScopeId, uint dwLevelId, int OpenFlags, out IntPtr pLevelHandle, IntPtr lpReserved);

        [DllImport("advapi32", SetLastError = true, CallingConvention = CallingConvention.StdCall)]
        public static extern bool SaferComputeTokenFromLevel(IntPtr LevelHandle, IntPtr InAccessToken, out IntPtr OutAccessToken, int dwFlags, IntPtr lpReserved);

        [DllImport("advapi32", SetLastError = true)]
        public static extern bool SaferCloseLevel(IntPtr hLevelHandle);

        [DllImport("advapi32.dll")]
        public static extern bool ImpersonateLoggedOnUser(IntPtr hToken);

        [DllImport("advapi32.dll")]
        public static extern bool CreateRestrictedToken(IntPtr ExistingTokenHandle, uint Flags, uint DisableSidCount, IntPtr SidsToDisable, uint DeletePrivilgeCount, IntPtr PriviligesToDelete, uint RestrictedSidCount, IntPtr SidsToRestrict, out IntPtr NewTokenHandle);

        [DllImport("advapi32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool OpenProcessToken(IntPtr ProcessHandle, UInt32 DesiredAccess, out IntPtr TokenHandle);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool CloseHandle(IntPtr hHandle);

        [DllImport("advapi32.dll", SetLastError = true)]
        static extern Boolean SetTokenInformation(IntPtr TokenHandle, uint TokenInformationClass, IntPtr TokenInformation, int TokenInformationLength); //TokenInformationClass = 25 (TokenIntegrityLevel)

        [StructLayout(LayoutKind.Sequential)]
        public struct TOKEN_MANDATORY_LABEL
        {
            public SID_AND_ATTRIBUTES Label;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SID_AND_ATTRIBUTES
        {
            public IntPtr Sid;
            public int Attributes;
        }

        [DllImport("advapi32.dll", SetLastError = true)]
        static extern bool ConvertStringSidToSid(
             string StringSid,
             out IntPtr ptrSid
             );

        [DllImport("advapi32.dll", SetLastError = true)]
        static extern bool AllocateAndInitializeSid(
    ref SidIdentifierAuthority pIdentifierAuthority,
    byte nSubAuthorityCount,
    int dwSubAuthority0, int dwSubAuthority1,
    int dwSubAuthority2, int dwSubAuthority3,
    int dwSubAuthority4, int dwSubAuthority5,
    int dwSubAuthority6, int dwSubAuthority7,
    out IntPtr pSid);

        [StructLayout(LayoutKind.Sequential)]
        public struct SidIdentifierAuthority
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6, ArraySubType = UnmanagedType.I1)]
            public byte[] Value;
        }

        [DllImport("advapi32.dll", SetLastError = true)]
        static extern bool CheckTokenMembership(IntPtr TokenHandle, IntPtr SidToCheck, out bool IsMember);

        public static Process StartUnelevatedProcess(string command, string commandArgs, string workingDirectory)
        {
            //create new safe handle
            IntPtr hLevel = new IntPtr();
            var result = SaferCreateLevel(0x2, 0x20000, 0x1, out hLevel, IntPtr.Zero);

            //restrict it now
            IntPtr hRestrictedLevel = new IntPtr();
            result = SaferComputeTokenFromLevel(hLevel, IntPtr.Zero, out hRestrictedLevel, 0, IntPtr.Zero);

            result = SaferCloseLevel(hLevel);

            //reduce the integrity
            TOKEN_MANDATORY_LABEL tml = new TOKEN_MANDATORY_LABEL();
            tml.Label = new SID_AND_ATTRIBUTES();
            tml.Label.Attributes = 0x00000020;
            ConvertStringSidToSid("S-1-16-8192", out tml.Label.Sid);

            IntPtr unmanagedAddr = Marshal.AllocHGlobal(Marshal.SizeOf(tml));
            Marshal.StructureToPtr(tml, unmanagedAddr, true);

            result = SetTokenInformation(hRestrictedLevel, 25, unmanagedAddr, Marshal.SizeOf(tml));

            IntPtr pSID = new IntPtr();

            const int NtSecurityAuthority = 5;
            SidIdentifierAuthority NtAuthority = new SidIdentifierAuthority();
            NtAuthority.Value = new byte[] { 0, 0, 0, 0, 0, NtSecurityAuthority };

            AllocateAndInitializeSid(ref NtAuthority, 2, 32, 544, 0, 0, 0, 0, 0, 0, out pSID);

            CheckTokenMembership(IntPtr.Zero, pSID, out result);
            CheckTokenMembership(hRestrictedLevel, pSID, out result);
        
            var adminSID = new SID_AND_ATTRIBUTES();
            adminSID.Sid = pSID;
            IntPtr adminSIDIntPtr = Marshal.AllocHGlobal(Marshal.SizeOf(adminSID));
            Marshal.StructureToPtr(adminSID, adminSIDIntPtr, true);

            result = CreateRestrictedToken(hRestrictedLevel, 0, 1, adminSIDIntPtr, 0, IntPtr.Zero, 0, IntPtr.Zero, out hRestrictedLevel);

            // Initialize structs
            PROCESS_INFORMATION pi;
            STARTUPINFO si = new STARTUPINFO();
            si.cb = Marshal.SizeOf(si);
            si.lpDesktop = "winsta0\\default";

            // Now create the process as the user
            result = CreateProcessAsUser(hRestrictedLevel, command, command + " " + commandArgs, IntPtr.Zero, IntPtr.Zero, false, 0, IntPtr.Zero, workingDirectory, ref si, out pi);

            //close handles
            //CloseHandle(hRestrictedLevel);
            //CloseHandle(pi.hThread);
            //CloseHandle(pi.hProcess);

            return Process.GetProcessById(pi.dwProcessId);
        }
    }
}
