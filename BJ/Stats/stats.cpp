#include <bits/stdc++.h>
using namespace std;

int main(){
    double n = 0, rtp, used, m1, m2, mx;
    int sz[]{14408, 14331, 14355, 14344, 14361};

    for(int t = 1; t <= 5; t++){
        cout << t <<" ";
        string fileName = "rtpLog";
        fileName += ('0' + t);
        fileName += ".txt";
        ifstream fin(fileName);
        //ifstream fin(fileName);
        for(int i = 0; i < sz[t-1]; i++, n++){
            fin >> used >> rtp;
            //cout << rtp << " ";
            mx = max(mx, rtp);
            m1 += max(rtp - 1, 0.0);
            if(rtp > 1)
                m2 += (rtp - 1) * (rtp - 1);
        }
        cout << "Done with " << fileName << "\n";
        fin.close();
    }
    cout << "N: " << n << endl;
    cout << "M1: " << setprecision(10) << m1 / n << endl;
    cout << "M2: " << setprecision(10) << m2 / n << endl;
    cout << "mx: " << mx;
}