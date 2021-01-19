This is a program which detects a vowel when an input voice(in .txt or recording module) format is given as input.
In this program I have implemented several functions for modularity such as 
1) void durbin() : This function takes the pre computed Ris (from the Ris() function) and calculates the Ais, Cis for the given Ris. In this function I have also implemented Raised sine window on the Cis.

2) void Wis() : This function stores the Wis value given by sir in a vector and returns it.

3) vector Ris() : This function takes the input file in .txt format as input and then computes the Ris.

4) vector energy(): This function is used for computing stable frames in the samples by computing the short term energy and then taking the frames which are besides the maximum energy frame.

5) vector ProcessData(): This is the most important function as it takes all the Ris of the 5 stable frames as parameter and everytime calculates the 12 Cis for all the 5 frames and stores in a vector of size 60.

6) int main(): In this function, I have first trained and then tested. First, I have stored the file names in the ref matrix. Now for every file, I have first removed the DC Shift(if any), applied the normalization, 
and further applied the hamming window. After doing this I have then calculated the called the energy() function and found out the 5 stable frames along with thier frequencies.

After this, for every stable frame, Ris are being calculated and stored in the vectors. After calculating the Ris, now we move ahead to calculate the final Cis of the 5 stable frames.
These Cis of the stable frames is stored in a single dimension vector. After this, we then find the average of the Cis and store it in the respective vector.

The same process we repeat for the test data of calculating Ais, Ris, Cis, Raised sin window and dividing the samples in to 5 stable frames and calculating the final values.
Now for each file we calculate the tokhura's distance and store it in a vector.
After storing into the vector we find the Cis match which has the minimum distance from the training data.

The vowel with minimum distance is given as output.
The above program gives around 92% accuracy for the test data provided as input.