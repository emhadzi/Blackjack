#include <bits/stdc++.h>
using namespace std;

int main(){
    ifstream fin("winnings.txt");
    double real = 0, exp = 0;
    char c;
    fin >> c;
    while(!fin.eof()){
        double a, b;
        fin >> a >> b;
        real += a;
        exp += b;
    }
    cout << real << " " << exp << endl;
}