#include <bits/stdc++.h>
#include "RTP.cpp"
using namespace std;

int sz;
int avail[11];

pair <bool, int> pl[2], dl;
bool db[2];
int p1, p2, d;

string action[]{"stand", "hit", "double", "split"};

void addCard(pair <bool, int>& x, int c){
    x.first |= (c == 1);
    x.second += c;
}

inline int makeSoft(bool soft, int x){
    if(soft && x + 10 < 22)
        x += 10;
    return x;
}

int draw(char side){
    cout << "Draw " << side << ": ";
    int c;
    cin >> c;
    sz--;
    avail[c]--;
    return c;
} 

int dealerDraw(){
    //if both bust dont draw
    if(pl[0].second > 21 && (pl[1].second == 0 || pl[1].second > 21)){
        if(pl[1].second == 0)
            return db[0] ? -1 : 0;
        return 1 + (db[0] ? -2 : -1) + (db[1] ? -2 : -1);
    }

    cout << "Dealer drawing: \n";
    int c = draw('d');
    addCard(dl, c);
    bool hiddenBJ = (d == 10 && c == 1);
    while(makeSoft(dl.first, dl.second) < 17)
        addCard(dl, draw('d'));

    bool split = pl[1].second != 0;
    int total = (split ? -1 : 0);
    if(hiddenBJ)
        return total;
    for(int ind = 0; ind < (split ? 2 : 1); ind++){
        int dTot = makeSoft(dl.first, dl.second), handWon;
        if(pl[ind].second > 21)
            handWon = 0;
        else if(dTot > 21 || dTot < makeSoft(pl[ind].first, pl[ind].second))
            handWon = 2;
        else if(dTot == makeSoft(pl[ind].first, pl[ind].second))
            handWon = 1;
        else 
            handWon = 0;

        if(db[ind])
            handWon = 2 * handWon - 1;
        total += handWon; 
    }
    return total;
}

void hand(int ind, int p1, int p2, bool canSplit){
    addCard(pl[ind], p1);
    addCard(pl[ind], p2);

    string startState = "aaaaaaaaaa";
    startState[p1-1]++; startState[p2-1]++;
    
    avail[p1]++; avail[p2]++;
    player pH(sz + 2, avail, d, canSplit);
    avail[p1]--; avail[p2]--;

    cout << "Recommended action: " << action[pH.getAct(startState)] << "(" << pH.getEV(startState) << ")" << "\n";
    int act;
    cin >> act;

    //stand
    if(act == 0){}
    //double
    else if(act == 2){
        db[ind] = 1;
        addCard(pl[ind], draw('p'));
    }

    //hit
    else if(act == 1){
        string cur = startState;
        while(act == 1){
            int card = draw('p');
            addCard(pl[ind], card);
            if(pl[ind].second > 21)
                break;
            cur[card - 1] = char(cur[card - 1] + 1);
            cout << "Recommended action: " << action[pH.getAct(cur)] << "(" << pH.getEV(cur) << ")" << "\n";
            cin >> act;
        }
    }
    //split
    else{
        for(int t = 0; t < 2; t++){
            cout << "Playing hand " << t + 1 << ":\n";
            cout << "Input second card of current hand:\n";
            pl[t] = {0, 0};
            p2 = draw('p');
            hand(t, p1, p2, 0);
        }
    }
}

double round(){
    bool insur = 0;
    db[0] = db[1] = 0;
    pl[0] = pl[1] = dl = {0, 0};

    cout << "Input p1, d, p2: \n";
    p1 = draw('p');
    d = draw('d');
    addCard(dl, d);
    p2 = draw('p');

    //ace dealer upcard
    if(d == 1){
        double insurEv = (1.5 * avail[10] / (sz + eps)) - 0.5;
        cout << "Recommended insurance: " << (insurEv > 0 ? "Yes (" : "No (") << insurEv << ")\n";
        cout << "Did the dealer show Blackjack?: ";
        bool dealerBJ = 0;
        cin >> dealerBJ;
        insur = insurEv > 0;
        if(dealerBJ){
            avail[10]--;
            sz--;
            if((p1 == 1 && p2 == 10) || (p1 == 10 && p2 == 1))
                return 1 + insur;
            return insur;
        }
    }
    //player blackjack
    if((p1 == 1 && p2 == 10) || (p1 == 10 && p2 == 1)){
        if(d == 10 || d == 1){
            cout << "Did the dealer show Blackjack?: ";
            bool dealerBJ = 0;
            cin >> dealerBJ;
            if(dealerBJ)
                return 1;
        }
        return 2.5;
    }

    hand(0, p1, p2, 1);
    return dealerDraw() - 0.5 * insur;
}

int main(){
    ofstream log("winnings.txt", ios::app);
    log << "#\n";

    player::buildHands();
    while(1){
        cout << "Bet (-1 for shoe reset): ";
        double bet = 0;
        cin >> bet;
        if(bet == -1){
            ofstream fout("shoeDistribution.txt");
            fout << "416\n 0 ";
            for(int i = 1; i < 10; i++)
                fout << "32 ";
            fout << 128;
            fout.close();
        }
        else if(bet > 0){
            ifstream fin("shoeDistribution.txt");
            fin >> sz;
            for(int i = 0; i < 11; i++)
                fin >> avail[i];
            fin.close();
            double rtp = RTP::calcRTP(sz, avail);
            cout << "Betting: " << bet << " with RTP: " << 100 * rtp << "%\n";
            double win = bet * (round() - 1), expected = bet * (rtp - 1);
            cout << "Won: " << win << " expected: " << expected << "\n\n";
            log << win << " " << expected << endl;
            //cout << "Total: " << total << "\n";
        }
    }
}


