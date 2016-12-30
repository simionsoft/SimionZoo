// This is the main DLL file.

#include "stdafx.h"

#include "FASTDimensionalPortalDLL.h"
#include "FASTWorld.h"

#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))

extern "C" {

	//static double GenSpeedF;	//Filtered HSS (generator) speed, rad/s.
	//static double IntSpdErr; //Current integral of speed error w.r.t. time, rad.
	//static double LastGenTrq; //Commanded electrical generator torque the last time the controller was called, N-m.
	//static double LastTime; //Last time this DLL was called, sec.
	//static double LastTimePC; //Last time the pitch  controller was called, sec.
	//static double LastTimeVS; //Last time the torque controller was called, sec.
	//static double PitCom[3]; //Commanded pitch of each blade the last time the controller was called, rad.
	//static double LastGenSpeedF;
	//static double LastRotorSpeed;

//
//#define RATED_POWER 5296610.0//5000000
//#define INITIAL_TORQUE 40000.0
//
//	double sgn(double value)
//	{
//		if (value<0.0) return -1.0;
//		else if (value>0.0) return 1.0;
//
//		return 0.0;
//	}

	FASTWorld g_FASTWorld;
	bool g_bDummyTest= true;	//for debugging

	void __declspec(dllexport) __cdecl DISCON(float *avrSWAP //inout
		, int *aviFAIL //inout
		, char *accINFILE //in
		, char* avcOUTNAME //in
		, char *avcMSG) //inout
	{
		int iStatus = NINT(avrSWAP[1 - 1]);

		if (iStatus==0)
		{
			//Ony in first call
			g_FASTWorld.connectToNamedPipeServer();
			else printf("Dimensional portal opened\n");
		}
		if (iStatus>=0)
		{
			//Done in all calls but last
			g_FASTWorld.retrieveStateVariables(avrSWAP,(bool)(iStatus==0));
			g_FASTWorld.sendState();
			g_FASTWorld.receiveAction();
			g_FASTWorld.setActionVariables(avrSWAP);
		}
		if (iStatus < 0)
		{
			//Last call
			g_FASTWorld.disconnectFromNamedPipeServer();
		}
	}
	//{
	//	int inFileLen = (int)avrSWAP[50 - 1];
	//	int outNameLen = (int)avrSWAP[51 - 1];
	//	int msgLen = (int)avrSWAP[49 - 1];

	//	//Local Variables:

	//	double Alpha; //Current coefficient in the recursive, single-pole, low-pass filter, (-).
	//	double BlPitch[3]; //Current values of the blade pitch angles, rad.
	//	double ElapTime; //Elapsed time since the last call to the controller, sec.
	//	double CornerFreq = 1.570796; //Corner frequency (-3dB point) in the recursive, single-pole, low-pass filter, rad/s. -- chosen to be 1/4 the blade edgewise natural frequency ( 1/4 of approx. 1Hz = 0.25Hz = 1.570796rad/s)
	//	double GenSpeed;		//Current  HSS (generator) speed, rad/s.
	//	double GenTrq;	//Electrical generator torque, N-m.
	//					//double GK;	//Current value of the gain correction factor, used in the gain scheduling law of the
	//					//pitch controller, (-).
	//	double HorWindV; //Horizontal hub-heigh wind speed, m/s.
	//					 //	double OnePlusEps= 1.0 + DBL_EPSILON; //The number slighty greater than unity in single precision.
	//	double PC_DT = 0.0125;  //Communication interval for pitch  controller, sec.
	//	double PC_KI = 0.008068634; //Integral gain for pitch controller at rated pitch (zero), (-).
	//	double PC_KK = 0.1099965; //Pitch angle where the the derivative of the aerodynamic power w.r.t. pitch has increased by a factor of two relative to the derivative at rated pitch (zero), rad.
	//	double PC_KP = 0.01882681; //Proportional gain for pitch controller at rated pitch (zero), sec.
	//	double PC_MaxPit = 1.570796; //Maximum pitch setting in pitch controller, rad.
	//	double PC_MaxRat = 0.1396263; //Maximum pitch  rate (in absolute value) in pitch  controller, rad/s.
	//	double PC_MinPit = 0.0; //Minimum pitch setting in pitch controller, rad.
	//	double PC_RefSpd = 122.9096; //Desired (reference) HSS speed for pitch controller, rad/s.
	//	double PC_RefRotorSpd = 1.267109037;//Rated rotor speed for pitch controller, rad/s

	//	double PitRate[3]; //Pitch rates of each blade based on the current pitch angles and current pitch command, rad/s.
	//	double R2D = 57.295780; //Factor to convert radians to degrees.
	//	double RPS2RPM = 9.5492966; //Factor to convert radians per second to revolutions per minute.
	//	double SpdErr; //Current speed error, rad/s.
	//	double Time; //Current simulation time, sec.
	//	double TrqRate; //Torque rate based on the current and last torque commands, N-m/s.
	//	double VS_DT = 0.0125; //Communication interval for torque controller, sec.
	//	double VS_MaxRat = 15000.0; //Maximum torque rate (in absolute value) in torque controller, N-m/s.
	//	double VS_MaxTq = 47402.91; //Maximum generator torque in Region 3 (HSS side), N-m. -- chosen to be 10% above VS_RtTq = 43.09355kNm
	//	double VS_RtPwr = RATED_POWER; //Rated generator generator power in Region 3, Watts. -- chosen to be 5MW divided by the electrical generator efficiency of 94.4%
	//	double RotorSpeed;

	//	int iStatus; //A status flag set by the simulation as follows: 0 if this is the first call, 1 for all subsequent time steps, -1 if this is the final call at the end of the simulation.
	//	int NumBl; //Number of blades, (-).
	//	int UnDb = 85; //I/O unit for the debugging information
	//	int UnDb2 = 86;                              //I/O unit for the debugging information


	//	bool PC_DbgOut = false; //Flag to indicate whether to output debugging information

	//							//char Tab='\t'; //The tab character.
	//							//char FmtDat[26]= "(F8.3,99('"//Tab//"',ES10.3E2,:))" //The format of the debugging data

	//	char inFile[512];
	//	strcpy_s(inFile, 512, accINFILE);
	//	//CHARACTER(SIZE(accINFILE)-1) :: InFile                                          //a Fortran version of the input C string (not considered an array here)    [subtract 1 for the C null-character]
	//	char RootName[512];
	//	strcpy_s(RootName, 512, avcOUTNAME);
	//	//CHARACTER(SIZE(avcOUTNAME)-1):: RootName                                        //a Fortran version of the input C string (not considered an array here)    [subtract 1 for the C null-character]
	//	char ErrMsg[512];
	//	ErrMsg[0] = 0;
	//	//CHARACTER(SIZE(avcMSG)-1)    :: ErrMsg                                          //a Fortran version of the C string argument (not considered an array here) [subtract 1 for the C null-character] 


	//	//Load variables from calling program (See Appendix A of Bladed User's Guide):

	//	iStatus = NINT(avrSWAP[1 - 1]);
	//	NumBl = NINT(avrSWAP[61 - 1]);

	//	BlPitch[1 - 1] = (double)avrSWAP[4 - 1];
	//	BlPitch[2 - 1] = (double)avrSWAP[33 - 1];
	//	BlPitch[3 - 1] = (double)avrSWAP[34 - 1];
	//	GenSpeed = avrSWAP[20 - 1];
	//	HorWindV = avrSWAP[27 - 1];
	//	Time = avrSWAP[2 - 1];
	//	RotorSpeed = avrSWAP[21 - 1];
	//	//Initialize *aviFAIL to 0:


	//	*aviFAIL = 0;

	//	//Read any External Controller Parameters specified in the User Interface
	//	//  and initialize variables:

	//	if (iStatus == 0)  //.TRUE. if we're on the first call to the DLL
	//	{
	//		initialize();


	//		
	//		*aviFAIL = 1;
	//		strcpy_s(ErrMsg, 512, "FASTDimensionalPortal to RLSimion opened\n");

	//		GenSpeedF = GenSpeed;                       //This will ensure that generator speed filter will use the initial value of the generator speed on the first pass
	//		for (int i = 0; i<3; i++)
	//			PitCom[i] = BlPitch[i];   //This will ensure that the variable speed controller picks the correct control region and the pitch controller picks the correct gain on the first call

	//		IntSpdErr = 0.0;//PitCom[1 -1]/( GK*PC_KI );          //This will ensure that the pitch angle is unchanged if the initial SpdErr is zero

	//		LastTime = Time; //This will ensure that generator speed filter will use the initial value of the generator speed on the first pass
	//		LastTimePC = Time - PC_DT; //This will ensure that the pitch  controller is called on the first pass 
	//		LastTimeVS = Time - VS_DT; //This will ensure that the torque controller is called on the first pass 

	//		LastGenTrq = INITIAL_TORQUE;
	//		LastGenSpeedF = GenSpeedF;
	//		LastRotorSpeed = RotorSpeed;
	//	}
	//	if (iStatus > 0 && *aviFAIL >= 0)
	//	{

	//		g_pS->setValue("beta", (double)avrSWAP[4 - 1]);
	//		//BlPitch[1 - 1] = (double)avrSWAP[4 - 1];
	//		//BlPitch[2 - 1] = (double)avrSWAP[33 - 1];
	//		//BlPitch[3 - 1] = (double)avrSWAP[34 - 1];

	//		GenSpeed = avrSWAP[20 - 1];
	//		HorWindV = avrSWAP[27 - 1];
	//		Time = avrSWAP[2 - 1];
	//		RotorSpeed = avrSWAP[21 - 1];


	//		//Set unused outputs to zero (See Appendix A of Bladed User's Guide):

	//		avrSWAP[36 - 1] = 0.0; //Shaft brake status: 0=off
	//		avrSWAP[41 - 1] = 0.0; //Demanded yaw actuator torque
	//		avrSWAP[46 - 1] = 0.0; //Demanded pitch rate (Collective pitch)
	//		avrSWAP[48 - 1] = 0.0; //Demanded nacelle yaw rate
	//		avrSWAP[65 - 1] = 0.0; //Number of variables returned for logging
	//		avrSWAP[72 - 1] = 0.0; //Generator start-up resistance
	//		avrSWAP[79 - 1] = 0.0; //Request for loads: 0=none
	//		avrSWAP[80 - 1] = 0.0; //Variable slip current status
	//		avrSWAP[81 - 1] = 0.0; //Variable slip current demand

	//							   //Filter the HSS (generator) speed measurement:
	//							   //NOTE: This is a very simple recursive, single-pole, low-pass filter with
	//							   //      exponential smoothing.

	//							   //Update the coefficient in the recursive formula based on the elapsed time
	//							   //  since the last call to the controller:

	//							   //Alpha     = exp( ( LastTime - Time )*CornerFreq );
	//							   //Apply the filter:
	//		GenSpeedF = GenSpeed;
	//		//( 1.0 - Alpha )*GenSpeed + Alpha*GenSpeedF;

	//		//double currentEstimatedPower= LastGenSpeedF*LastGenTrq;
	//		double measuredPower = avrSWAP[14 - 1];
	//		double PowerError = RATED_POWER - measuredPower;

	//		//Variable-speed torque control:
	//		ElapTime = Time - LastTimeVS;

	//		double d_T_g;
	//		double C_0 = 1.0;
	//		double J_r = 115926.0;
	//		double n_g = 97.0;
	//		double J_g = 534.116;
	//		double J_t;
	//		double T_a;
	//		double d_omega_r;
	//		double K_t = 6215000;

	//		if ( /*( Time*OnePlusEps - LastTimeVS )*/ (ElapTime + 0.001) >= VS_DT)
	//		{
	//			d_omega_r = (RotorSpeed - LastRotorSpeed) / ElapTime;
	//			T_a = (J_r + J_g*n_g*n_g)*d_omega_r + measuredPower / GenSpeedF;
	//			J_t = J_r + n_g*n_g*J_g;

	//			//d(Tg)/dt= (1/omega_r)*(C_0*error_P - (1/J_t)*(T_a*T_g - K_t*omega_r*T_g - T_g*T_g))
	//			d_T_g = (1.0 / GenSpeedF)*(C_0*PowerError - (1 / J_t)*(T_a*LastGenTrq - K_t*RotorSpeed*LastGenTrq
	//				- LastGenTrq*LastGenTrq));


	//			d_T_g = std::min(std::max(d_T_g, -VS_MaxRat), VS_MaxRat);

	//			GenTrq = LastGenTrq + d_T_g*ElapTime;

	//			//Saturate the commanded torque using the maximum torque limit:

	//			GenTrq = std::min(GenTrq, VS_MaxTq);   //Saturate the command using the maximum torque limit


	//											  //Reset the values of LastTimeVS and LastGenTrq to the current values:

	//			LastTimeVS = Time;
	//			LastGenTrq = GenTrq;
	//			LastGenSpeedF = GenSpeedF;
	//			LastRotorSpeed = RotorSpeed;
	//		}


	//		//Set the generator contactor status, avrSWAP(35), to main (high speed) 
	//		//  variable-speed generator, the torque override to yes, and command the
	//		//  generator torque (See Appendix A of Bladed User's Guide):

	//		avrSWAP[35 - 1] = 1.0;          //Generator contactor status: 1=main (high speed) variable-speed generator
	//		avrSWAP[56 - 1] = 0.0;          //Torque override: 0=yes
	//		avrSWAP[47 - 1] = (float)LastGenTrq;   //Demanded generator torque





	//											   //Pitch control:

	//											   //Compute the elapsed time since the last call to the controller:

	//		ElapTime = Time - LastTimePC;


	//		//Only perform the control calculations if the elapsed time is greater than
	//		//  or equal to the communication interval of the pitch controller:
	//		//NOTE: Time is scaled by OnePlusEps to ensure that the controller is called
	//		//      at every time step when PC_DT = DT, even in the presence of
	//		//      numerical precision errors.

	//		if ((ElapTime + 0.001) >= PC_DT)
	//		{
	//			//Compute the current speed error and its integral w.r.t. time; saturate the
	//			//  integral term using the pitch angle limits:

	//			SpdErr = RotorSpeed - PC_RefRotorSpd;                                 //Current speed error
	//			IntSpdErr = IntSpdErr + SpdErr*ElapTime;                           //Current integral of speed error w.r.t. time


	//			double KI = 0.000;
	//			double KP = 0.10;

	//			//Saturate the integral term using the pitch angle limits, converted to integral speed error limits
	//			if (KI != 0.0)
	//				IntSpdErr = std::min(std::max(IntSpdErr, PC_MinPit / KI), PC_MaxPit / KI);


	//			double dBeta = //0.5*KP*SpdErr*(max(min(0.5,SpdErr*10.0),-0.5)+1.0);
	//				KP*SpdErr + KI*IntSpdErr;						//typical PI
	//																//0.5*KP*SpdErr*(1.0+sgn(SpdErr))+KI*IntSpdErr; // in the original paper
	//																//	dBeta= min( max( dBeta, -PC_MaxRat ), PC_MaxRat );
	//																//Saturate the overall commanded pitch using the pitch rate limit:
	//																//NOTE: Since the current pitch angle may be different for each blade
	//																//      (depending on the type of actuator implemented in the structural
	//																//      dynamics model), this pitch rate limit calculation and the
	//																//      resulting overall pitch angle command may be different for each
	//																//      blade.

	//			for (int k = 0; k<NumBl; k++) //Loop through all blades
	//			{
	//				PitCom[k] = BlPitch[k] + dBeta*ElapTime;
	//				PitCom[k] = std::min(std::max(PitCom[k], PC_MinPit), PC_MaxPit);
	//				PitRate[k] = (PitCom[k] - BlPitch[k]) / ElapTime; //Pitch rate of blade K (unsaturated)
	//				PitRate[k] = std::min(std::max(PitRate[k], -PC_MaxRat), PC_MaxRat); //Saturate the pitch rate of blade K using its maximum absolute value
	//				PitCom[k] = BlPitch[k] + PitRate[k] * ElapTime; //Saturate the overall command of blade K using the pitch rate limit

	//				PitCom[k] = std::min(std::max(PitCom[k], PC_MinPit), PC_MaxPit); //Saturate the overall command using the pitch angle limits         
	//			}

	//			LastTimePC = Time;
	//		}


	//		//Set the pitch override to yes and command the pitch demanded from the last
	//		//  call to the controller (See Appendix A of Bladed User's Guide):

	//		avrSWAP[55 - 1] = 0.0;       //Pitch override: 0=yes

	//		avrSWAP[42 - 1] = (float)PitCom[1 - 1]; //Use the command angles of all blades if using individual pitch
	//		avrSWAP[43 - 1] = (float)PitCom[2 - 1]; //"
	//		avrSWAP[44 - 1] = (float)PitCom[3 - 1]; //"

	//		avrSWAP[45 - 1] = (float)PitCom[1 - 1]; //Use the command angle of blade 1 if using collective pitch

	//												//Reset the value of LastTime to the current value:
	//		LastTime = Time;
	//	}

	//	strcpy_s(avcMSG, msgLen - 1, ErrMsg);//C_NULL_CHAR, avcMSG )

	//	return;
	//}

}