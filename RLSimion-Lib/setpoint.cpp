#include "stdafx.h"
#include "setpoint.h"


//CFileSetPoint//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

int countlines(char *filename)
{
  // count the number of lines in the file called filename                                    
  FILE *fp;
  
  fopen_s(&fp,filename,"r");
  int ch=0;
  int lines=1;

  while (!feof(fp))
    {
	ch= getc(fp);
      if (ch == '\n')
    lines++;
    }
  fclose(fp);
  return lines;
}

CFileSetPoint::CFileSetPoint(char *pFilename)
{
	//char fullFilename[1024];
	m_numSteps= 0;//(int)g_pParameters->getParameter("TIME")/g_pParameters->getParameter("DELTA_T");


	FILE *pFile;

	int numLines= countlines(pFilename);
	if (numLines==0) return;

	char buffer [1024];
	fopen_s(&pFile,pFilename,"r");
	
	if (pFile!=0)
	{
		m_pSetPoints= new double [numLines];
		m_pTimes= new double [numLines];

		while (!feof(pFile))
		{
			fgets(buffer,1024,pFile);
			if (sscanf_s(buffer,"%lf %lf\n",&m_pTimes[m_numSteps],&m_pSetPoints[m_numSteps])==2)
				m_numSteps++;
		}
		fclose(pFile);
	}
	else
		printf("ERROR: could not open setpoint file %s\n",pFilename);

	m_totalTime= m_pTimes[m_numSteps-1];
}


CFileSetPoint::~CFileSetPoint()
{
	if (m_pSetPoints) delete [] m_pSetPoints;
}

double CFileSetPoint::getPointSet(double time)
{
	int i= 0;

	if (m_totalTime==0) return 0.0;

	while (time>m_totalTime)
		time-= m_totalTime;

	//first approach: assume equal-lenght time points
	i= (int) (time/(m_totalTime/m_numSteps));

	//forward?
	while (i< m_numSteps-1 && time>m_pTimes[i+1])
		i++;
	//backward?
	while (i>=0 && time<m_pTimes[i])
		i--;

	if (i<m_numSteps-1)
	{
		double u;
		u= ((time-m_pTimes[i])/(m_pTimes[i+1]-m_pTimes[i]));
		return m_pSetPoints[i] + u* (m_pSetPoints[i+1]-m_pSetPoints[i]);
	}

	return m_pSetPoints[m_numSteps-1];//step % m_numSteps];
}





//CFixedStepSizeSetPoint////////////////////////////////////////
///////////////////////////////////////////////////////////////

CFixedStepSizeSetPoint::CFixedStepSizeSetPoint(double stepTime, double min, double max)
{
	m_stepTime= stepTime;
	m_min= min;
	m_max= max;
	m_lastStepTime= 0.0;
	m_lastSetPoint= min;
}

CFixedStepSizeSetPoint::~CFixedStepSizeSetPoint(){}

double CFixedStepSizeSetPoint::getPointSet(double time)
{
	if (time==0.0 || (time-m_lastStepTime>m_stepTime))
	{
		m_lastSetPoint= ((double)(rand()%10000))*(m_max-m_min) + m_min;
		m_lastStepTime= time;
	}
	return m_lastSetPoint;

}