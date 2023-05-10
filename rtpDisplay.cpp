#include <bits/stdc++.h>
#include "RTP.cpp"
using namespace std;

int sz, prevSz = 8 * 52 + 1;
int avail[11];

int main(){
    player::buildHands();
    RTP::cnt = 0;

    while(1){
        ifstream fin("shoeDistribution.txt");
        fin >> sz;
        if(prevSz != sz){
            prevSz = sz;
            for(int i = 0; i < 11; i++)
                fin >> avail[i];
            cout << "Remaining shoe size: " << sz << "\n";
            cout << "Shoe distribution: ";
            for(int i = 1; i <= 10; i++)
                cout << avail[i] << " ";
            cout << "\n";
            cout << "True Hi-Lo count: " << RTP::trueHighLow(sz, avail) << "\n";
            double rtp = RTP::calcRTP(sz, avail);
            cout << "RTP: " << 100 * rtp << "%\n\n";
            fin.close();
        }
        fin.close();
    }
}
