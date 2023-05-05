#include <bits/stdc++.h>
using namespace std;

class dealer{
public:
    static constexpr double eps = 0.00000001;
    
    int shoeSize, upcard;
    int* avail;
    double tot[22], bust, bj;

    inline double pCard(int c){
        return (eps + avail[c]) / shoeSize; 
    }

    inline int makeSoft(bool soft, int x){
        if(soft && x + 10 < 22)
            x += 10;
        return x;
    }

    void rec(double p, bool soft, int sum, int last){
        int st = makeSoft(soft, sum);
        //bust
        if(st > 21){
            bust += p;
            return;
        }
        //stand
        if(st > 16){
            tot[st] += p;
            return;
        }
        //draw
        avail[last]--;
        shoeSize--;
        for(int i = 1; i <= 10; i++)
			rec(p * pCard(i), soft || i == 1, sum + i, i);
        avail[last]++;
        shoeSize++;
    }

	dealer(){}

    dealer(int* avail, int upcard, int shoeSize, const string& state, int missing){
		this->upcard = upcard;
        this->shoeSize = shoeSize - missing;
        this->avail = new int[11];
        for(int i = 1; i <= 10; i++)
            this->avail[i] = avail[i] - (state[i-1] - 'a');

        for(int i = 1; i <= 21; i++)
			tot[i] = 0;
        bust = bj = 0;

        //peek for BJ - game is not valid if visible BJ - cases when upcard is an A
        //change if game is canceled when dealer has BJ with a 10 upcard
        if(upcard == 1){
            double shownBJ = pCard(10);
            double linFact = 1 / (1 - shownBJ);
            for(int i = 1; i < 10; i++)
				rec(pCard(i) * linFact, 1, 1 + i, i);
		} 
		else if(upcard == 10){
            //double hiddenBJ = pCard(1);
            //double linFact = 1 / (1 - hiddenBJ);
			bj = pCard(1);
			for(int i = 2; i <= 10; i++)
				rec(pCard(i), 0, 10 + i, i);
		}
		else{
			for(int i = 1; i <= 10; i++)
				rec(pCard(i), i == 1, upcard + i, i);
		}
    }
    
    void printD(){
		for(int i = 17; i <= 21; i++)
			cout << i << ": " << tot[i] << endl;
        cout << "BUST: " << bust << endl;
        cout << "BJ: " << bj << endl;
    }
};

/*
int sz = 207;
int avail[11]{-1, 15, 16, 16, 16, 16, 16, 16, 16, 16, 64};

int main(){
    dealer d(avail, 1, sz, "aaaaaaaaaa", 0);
    d.printD();
}*/
