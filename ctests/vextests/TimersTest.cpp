#include<string>
#include<iostream>

using namespace std;

	int main(int argc, char *argv[]) {	
		double temp = 0;
		int iterations = stoi(argv[1]); 
    time_t start;
    time_t end;
    time(&start);
		for (int i =0 ; i <iterations; i++) {
			temp += i;
		}
    time(&end);
		cout << temp << ": " << start << " " << end << endl;
	}
