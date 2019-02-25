#pragma once

#include <vector>
using namespace std;

class RunTimeRequirements
{
		vector<const char*> m_inputFiles;
		vector<const char*> m_inputFilesRenamed; // names to be given in the remote machines to input files
		vector<const char*> m_outputFiles;
	public:
		RunTimeRequirements();
		~RunTimeRequirements();
		
		void addInputFile(const char* inputFile, const char* inputFileRenamed = nullptr);
		void addOutputFile(const char* outputFile);

		unsigned int getNumInputFiles();
		const char* getInputFile(unsigned int i);
		const char* getInputFileRename(unsigned int i);
		unsigned int getNumOutputFiles();
		const char* getOutputFile(unsigned int i);

		void printXML();
};

