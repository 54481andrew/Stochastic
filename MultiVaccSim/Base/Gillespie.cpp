
/*
Build a simple c++ script that will 
simulate a stochastic SIR epidemiological
model. The simulation procedes according
to the Gillespie algorithm.  

*/

#include <iostream> // input/output cout, cin
#include <math.h> // Math functions
#include <stdlib.h>
#include <time.h>
#include <fstream> //Used to open files to which data are saved
<<<<<<< HEAD:MultiVaccSim/Gillespie.cpp
#include <string>
#include <vector>
=======
#include <vector> 
>>>>>>> origin/AddParMatInit:MultiVaccSim/Base/Gillespie.cpp

using namespace std;


//VARIABLES
bool EventFlag;
<<<<<<< HEAD:MultiVaccSim/Gillespie.cpp
int S_next, S, Iv_next, Iv, Ip_next, Ip, V_next, V, P_next, P, Npop_next, Npop,
  nbirths, ndeaths, ninfv, ninfp, nrecv, nrecp, S_death, Iv_death, Ip_death,
  V_death, P_death, NVacc, NTrials, NVals, NPar, NPars;
std::string filepre = "npar_";
double b0, tb, T, tv, b, gamv, R0p, Bp, gamp, d, Sum_rate, UniSeed, RandDeath,
  Picker, Sum, dTime, t, Tmax, tick, ti;
=======
int S_next, S, Iv_next, Iv, Ip_next, Ip, V_next, V, P_next, P, Npop_next, Npop, nbirths, ndeaths, ninfv, ninfp, nrecv, nrecp, S_death, Iv_death, Ip_death, V_death, P_death, Nv, NTrials, NPar;
char FileNamePar[50] = "Data/ParMat";
char FileNameDat[50]= "Data/DatMat";
double b0, tb, T, tv, b, gamv, R0p, Bp, gamp, d, Sum_rate, UniSeed, RandDeath, Picker, Sum, dTime, t, Tmax, tick, ti;
const double pi = 3.1415926535;
>>>>>>> origin/AddParMatInit:MultiVaccSim/Base/Gillespie.cpp

ofstream out_data;

//FUNCTION DECLARATIONS
double BirthRate ( double ) ;
int Initialize(double **arr, int NCol); //Fills ParMat, returns #Rows
void OneSim (int, int, int, int, int) ;
<<<<<<< HEAD:MultiVaccSim/Gillespie.cpp
vector<double> Seq(double , double , int );



  //************//
  // PARAMETERS //
  //************//
  /*
  b0 ~ birth rate during birth period
  d  ~ death rate

  
  Nv ~ number of vaccines used
  tv ~ time of vaccination

  tb ~ duration of birth period
  T  ~ interval between birth periods
  
  */
=======
double Rand () ;
vector<double> Seq(double, double, int);//Returns sequence
void Show(int i, double **arr, int NRow, int NCol); //Shows ith row of ParMat
int VaccFun ( int, int ) ;
void WriteParMat(double **arr, int NRow, int NCol); 
>>>>>>> origin/AddParMatInit:MultiVaccSim/Base/Gillespie.cpp

//MAIN
int main()
{
<<<<<<< HEAD:MultiVaccSim/Gillespie.cpp
  //Set simulation parameters
  Tmax = (double) 5*365; //Maximum simulation time
  dTime = 0.001; //Time increment
  NTrials = 25; //Perform the Gillespie simulation NTrials times for each parameter set
  NVals = 10; //Number of parameter values to loop through
  tick = 7;   //Write data after each (simulation) time interval tick
  
  //Choose parameters that are VARIED. 
  vector<double> TvVals (NVals);
  TvVals = Seq(1, 364, NVals);
  
  NPars = TvVals.size();            
  srand ( time(NULL) );
  
  //Convert parameter set to char, save text file
  for(NPar = 0; NPar < NPars ; NPar++)
    {
      //Choose fixed parameters: Set biological parameters
      b0 = 400;
      tb = 90;
      T  = 365;
      d = 0.004;
      
      //VACCINATION
      NVacc = 5000;
      gamv = 0.07;
      tv = TvVals[NPar];
      
      //PATHOGEN
      Bp = 0.0000005;
      gamp = 0.005;
      R0p = 0;

      char FileName[50];
      sprintf(FileName, "Data/NPar_%d",NPar);

      out_data.open(FileName);
      out_data << "time S Iv Ip V P N births deaths ninfv ninfp nrecv nrecp S_death Iv_death Ip_death V_death P_death\n"; //Place these names in the open file

      cout << "************" << endl;
      cout << "Parameter: " << NPar << endl;
      cout << "************" << endl;
      
      for(int ntrial = 0; ntrial < NTrials; ntrial++)
	{	  
	  //Set initial conditions  
	  t = 0.0;
	  S = (int) 1000 ; 
	  Iv = (int) 0;
	  Ip = (int) 100;
	  P = (int) 0;
	  V = (int) 0;
	  
	  OneSim(S, Iv, Ip, P, V);
	  
	  cout << "Sim Trial: " << ntrial << endl;
	  
	}//End loop through NTrials
      out_data.close();
    }//End loop through NPars     
=======

  srand ( time(NULL) );

      //Simulation parameters
      Tmax = (double) 10*365; dTime = 0.001; NTrials = 5;
      int NCol = 10; //# columns (parameters)
      //Assign pointer to 2D array
      double **ParMat=new double*[NCol];
      //Fill 2D array with values
      int NRow = Initialize(ParMat, NCol);
      WriteParMat(ParMat, NRow, NCol); //Write ParMat
      
      for(int NPar = 0; NPar < NRow; NPar++) //Loop through parameters
	{
	  //Extract parameters
	  b0 = ParMat[1][NPar]; //b0
	  d = ParMat[2][NPar]; //d
	  Bp = ParMat[3][NPar]; //Bp
	  Nv = ParMat[4][NPar]; //Nv
	  tv = ParMat[5][NPar]; //tv
	  gamv = ParMat[6][NPar]; //gamv
	  gamp = ParMat[7][NPar]; //gamp
	  tb = ParMat[8][NPar]; //tb
	  T = ParMat[9][NPar]; //T

	  //Write data (1st row)
	  sprintf(FileNameDat, "Data/NPar_%d", NPar);
	  out_data.open(FileNameDat);
	  out_data << "time S Iv Ip V P N births deaths ninfv ninfp nrecv nrecp S_death Iv_death Ip_death V_death P_death\n"; //Write data
	  
	  for(int ntrial = 0; ntrial < NTrials; ntrial++)
	    {
	      
	      //Set initial conditions  
	      t = 0;
	      S = (int) 1000 ; 
	      Iv = (int) 0;
	      Ip = (int) 100;
	      P = (int) 0;
	      V = (int) 0;
	      
	      OneSim(S, Iv, Ip, V, P);
	      
	      cout << "Sim Trial: " << ntrial << endl;
	      
	    }//End loop through NTrials
	  out_data.close();
	  
	  cout << "*****************************" << endl;	  
	  cout << "Finished Parameter Set " << NPar+1 << " / " << NRow << endl;
	  cout << "*****************************" << endl;
	  
	}//End Loop through NPars
      //Remove ParMat
      for (int j=0; j<NCol; j++)
	delete [] ParMat[j];
      delete [] ParMat;
>>>>>>> origin/AddParMatInit:MultiVaccSim/Base/Gillespie.cpp
}//End Main

      
      
//////////////////////////////////////
///////// DEFINE FUNCTIONS ///////////
//////////////////////////////////////


void OneSim (int S, int Iv, int Ip, int V, int P)
{
<<<<<<< HEAD:MultiVaccSim/Gillespie.cpp
  
  Npop = S + Iv + Ip + P + V;
  
  //Define Structure for storing data
  
  //  cout << "d: " << d << "\n";
  //cout << "S0: " << S << "; Iv0: " << Iv << "; Ip0: " << Ip << "; V0: " << V << "; P0: " << P << "\n" << "\n"; 
  
  ti = 0; // Works with tick
  
=======

  Npop = S + Iv + Ip + V + P;
    
  tick = 1; //This program writes data after each time interval tick
  ti = 0;   // Works with tick

>>>>>>> origin/AddParMatInit:MultiVaccSim/Base/Gillespie.cpp
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
  
  while(t < Tmax && Ip > 0) {
        
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
    if( t >= ti )
      {
  out_data << t << " " << S << " " << Iv << " " << Ip << " " << V << " " << P  << " " << Npop << " " << nbirths << " " << ndeaths <<  " " << ninfv << " " << ninfp << " " << nrecv << " " << nrecp << " " << S_death << " " << Iv_death << " " << Ip_death << " " << V_death << " " << P_death << "\n"; 
	ti = ti + tick;
	nbirths = 0;
	ndeaths = 0;
      }
      
    t += dTime;

  }//End While

}//End OneSim



//Birth function
 double BirthRate( double time ) {
   //Local function declaration
   double result;
   double modtime;
   modtime = fmod(time, T ) ;
   result = b0*(modtime < tb) ;
   return result;
 }

//Random number function
double Rand (){
  double unif;
  unif = 0;
  while (unif == 0)
    {
      unif = (double) rand() / RAND_MAX ;
   }
  return unif;
}  

<<<<<<< HEAD:MultiVaccSim/Gillespie.cpp
//Sequence function
vector<double> Seq(double minval, double maxval, int lengthval) {
  vector<double> vec(lengthval);
  for(int seqindex = 0; seqindex < lengthval; seqindex++)
    {
      vec[seqindex] = minval + (double) seqindex/(lengthval-1)*(maxval-minval);\

    }
  return vec;
}
=======
//Birth function
 double BirthRate( double time ) {
   //Local function declaration
   double result;
   double modtime;
   modtime = fmod(time, T ) ;
   result = b0*(modtime < tb) ;
   return result;
 }
>>>>>>> origin/AddParMatInit:MultiVaccSim/Base/Gillespie.cpp

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

<<<<<<< HEAD:MultiVaccSim/Gillespie.cpp


=======
//************************************
//function to Initialize values of 2D array
int Initialize(double **arr, int NCol)
{
  //Choose variable values to fill array with
  vector<double> tvVals;
  vector<double> BpVals;
  tvVals = Seq(1, 364, 25);
  BpVals = Seq(0.000005,0.00001, 1);
  int NRow = (int)tvVals.size()* (int)BpVals.size();

  //Allocate memory for the array
  for(int j=0; j<NCol; j++)
    {
      arr[j]=new double[NRow];
    }
  //Fixed parameters
  int i;

  for(int i1=0; i1<tvVals.size(); i1++)
    {
      for(int i2=0; i2<BpVals.size(); i2++)
	{
	  i = i1*BpVals.size() + i2;
	  arr[0][i] = i; //NPar
	  arr[1][i] = 100.0;   //b0
	  arr[2][i] = 0.004; //d
	  arr[3][i] = BpVals[i2]; //Bp
	  arr[4][i] = 5000.0; //Nv
	  arr[5][i] = tvVals[i1]; //tv
	  arr[6][i] = 0.007; //gamv
	  arr[7][i] = 0.007; //gamp
	  arr[8][i] = 300.0; //tb
	  arr[9][i] = 365.0; //T
	}
    }
  return i+1; //Return total NUMBER of rows (hence +1)
}

//************************************
//function to write values of 2D array
void WriteParMat(double **arr, int NRow, int NCol)
{
  out_data.open(FileNamePar);
  out_data << "NPar b0 d Bp Nv tv gamv gamp tb T\n"; //Write header
  int i,j;
  for(i=0; i<NRow;i++)
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
>>>>>>> origin/AddParMatInit:MultiVaccSim/Base/Gillespie.cpp
