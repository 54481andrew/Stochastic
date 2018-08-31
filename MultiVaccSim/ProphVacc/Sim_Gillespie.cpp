
/*
Build a simple c++ script that will 
simulate a stochastic SIR epidemiological
model. The simulation procedes according
to the Gillespie algorithm, and simulates 
the use of vaccination to prevent the invasion
of a zoonotic pathogen. 

-This sim is TRUE Gillespie
*/

#include <iostream> // input/output std::cout, cin
#include <math.h>   // Math functions
#include <stdlib.h>
#include <time.h>
#include <fstream> //Used to open files to which data are saved
#include <vector> 
#include <string.h>
#include <sys/stat.h> // mkdir

//********
//CONSTANT MACRO
#define TPathLEN 1
#define tvLEN 1
#define NTrials 1
#define NParSets = tvLEN*TPathLEN*1*1; //Number of parameter sets simulated
#define NumPars 12

//********
//USER-ASSIGNED VARIABLES
char SimName[50] = "Sim_Test";
bool VerboseWriteFlag = true;

vector<double> tvVals;

double TPathMIN = 5*365; double TPathMAX = 6*365; 
vector<double> TPathInvVals;

double bpvals[] = {0.00005};
vector<double> BpVals; 

int pinitvals[]={10};
vector<int> PInitVals; 

double TMax = 10.0*365.0; double tick = 1.0; //OneSim writes data at time-intervals tick

int SInit = 10000;

//********
//ARRAYS
double ParMat [NParSets][NumPars];
double TExtMat [NParSets][NTrials];

//********
//CRITICAL VARIABLES

int S, Iv, Ip, V, P, NPop, Par;
char FileNamePar[50] = "Data/ParMat_"; 
char FileNameTExt[50] = "Data/TExtMat_";
char DirName[50] = "Data/";
char FileSuffix[50], FileNameDat[50], FileNameTExt[50], DirName[50];
double b0, tb, T, tv, b, gamv, R0p, Bp, gamp, d, Event_Rate, Event_Rate_Prod, RandDeath, dTime, t, ti, TPathInv;
ofstream out_data;

//OTHER VARIABLES
int nbirths, ndeaths, ninfv, ninfp, nrecv, nrecp, S_death, Iv_death, Ip_death, V_death, P_death;

//FUNCTION DECLARATIONS
void ApplyEvent();
void GetTime();
void Initialize(); //Fills ParMat, initializes TExtMat, returns #Rows
void OneSim (double StartTime, double EndTime, bool StopOnErad);
double Rand () ;
vector<double> Seq(double, double, int); //Returns sequence
void VaccFun();
void WriteMat(double **arr, int NRow, int NCol, char* filename); 


//MAIN
int main()
{
  srand ( time(NULL) );

  strcat(FileNamePar, SimName);  
  strcat(FileNameTExt, SimName);  
  Initialize(); //Fill in the parameter matrix

  WriteMat(ParMat, NParSets, NumPars, FileNamePar); //Write ParMat
  
  for(int Par = 0; Par < NParSets; Par++) //Loop through parameters
    {
      if(VerboseWriteFlag){
	strcat(DirName, SimName);  
	mkdir(DirName, ACCESSPERMS);
	strcat(DirName, "/");	  
	strcpy(FileNameDat, DirName);
	sprintf(FileSuffix, "Par_%d",Par);
	strcat(FileNameDat, FileSuffix);
	out_data.open(FileNameDat);
	out_data << "time S Iv Ip V P N births deaths ninfv ninfp nrecv nrecp S_death Iv_death Ip_death V_death P_death\n";
      }
      
      //Extract parameters
      //Parmat[0] corresponds to Par;
      b0 = ParMat[1][Par]; //b0
      d = ParMat[2][Par]; //d
      Bp = ParMat[3][Par]; //Bp
      Nv = ParMat[4][Par]; //Nv
      tv = ParMat[5][Par]; //tv
      gamv = ParMat[6][Par]; //gamv
      gamp = ParMat[7][Par]; //gamp
      tb = ParMat[8][Par]; //tb
      T = ParMat[9][Par]; //T
      PInit = (int) ParMat[10][Par]; // Initial level of pathogen upon invasion
      TPathInv = ParMat[11][Par]; // Time of pathogen invasion
      
      for(int ntrial = 0; ntrial < NTrials; ntrial++)
	{
	  S = SInit; //S
	  Iv = 0; //Iv
	  Ip = 0; //Ip
	  V = 0; //V
	  P = 0; //P
	  NPop = S + Iv + Ip + V + P;

	  //Simulate to quasi steady state (rewrites State)
	  OneSim(0, TPathInv, false);
	      
	  //Simulate invasion until TMax years, or pathogen extinction
	  P = PInit;
	  OneSim(TPathInv, TMax, true);
	  
	  //Store final value of t in TExtMat
	  TExtMat[ntrial][Par] = t;
	      
	  std::cout << "Sim Trial: " << ntrial << endl;
	  
	}//End loop through NTrials
	  
      std::cout << "*****************************" << endl;	  
      std::cout << "Finished Parameter Set " << Par+1 << " / " << NParSets << endl;
      std::cout << "*****************************" << endl;
      
    }//End Loop through NParSets
  
  WriteMat(TExtMat, NParSets, NTrials, FileNameTExt); //Write TExtMat

  //Remove ParMat, TExtMat from memory
  for (int j=0; j<NumPars; j++)
    delete [] ParMat[j];
  delete [] ParMat;
  
  for (int j=0; j<NTrials; j++)
    delete [] TExtMat[j];
  delete [] TExtMat;
      
}//End Main

      
      
//////////////////////////////////////
///////// DEFINE FUNCTIONS ///////////
//////////////////////////////////////


void OneSim (double StartTime, double EndTime, bool StopOnErad = false)
{
  //Set initial conditions: No Pathogen, no vaccination
  ti = StartTime;
  t = StartTime;
  b = (double) b0*(t%%T < tb));
  //Track births, deaths, recoveries, etc for error checking
  nbirths = 0; ndeaths = 0; ninfv = 0; ninfp = 0; nrecv = 0; 
  nrecv = 0; S_death = 0; Iv_death = 0; Ip_death = 0;  
  V_death = 0; P_death = 0;
  
  while(t < EndTime && (Ip > 0 || !StopOnErad)) {
    Event_Rate = b + d*NPop + Bp*S*Ip + Bp*Iv*Ip + gamv*Iv + gamp*Ip ;
    Event_Rate_Prod = Event_Rate*Rand();
    
    dTime = GetTime(); //Get time to next event

    //Check if dTime brings t past tv, tb, or T
    if()//HERE
    
    ApplyEvent();

    //Vaccination
    if ( (fmod(t,T) <= tv) && (fmod(t + dTime,T) > tv ) )
      {
      }



    
    //Write old values
    if( t >= ti && VerboseWriteFlag)
      {
	out_data << t << " " << S << " " << Iv << " " << Ip << " " << V << " " << P  << " " << 
	  NPop << " " << nbirths << " " << ndeaths <<  " " << ninfv << " " << ninfp << " " << nrecv << 
	  " " << nrecp << " " << S_death << " " << Iv_death << " " << Ip_death << " " << V_death << " " << P_death << "\n"; 
	ti += tick;
	nbirths = 0; ndeaths = 0;
      }  
    t += dTime;
  }//End While

  if(VerboseWriteFlag){
    out_data << t << " " << S << " " << Iv << " " << Ip << " " << V << " " << P  << " " << NPop << " " << nbirths << " " << ndeaths <<  " " << ninfv << " " << ninfp << " " << nrecv << " " << nrecp << " " << S_death << " " << Iv_death << " " << Ip_death << " " << V_death << " " << P_death << "\n"; 
    out_data.close();
  }
  
  //Update state vector
  State[0] = S;
  State[1] = Iv;
  State[2] = Ip;
  State[3] = V;
  State[4] = P;
}//End OneSim



//************************************
//Function returning a random number between 0 and 1
double Rand (){
  double unif = 0.0;
  do
    {
      unif = (double) rand() / RAND_MAX ;
    } while(unif==0.0);
  return unif;
}  

//************************************
//Function that returns the time for the next event
double GetTime (){
  double dtime, random;
  // Select random time step (doesn't allow for infite time step)
  do
    {
      random = (double) rand() / RAND_MAX ;
      dtime = -log(random) / Event_Rate;
    } while (tstep == INFINITY); //End of DO WHILE loop
  return(dtime);
}

//************************************
//Function that updates state variables
void ApplyEvent() {
  if(Event_Rate_Prod <= b) //Birth
    { S++; NPop++; nbirths++; } 
  else if(Event_Rate_Prod <= b + d*NPop) //Death
    {
      RandDeath = Rand()*NPop;
      if( RandDeath < S ) //S dies
	{S--; S_death++;}
      else if(RandDeath < S + Iv) //Iv dies
	{Iv--; Iv_death++;}
      else if(RandDeath < S + Iv + Ip) //Ip dies
	{Ip--; Ip_death++;}
      else if(RandDeath < S + Iv + Ip + V) //V dies
	{V--; V_death++;}
      else{P--; P_death++;}  //P dies	
      NPop--; ndeaths++;
    }  
  else if(Event_Rate_Prod <= b + d*NPop + Bp*Ip*S)    //Event: Pathogen infection of S
    {S--; Ip++; ninfp++;}
  else if(Event_Rate_Prod <= b + d*NPop + Bp*Ip*S + Bp*Ip*Iv) //Event: Pathogen infection of V
    {Iv--; Ip++; ninfp++;}
  else if(Event_Rate_Prod <= b + d*NPop + Bp*Ip*S + Bp*Ip*Iv + gamv*Iv) //Event: Iv Recovery
    {Iv--;V++;nrecv++;}
  else if(Event_Rate_Prod <= b + d*NPop + Bp*Ip*S + Bp*Ip*Iv + gamv*Iv) //Event: Ip Recovery
    {Ip--; P++; nrecp++;}
}

//************************************
//Birth function
 double BirthRate( double time ) {
   //Local function declaration
   double result;
   double modtime;
   modtime = fmod(time, T ) ;
   result = b0*(modtime < tb) ;
   return result;
 }


//************************************
//Vaccination function
void VaccFun(){
  int temp  = min(round( (double) Nv*S/(max(NPop,1))), S);
  S -= temp;
  Iv += temp;
}

//************************************
//function to Initialize values of 2D array
void Initialize()
{
  tvVals = Seq(1, 365, tvLEN);
  TPathInvVals = Seq(TPathMIN, TPathMAX, TPathLEN);
  BpVals.assign(bpvals, bpvals + BpLEN);
  PInitVals.assign(pinitvals, pinitvals + PInitLEN);
  
  //Fill in ParMat
  int i = 0;
  for(int i1=0; i1<tvVals.size(); i1++)
    for(int i2=0; i2<BpVals.size(); i2++)
      for(int i3=0; i3<TPathInvVals.size(); i3++)
	for(int i4=0; i4<PInitVals.size();i4++)
	  {
	    ParMat[0][i] = i; //Par
	    ParMat[1][i] = 100.0;   //b0
	    ParMat[2][i] = 0.004; //d
	    ParMat[3][i] = BpVals[i2]; //Bp
	    ParMat[4][i] = 5000.0; //Nv
	    ParMat[5][i] = tvVals[i1]; //tv
	    ParMat[6][i] = 0.007; //gamv
	    ParMat[7][i] = 0.007; //gamp
	    ParMat[8][i] = 90.0; //tb
	    ParMat[9][i] = 365.0; //T
	    ParMat[10][i] = (double) PInitVals[i4]; //PInit
	    ParMat[11][i] = TPathInvVals[i3]; //TPathInv
	    i++;
	  }
  return i; //Return total number of rows
}

//************************************
//function to write values of 2D array
void WriteMat(double **arr, int NRow, int NCol, char*filename)
{
  out_data.open(filename);
  int i,j;
  for(i=0; i<NRow;i++)
    {
      for(j=0; j<NCol; j++)
	{
	  out_data << arr[i][j] << " "; //Write data
	}
      out_data << endl;
    }
  out_data.close();
}


//************************************
//function that returns a sequence from minval to maxval, and of length lengthval
vector<double> Seq(double minval, double maxval, int lengthval) {
  vector<double> vec(lengthval);
  if(lengthval==1)
    {
      vec[0] = minval;
    }else{
  for(int seqindex = 0; seqindex < lengthval; seqindex++)
    {
      vec[seqindex] = minval + (double) seqindex/(lengthval-1)*(maxval-minval);
    }
  }
  return vec;
}
