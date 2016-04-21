// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RLSIMIONINTERFACEDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RLSIMIONINTERFACEDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef RLSIMIONINTERFACEDLL_EXPORTS
#define RLSIMIONINTERFACEDLL_API __declspec(dllexport)
#else
#define RLSIMIONINTERFACEDLL_API __declspec(dllimport)
#endif

// This class is exported from the RLSimionInterfaceDLL.dll
class RLSIMIONINTERFACEDLL_API CRLSimionInterfaceDLL {
public:
	CRLSimionInterfaceDLL(void);
	// TODO: add your methods here.
};

extern RLSIMIONINTERFACEDLL_API int nRLSimionInterfaceDLL;

RLSIMIONINTERFACEDLL_API int fnRLSimionInterfaceDLL(void);
