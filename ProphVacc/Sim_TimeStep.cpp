
/*
Build a simple c++ script that will 
simulate a stochastic SIR epidemiological
model. The simulation procedes according
to the Gillespie algorithm, and simulates 
the use of vaccination to prevent the invasion
of a zoonotic pathogen. 
*/

#include <iostream> // input/output cout, cin
#include <math.h> // Math functions
#include <stdlib.h>
#include <time.h>
#include <fstream> //Used to open files to which data are saved
#include <vector> 
#include <string.h>
#include <sys/stat.h> // mkdir

using namespace std;

//USER-ASSIGNED VARIABLES
char SimName[50] = "Sim_2";
bool VerboseWriteFlag = false;

//OTHER VARIABLES
bool EventFlag;
int S_next, S, Iv_next, Iv, Ip_next, Ip, V_next, V, P_next, P, Npop_next, Npop, nbirths, ndeaths, ninfv, ninfp, nrecv, nrecp, S_death, Iv_death, Ip_death, V_death, P_death, Nv, NTrials, NPar, PInit;
int State[5];
char FileNamePar[50], FileSuffix[50], FileNameDat[50], FileNameTExt[50], DirName[50];
double b0, tb, T, tv, b, gamv, R0p, Bp, gamp, d, Sum_rate, UniSeed, RandDeath, Picker, Sum, dTime, t, TMax, tick, ti, TPathInv;
ofstream out_data;


//FUNCTION DECLARATIONS
double BirthRate ( double ) ;
int Initialize(double **pmat, double **textmat, int NCol); //Fills ParMat, initializes TExtMat, returns #Rows
void OneSim (double StartTime, double EndTime, int* State, bool StopOnErad);
double Rand () ;
vector<double> Seq(double, double, int); //Returns sequence
void Show(int i, double **arr, int NPars, int NCol); //Shows ith row of ParMat
int VaccFun ( int, int );
void WriteMat(double **arr, int NPars, int NCol, char* filename); 

//MAIN
int main()
{
  strcpy(FileNamePar, "Data/ParMat_");
  strcat(FileNamePar, SimName);  

  strcpy(FileNameTExt, "Data/TExtMat_");
  strcat(FileNameTExt, SimName);  

  srand ( time(NULL) );

  //Simulation parameters: OneSim writes data at time-intervals tick
  TMax=(double)10*365; dTime=0.001; NTrials=50; tick=(double)1; 

      int NCol = 12; //# columns (parameters)
      //Assign pointer to 2D array ParMat
      double **ParMat=new double*[NCol];
      //Assign pointer to 2D array TExtMat
      double **TExtMat = new double*[NTrials];

	int NPars = Initialize(ParMat, TExtMat, NCol);

      WriteMat(ParMat, NPars, NCol, FileNamePar); //Write ParMat
      
      for(int NPar = 0; NPar < NPars; NPar++) //Loop through parameters
	{

	  if(VerboseWriteFlag){
	    strcpy(DirName, "Data/");
	    strcat(DirName, SimName);  
	    mkdir(DirName, ACCESSPERMS);
	    strcat(DirName, "/");	  
	    strcpy(FileNameDat, DirName);
	    sprintf(FileSuffix, "NPar_%d",NPar);
	    strcat(FileNameDat, FileSuffix);
	    out_data.open(FileNameDat);
	    out_data << "time S Iv Ip V P N births deaths ninfv ninfp nrecv nrecp S_death Iv_death Ip_death V_death P_death\n"; //Write data
	  }

	  //Extract parameters
	  //Parmat[0] corresponds to NPar;
	  b0 = ParMat[1][NPar]; //b0
	  d = ParMat[2][NPar]; //d
	  Bp = ParMat[3][NPar]; //Bp
	  Nv = ParMat[4][NPar]; //Nv
	  tv = ParMat[5][NPar]; //tv
	  gamv = ParMat[6][NPar]; //gamv
	  gamp = ParMat[7][NPar]; //gamp
	  tb = ParMat[8][NPar]; //tb
	  T = ParMat[9][NPar]; //T
	  PInit = (int) ParMat[10][NPar]; // Initial level of pathogen upon invasion
	  TPathInv = ParMat[11][NPar]; // Time of pathogen invasion

	  for(int ntrial = 0; ntrial < NTrials; ntrial++)
	    {
	      State[0] = 1000; //S
	      State[1] = 0; //Iv
	      State[2] = 0; //Ip
	      State[3] = 0; //V
	      State[4] = 0; //P
	      //Simulate to quasi steady state (rewrites State)
	      OneSim(0, TPathInv, State, false);
	      
	      //Simulate invasion until TMax years, or pathogen extinction
	      State[2] = PInit;
	      OneSim(TPathInv, TMax, State, true);
	      
	      //Store final value of t in TExtMat
	      TExtMat[ntrial][NPar] = t;
 
	      cout << "Sim Trial: " << ntrial << endl;
	      
	    }//End loop through NTrials
	  if(VerboseWriteFlag){out_data.close();}
	  
	  cout << "*****************************" << endl;	  
	  cout << "Finished Parameter Set " << NPar+1 << " / " << NPars << endl;
	  cout << "*****************************" << endl;
	  
	}//End Loop through NPars

      //Write TExtMat
      WriteMat(TExtMat, NPars, NTrials, FileNameTExt); //Write ParMat


      //Remove ParMat
      for (int j=0; j<NCol; j++)
	delete [] ParMat[j];
      delete [] ParMat;
}//End Main

      
      
//////////////////////////////////////
///////// DEFINE FUNCTIONS ///////////
//////////////////////////////////////


void OneSim (double StartTime, double EndTime, int* State, bool StopOnErad = false)
{
  //Set initial conditions: No Pathogen, no vaccination
  S = State[0];
  Iv =State[1];
  Ip =State[2];
  V = State[3];
  P = State[4];
  Npop = S + Iv + Ip + V + P;

  ti = StartTime;
  t = StartTime;

  //Track births, deaths, recoveries, etc for error checking
  nbirths = 0;
  ndeaths = 0;
  ninfv = 0;
  ninfp = 0;
  nrecv = 0;
  nrecv = 0;
  S_death = 0;
  Iv_death = 0;
  Ip_death = 0;
  V_death = 0;
  P_death = 0;
  
  while(t < EndTime && (Ip > 0 || !StopOnErad)) {
        
    /*
      Update the state variables S, Ip, P
      as functions of the old state variables. 
      Event1: Birth:     b
      Event2: Death:     d*Npop
      Event3: P Infects S: Bp*S*Ip
      Event4: P Infects Iv: Bp*Iv*Ip
      Event4: V Recovery: gamv*Iv
      Event5: P Recovery:  gamp*Ip
      Event6: Vaccination: when t <= tv and t + dTime > tv
    */    

    //Get reaction rate for next time step
    b = BirthRate( t ) ;    

    Sum_rate = b + d*Npop + Bp*S*Ip + Bp*Iv*Ip + gamv*Iv + gamp*Ip ;
      
    //Determine whether an event occurs during this time step
      EventFlag = Rand() < Sum_rate*dTime;

    Picker = Rand();
    Sum = 0;

    //Vaccination: this is assumed to be a sudden pulse
    if ( (fmod(t,T) <= tv) && (fmod(t + dTime,T) > tv ) )
      {
	//	cout << "vacc" << "\n";
	S_next = S - VaccFun(S,Npop);
	Iv_next = Iv + VaccFun(S,Npop);
	S = S_next;
	Iv = Iv_next;
	EventFlag = false;
      }
    
  if ( EventFlag )
  {
      //Event: Birth
    if (Picker < b / Sum_rate)
      {
	S_next = S + 1;
	Iv_next = Iv;
	Ip_next = Ip;
	V_next = V;
	P_next = P;
	Npop_next = Npop + 1;
	nbirths += 1;
      }    
    Sum += b;

    //Event: Death
    if (Sum / Sum_rate <= Picker && Picker < (Sum + d*Npop) / Sum_rate)
      {
	//Pick the host that dies
	RandDeath = Rand();
	//Host of class S dies
	if( RandDeath < (double) S/Npop)
	  {
	    S_next = S - 1;
	    Iv_next = Iv;
	    Ip_next = Ip;
	    V_next = V;
	    P_next = P;
	    S_death ++ ;
	  }
	else {//Host of class Iv dies
	  if(RandDeath < (double) (S + Iv)/Npop)
	    {
	      S_next = S;
	      Iv_next = Iv - 1;
	      Ip_next = Ip;
	      V_next = V;
	      P_next = P;
	      Iv_death ++ ;
	    }
	  else {//Host of class Ip dies
	    if(RandDeath < (double) (S + Iv + Ip)/Npop)
	      {
		S_next = S;
		Iv_next = Iv;
		Ip_next = Ip - 1;
		V_next = V;
		P_next = P;		
		Ip_death ++ ;
	      }
	    else{//Host of class V dies
	      if(RandDeath < (double) (S + Iv + Ip + V)/Npop)
		{
		  S_next = S;
		  Iv_next = Iv;
		  Ip_next = Ip;
		  V_next = V - 1;
		  P_next = P;
		  V_death ++ ;		  
		}
	      else{//Host of class P dies
		  S_next = S;
		  Iv_next = Iv;
		  Ip_next = Ip;
		  V_next = V;
		  P_next = P - 1;
		  P_death ++ ;		  
	      }
	    }
	  }
	}
	Npop_next = Npop - 1;
	ndeaths += 1;
      }
    Sum = Sum + d*Npop;

    //Event: Pathogen infection, Susceptible
    if (Sum / Sum_rate <= Picker && Picker < (Sum + Bp*Ip*S) / Sum_rate)
      {
	S_next = S - 1;
	Iv_next = Iv;
	Ip_next = Ip + 1;
	V_next = V;
	P_next = P;
	Npop_next = Npop;
	ninfp += 1;
      }
    Sum += Bp*Ip*S;

    //Event: Pathogen infection, Vaccine exposed
    if (Sum / Sum_rate <= Picker && Picker < (Sum + Bp*Ip*Iv) / Sum_rate)
      {
	S_next = S;
	Iv_next = Iv - 1;
	Ip_next = Ip + 1;
	V_next = V;
	P_next = P;
	Npop_next = Npop;
	ninfp += 1;
      }
    Sum += Bp*Ip*Iv;

      //Event: Vaccine recovery 
    if ( Sum / Sum_rate <= Picker && Picker <= (Sum + gamv*Iv) / Sum_rate )
      {
	S_next = S;
	Iv_next = Iv - 1;
	Ip_next = Ip;
	V_next = V + 1;
	P_next = P;
	Npop_next = Npop;
	nrecv += 1;
      }
    Sum += gamv*Iv;
    
      //Event: Pathogen recovery
    if ( Sum / Sum_rate <= Picker && Picker <= (Sum + gamp*Ip) / Sum_rate )
      {
	S_next = S;
	Iv_next = Iv;
	Ip_next = Ip - 1;
	V_next = V;
	P_next = P + 1;
	Npop_next = Npop;
	nrecp += 1;
      }
        
    //Update current state and time
    S = S_next;
    Iv = Iv_next;
    Ip = Ip_next;
    V = V_next;
    P = P_next;
    Npop = Npop_next;
      
  } // End If (EventFlag)
    
    //Write old values
    if( t >= ti && VerboseWriteFlag)
      {
  out_data << t << " " << S << " " << Iv << " " << Ip << " " << V << " " << P  << " " << Npop << " " << nbirths << " " << ndeaths <<  " " << ninfv << " " << ninfp << " " << nrecv << " " << nrecp << " " << S_death << " " << Iv_death << " " << Ip_death << " " << V_death << " " << P_death << "\n"; 
	ti += tick;
	nbirths = 0;
	ndeaths = 0;
      }
  
    t += dTime;
  }//End While

  if(VerboseWriteFlag){
    out_data << t << " " << S << " " << Iv << " " << Ip << " " << V << " " << P  << " " << Npop << " " << nbirths << " " << ndeaths <<  " " << ninfv << " " << ninfp << " " << nrecv << " " << nrecp << " " << S_death << " " << Iv_death << " " << Ip_death << " " << V_death << " " << P_death << "\n"; 
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
  double unif;
  unif = 0;
  while (unif == 0)
    {
      unif = (double) rand() / RAND_MAX ;
   }
  return unif;
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
int VaccFun( int S, int Npop) {
  int result;
  int numvacc;
  double temp;

  if(Npop==0 || S==0)
    {
      temp = 0;
    }else{
    temp  = (double) Nv*S/(Npop) ;
  }
  numvacc = round( temp );
  result = min( numvacc , S ) ;
  return result;
}



//************************************
//function to Initialize values of 2D array
int Initialize(double **pmat, double **textmat, int NCol)
{
  //Choose variable values to fill array with
  vector<double> tvVals;
  vector<double> TPathInvVals;
  vector<double> BpVals; double bpvals[] = {0.00001, 0.00005, 0.0001};
  vector<int> PInitVals; int pinitvals[]={1, 5, 10};

  tvVals = Seq(1, 365, 13);
  TPathInvVals = Seq(5*365 + 1, 6*365, 13);

  BpVals.assign(bpvals, bpvals + 13);
  PInitVals.assign(pinitvals, pinitvals + 13);

  int NPars = tvVals.size()*TPathInvVals.size()*BpVals.size()*PInitVals.size();

  //Allocate memory for the array ParMat
  for(int j=0; j<NCol; j++)
    {
      pmat[j]=new double[NPars];
    }

  //Allocate memory for array TExtMat
  for(int j=0; j<NTrials; j++)
    {
      textmat[j]=new double[NPars];
    }
  
  //Fixed parameters
  int i = 0;
  for(int i1=0; i1<tvVals.size(); i1++)
    for(int i2=0; i2<BpVals.size(); i2++)
      for(int i3=0; i3<TPathInvVals.size(); i3++)
	for(int i4=0; i4<PInitVals.size();i4++)
	  {
	    pmat[0][i] = i; //NPar
	    pmat[1][i] = 100.0;   //b0
	    pmat[2][i] = 0.004; //d
	    pmat[3][i] = BpVals[i2]; //Bp
	    pmat[4][i] = 5000.0; //Nv
	    pmat[5][i] = tvVals[i1]; //tv
	    pmat[6][i] = 0.007; //gamv
	    pmat[7][i] = 0.007; //gamp
	    pmat[8][i] = 90.0; //tb
	    pmat[9][i] = 365.0; //T
	    pmat[10][i] = (double) PInitVals[i4]; //PInit
	    pmat[11][i] = TPathInvVals[i3]; //TPathInv
	    i++;
	  }
  return i; //Return total number of rows
}

//************************************
//function to write values of 2D array
void WriteMat(double **arr, int NPars, int NCol, char*filename)
{
  out_data.open(filename);
  //out_data << "NPar b0 d Bp Nv tv gamv gamp tb T PInit TPathInv\n"; //Write header
  int i,j;
  for(i=0; i<NPars;i++)
    {
      for(j=0; j<NCol; j++)
	{
	  out_data << arr[j][i] << " "; //Write data
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
