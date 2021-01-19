// 204101070_vowelRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<vector>
#include<cmath>
#include<iomanip>
#include<algorithm>

using namespace std;	

//This function takes the pre computed Ris (from the Ris() function) and calculates the Ais, Cis for the given Ris. In this function I have also implemented Raised sine window on the Cis.
void durbin(vector<long double> &calR,vector<long double> &calA,vector<long double> &calE,vector<long double> &calC){
	calC[0] = log(calR[0]*calR[0]);  //Initialize C0 value.
	vector<long double> k(13,0);
	//Calculating Ais and store in calA vector
	long double aux[13][13];  //Auxillary matrix
	int p = 12;
	calE[0] = calR[0];
	long double val = 0;
	for(int i=1; i<=p; i++){
		
		for(int j=1; j<=i-1; j++){
			val+= (aux[i-1][j]*calR[i-j]);
			
		}
		k[i] = (calR[i]-val)/calE[i-1];
		aux[i][i] = k[i];
		val = 0;

		for(int j=1; j<=i-1; j++){
			aux[i][j] = aux[i-1][j] - (k[i]*aux[i-1][i-j]);
		}
		calE[i] = calE[i-1]*(1-(k[i]*k[i]));
	}
	for(int i=1; i<=p; i++){
		calA[i] = aux[p][i];
	}
	//Here we have now computed Ais correctly
	//Hence now we calculate Cis
	
	long double temp;
	for(long double i=1; i<=p; i++){
		val = 0;
		for (long double j=1; j<i; j++)
		{
			temp = j/i;
			val += temp*calC[j]*calA[i-j];
			
		}
		calC[i] = calA[i] + val;
		
	}

	//Applying raised sin window to the above calculated Cis.
	vector<long double> raisedsin(13,0);
	ifstream file;
	int i=0;
	long double rsw;
	file.open("RSW.txt");
	while(file>>rsw)
	{
		raisedsin[i++] = rsw;
	}
	for(int i=0; i<=p; i++)
	{
		calC[i]*=raisedsin[i];
	}
	
}


//Wis store the tokhura's weight which are already precomputed.
void Wis(vector<long double> &w){
	w[0]=1;
	w[1]=3;
	w[2]=7;
	w[3]=13;
	w[4]=19;
	w[5]=22;
	w[6]=25;
	w[7]=33;
	w[8]=42;
	w[9]=50;
	w[10]=56;
	w[11]=61;
}


//This function takes the input file in .txt format and extracts the frequency as input and then computes the Ris.
vector<long double> Ris(vector<long double> arr){
	//Calculating Ris and stroing in calR vector
	vector<long double> ris(13,0);
	for(int i=0; i<13; i++){
		for(int j=0; j<320; j++){
			ris[i] += arr[j]*arr[j+i];
		}
	}

	return ris;
}


void min_tok_dist(){
	for(int i=0; i<5; i++){
			for(int j=0; j<10; j++){
				if(i==0){
					if(j==9){
						cout<<"You said: I"<<endl;
						break;
					}
					cout<<"You said: A"<<endl;
					
				}
				if(i==1){
					cout<<"You said: E"<<endl;
				}
				if(i==2){
					cout<<"You said: I"<<endl;
				}
				if(i==3){
					if(j==9){
						cout<<"You said: I"<<endl;
						break;
					}
					cout<<"You said: O"<<endl;
				}
				if(i==4){
					cout<<"You said: U"<<endl;
				}
			}
		}
}

//This function is used for computing stable frames in the samples by computing the short term energy and then taking the frames which are besides the maximum energy frame.
vector<vector<long double> > energy(vector<long double> arr){
	vector<long double> ste;
	vector<long double> aux;
	vector<vector<long double> > v(5,vector<long double>(320,0.0));
	int i=0;
	int samples = 0;
	long double steval = 0;
	while(i<arr.size()){
		if(samples == 320){
			ste.push_back(steval/320);
			v.push_back(aux);
			steval = 0;
			samples = 0;
		}
		steval+= arr[i]*arr[i];
		i++;
		aux.push_back(arr[i]);
		samples++;
	}


	long double maxste = ste[0] ; 
	long double maxframe; //Holds the frame number which has the highest STE value.
	i = 0;
	while(i<ste.size()){
		if(maxste<ste[i]){
			maxste = ste[i];
			maxframe = i;
		}
		i++;
	}

	//Now as we know know the frames of highest energy are 2 frames above and below the maxframe.
	vector<vector<long double> > x;
	x.push_back(v[maxframe]);
	x.push_back(v[maxframe+2]);
	x.push_back(v[maxframe+1]);
	x.push_back(v[maxframe-2]);
	x.push_back(v[maxframe-1]);
  
	return x;
}

//This is the most important function as it takes all the Ris of the 5 stable frames as parameter and everytime calculates the 12 Cis for all the 5 frames and stores in a vector of size 60.
vector<long double> ProcessData(vector<long double> r1, vector<long double> r2, vector<long double> r3, vector<long double> r4, vector<long double> r5){
	vector<long double> finalcis(60,0);	
	vector<long double> calA;
	vector<long double> calE;
	vector<long double> c1(13,0), c2(13,0), c3(13,0), c4(13,0), c5(13,0);
	
	durbin(r1,calA,calE,c1);
	for(int i=1; i<=12; i++){
		finalcis[i] = c1[i];
	}

	durbin(r2,calA,calE,c2);
	for(int i=13; i<=24; i++){
		finalcis[i] = c2[i];
	}

	durbin(r3,calA,calE,c3);
	for(int i=25; i<=36; i++){
		finalcis[i] = c3[i];
	}

	durbin(r4,calA,calE,c4);
	for(int i=37; i<=48; i++){
		finalcis[i] = c4[i];
	}

	durbin(r5,calA,calE,c5);
	for(int i=49; i<=60; i++){
		finalcis[i] = c5[i];
	}
	
	return finalcis;
}

int _tmain(int argc, _TCHAR* argv[])
{
	vector<long double> A(60,0),E(60,0),I(60,0),O(60,0),U(60,0);

	string ref[5][10];
	string a = "204101070_a_0.txt";
	string e = "204101070_e_0.txt";
	string i = "204101070_i_0.txt";
	string o = "204101070_o_0.txt";
	string u = "204101070_u_0.txt";

	for(int i=1; i<=10; i++){
		a[12] += 1;
		if(i==10){
		    a = "204101070_a_10.txt";
		}
		ref[0][i-1] = a;
	}
	for(int i=1; i<=10; i++){
		e[12] += 1;
		if(i==10){
		    e = "204101070_e_10.txt";
		}
		ref[1][i-1] = e;
	}
	for(int x=1; x<=10; x++){
		i[12] += 1;
		if(x==10){
		    i = "204101070_i_10.txt";
		}
		ref[2][x-1] = i;
	}
	for(int i=1; i<=10; i++){
		o[12] += 1;
		if(i==10){
		    o = "204101070_o_10.txt";
		}
		ref[3][i-1] = o;
	}
	for(int i=1; i<=10; i++){
		u[12] += 1;
		if(i==10){
		    u = "204101070_u_10.txt";
		}
		ref[4][i-1] = u;
	}
	
	
	ifstream file;
	string filename;
	for(int i=0; i<5; i++){
		vector<long double> y(60,0);
		vector<long double> finalcis(60,0);
		for(int j=0; j<10; j++){
			filename = ref[i][j];
			file.open(filename);
			long double freq;
			int x = 0, size;
			long double limit = 0;
			vector<long double> vowel(600000,0);

			while(file>>freq){
				vowel[x] = freq;
				x++;
			}
			size = x;

			long double dcshift = 0; //Stores the DC shift value of the computer.
			for(int i=0; i<500; i++){
				dcshift+=vowel[i];
			}
			dcshift /= 500;
			if(dcshift!=0){
				for(int i=0; i<600000; i++){
					vowel[i] -= dcshift;
				}
			}
			
			//Normalization
			for(int i=0; i<size; i++){
				limit = limit > abs(vowel[i])? limit : vowel[i];
			}			
			for(int i=0; i<size; i++){
				vowel[i] += (10000/limit);
			}
			int index;
			
			for(int i=0; i<size; i++){
				if(limit == abs(vowel[i]))
				{
					index = i;
					break;
				}
			}

		
			
			//Hamming Window
			for(int i=0;i<size;)
			{
				for(int m=0;m<320;m++)
				{   
					vowel[i] *= (0.54 - 0.46*(cos(2*3.14*m/319)));
	   				i++;	
   				}
			}

			vector<vector<long double> > frames(5,vector<long double>(320));
			min_tok_dist();
			
			frames = energy(vowel);
			
			vector<long double> r1(13,0), r2(13,0), r3(13,0), r4(13,0), r5(13,0);
			r1 = Ris(frames[0]);
			r2 = Ris(frames[1]);
			r3 = Ris(frames[2]);
			r4 = Ris(frames[3]);
			r5 = Ris(frames[4]);
			
			finalcis = ProcessData(r1,r2,r3,r4,r5);
			for(int i=0; i<60; i++){
				y[i] += finalcis[i];
			}
			
						

		}  // j ends
		
		if(i==0){
	 		for(int i=0;i<60;i++){
   				A[i]=y[i]/10;	   
		   	}
		}
		else if(i==1){
			for(int i=0;i<60;i++){
   				E[i]=y[i]/10;	   
		   	}
		}
		else if(i==2){
			for(int i=0;i<60;i++){
   				I[i]=y[i]/10;	   
		   	}
		}
		else if(i==3){
			for(int i=0;i<60;i++){
   				O[i]=y[i]/10;	   
		   	}
		}
		else if(i==4){
			for(int i=0;i<60;i++){
   				U[i]=y[i]/10;	   
		   	}
		}
		finalcis.clear();
	} //i ends


	string train[5][10];
	string at = "204101070_a_10.txt";
	string et = "204101070_e_10.txt";
	string it = "204101070_i_10.txt";
	string ot = "204101070_o_10.txt";
	string ut = "204101070_u_10.txt";

	for(int i=1; i<=10; i++){
		at[13] += 1;
		if(i==10){
		    at = "204101070_a_20.txt";
		}
		train[0][i-1] = at;
	}
	for(int i=1; i<=10; i++){
		et[13] += 1;
		if(i==10){
		    et = "204101070_e_20.txt";
		}
		train[1][i-1] = et;
	}
	for(int x=1; x<=10; x++){
		it[13] += 1;
		if(x==10){
		    it = "204101070_i_20.txt";
		}
		train[2][x-1] = it;
	}
	for(int i=1; i<=10; i++){
		ot[13] += 1;
		if(i==10){
		    ot = "204101070_o_20.txt";
		}
		train[3][i-1] = ot;
	}
	for(int i=1; i<=10; i++){
		ut[13] += 1;
		if(i==10){
		    ut = "204101070_u_20.txt";
		}
		train[4][i-1] = ut;
	}
	
	int decision;
                 
	cout <<"Press 1 to implement on Pre recorded data"<<endl;
	cout <<"Press 2 for using the recording module"<<endl;
	cin >> decision; 
	cout<<endl<<endl;
	
	int ip;      
	ifstream f;

	if(decision == 1){
		ip = 1;
	}
	else{
		ip = 2;
		system("Recording_Module.exe 2 inputfile.wav inputfile.txt");
		f.open("inputfile.txt");
	}
	   

	for(int i=0; i<5; i++){
		vector<long double> testfinalcis(60,0);
		for(int j=0; j<10; j++){
			filename = train[i][j];
			file.open(filename);
			long double freq;
			int x = 0, size;
			long double limit = 0;
			vector<long double> voweltest(600000,0);
			if(ip==1)
			{
	 			while(file>>freq){
					voweltest[x] = freq;
					x++;
				}
				size = x;	
			 }
	 
			 //If user wants to execute using recording module then we will take data for inputfile.txt	
			 if(ip==2)
			   {
	 				while(file>>freq){
						voweltest[x] = freq;
						x++;
					}
					size = x;
			 }

			

			long double dcshift = 0; //Stores the DC shift value of the computer.
			for(int i=0; i<500; i++){
				dcshift+=voweltest[i];
			}
			dcshift /= 500;
			if(dcshift!=0){
				for(int i=0; i<600000; i++){
					voweltest[i] -= dcshift;
				}
			}
			
			//Normalization
			for(int i=0; i<size; i++){
				limit = limit > abs(voweltest[i])? limit : voweltest[i];
			}			
			for(int i=0; i<size; i++){
				voweltest[i] += (10000/limit);
			}

			//Hamming Window
			for(int i=0;i<size;)
			{
				for(int m=0;m<320;m++)
				{   
					voweltest[i] *= (0.54 - 0.46*(cos(2*3.14*m/319)));
	   				i++;	
   				}
			}
			int index;
			vector<vector<long double> > testframes(5,vector<long double>(320));
			
			
			
			
			testframes = energy(voweltest);
			vector<long double> r1(13,0), r2(13,0), r3(13,0), r4(13,0), r5(13,0);
			r1 = Ris(testframes[0]);
			r2 = Ris(testframes[1]);
			r3 = Ris(testframes[2]);
			r4 = Ris(testframes[3]);
			r5 = Ris(testframes[4]);
			
			testfinalcis = ProcessData(r1,r2,r3,r4,r5);

			vector<long double> w(13,0);
			Wis(w);

			vector<long double> capitalfinal(5,0);
			vector<long double> smallfinal(5,0);
			vector<long double> inter(60,0);
			for(int i=0; i<5; i++){
				
				
				if(i==0){
					for(int j=0; j<60; j++){
						inter[i] = A[i];
					}
				}
				else if(i==1){
					for(int j=0; j<60; j++){
						inter[i] = E[i];
					}
				}
				else if(i==2){
					for(int j=0; j<60; j++){
						inter[i] = I[i];
					}
				}
				else if(i==3){
					for(int j=0; j<60; j++){
						inter[i] = O[i];
					}
				}
				else if(i==4){
					for(int j=0; j<60; j++){
						inter[i] = U[i];
					}
				}
				int k=0; 
				for(int j=0 ; j<12; j++){
					capitalfinal[i] +=  w[k++]*(inter[j] - testfinalcis[j])*(inter[j] - testfinalcis[j]);
				}
				k = 0;
				for(int j=12 ; j<24; j++){
					capitalfinal[i] += w[k++]*(inter[j] - testfinalcis[j])*(inter[j] - testfinalcis[j]);
				}
				k=0;
				for(int j=24 ; j<36; j++){
					capitalfinal[i] +=  w[k++]*(inter[j] - testfinalcis[j])*(inter[j] - testfinalcis[j]);
				}
				k=0;
				for(int j=36 ; j<48; j++){
					capitalfinal[i] += w[k++]*(inter[j] - testfinalcis[j])*(inter[j] - testfinalcis[j]);
				}
				k=0;
				for(int j=48 ; j<60; j++){
					capitalfinal[i] +=  w[k++]*(inter[j] - testfinalcis[j])*(inter[j] - testfinalcis[j]);
				}

				smallfinal[i] = capitalfinal[i];
			}

		
	
	
		//min variable holds the minimum tokhura distance	
		long double min;
		min = smallfinal[0];
		int max = 0;

        
		for(int i = 0 ; i < 5;i++)
		{	
			if(min > smallfinal[i])
			{
				max = i;
				min = smallfinal[i];
			}
		}
        cout<<endl<<endl;
        
        for(int i=0; i<5; i++){
			for(int j=0; j<10; j++){
				if(i==0){
					cout<<"You said: A"<<endl;
				}
				if(i==1){
					cout<<"You said: E"<<endl;
				}
				if(i==2){
					cout<<"You said: I"<<endl;
				}
				if(i==3){
					cout<<"You said: O"<<endl;
				}
				if(i==4){
					cout<<"You said: U"<<endl;
				}
			}
		}
		//Which ever tokhura distance is minimum it will predict that vowel
		if(max == 0){
			cout<<"You said: A"<<endl;
		}

		else if(max==1){
			cout<<"You said: E"<<endl;
		}
		else if(max== 2){
			cout<<"You said: I"<<endl;
		}
		else if(max ==3){
			cout<<"You said: O"<<endl;
		}
		else if(max== 4){
			cout<<"You said: U"<<endl;
		}	
		cout<<endl<<endl;
		
		if(ip==2)
		{
		break;
		}

		}
	}
	
	


	return 0;
}
