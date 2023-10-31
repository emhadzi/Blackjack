
#include <bits/stdc++.h>
#include "RTP.cpp"
using namespace std;

int sz;
int avail[11];
int suited[11][4];
//0 spades, 1 clubs, 2 hearts, 3 diamonds

pair <bool, int> pl[2], dl;
int p1, p2, d;

string action[]{"stand", "hit", "double", "split"};

inline int getSuit(char s){
    if(s == 's')
        return 0;
    if(s == 'c')
        return 1;
    if(s == 'h')
        return 2;
    return 3;
}

void makeShoe(int decks){
    sz = 52 * decks;
    avail[0] = -1;
    for(int i = 1; i < 10; i++){
        avail[i] = 4 * decks;
        for(int s = 0; s < 4; s++)
            suited[i][s] = decks;
    }
    avail[10] = 16 * decks;
}

inline int makeSoft(bool soft, int x){
    if(soft && x + 10 < 22)
        x += 10;
    return x;
}

int draw(char side, int ind){
    cout << "Draw " << side << ": ";
    int c;
    char s;
    cin >> c; //>> s;
    while(c < 1 || c > 10){
        cout << "Renter card: ";
        cin >> c;
    }
    if(c == 1 || c == 10)
        RTP::cnt--;
    else if(c < 7)
        RTP::cnt++;

    sz--;
    avail[c]--;
    //suited[c][getSuit(s)]--;

    if(side == 'd'){
        dl.first |= (c == 1);
        dl.second += c;
    }
    else{
        pl[ind].first |= (c == 1);
        pl[ind].second += c;
    }

    return c;
} 

int cmpRes(int ind){
    if(pl[ind].second > 21)
        return 0;
    int dTot = makeSoft(dl.first, dl.second);
    if(dTot > 21 || dTot < makeSoft(pl[ind].first, pl[ind].second))
        return 2;
    return dTot == makeSoft(pl[ind].first, pl[ind].second);
}

void dealerDraw(){
    cout << "Dealer drawing: \n";
    while(makeSoft(dl.first, dl.second) < 17)
        draw('d', 0);
}

void playerBust(){
    cout << "You busted\n";
    draw('d', 0);
}

double hand(){
    double total = 0;
    pl[0] = dl = {0, 0};
    cout << "Input p1, d, p2: \n";
    p1 = draw('p', 0);
    d = draw('d', 0);
    p2 = draw('p', 0);

    string startState = "aaaaaaaaaa";
    startState[p1-1]++; startState[p2-1]++;
    avail[p1]++; avail[p2]++;

    player pH(sz + 2, avail, d, 1);
    avail[p1]--; avail[p2]--;

    if(d == 1){
        double insurEv = (1.5 * avail[10] / (sz + eps)) - 0.5;
        cout << "Recommended insurance: " << (insurEv > 0 ? "Yes (" : "No (") << insurEv << ")\n";
        cout << "Did the dealer show Blackjack?: ";
        bool dealerBJ = 0;
        cin >> dealerBJ;
        if(dealerBJ){
            cout << "Disposed card: 10\n";
            avail[10]--;
            sz--;
            if((p1 == 1 && p2 == 10) || (p1 == 10 && p2 == 1))
                total++;
            return total;
        }
    }

    if((p1 == 1 && p2 == 10) || (p1 == 10 && p2 == 1)){
        total += 2.5;
        return total;
    }

    cout << "Recommended action: " << action[pH.getAct(startState)] << "(" << pH.getEV(startState) << ")" << "\n";
    int act;
    cin >> act;

    //stand
    if(act == 0){
        dealerDraw();
        total += cmpRes(0);
    }

    //double
    else if(act == 2){
        draw('p', 0);
        if(pl[0].second > 21){
            total--;
            playerBust();
        }
        else{
            dealerDraw();
            total += 2 * cmpRes(0) - 1;
        }
    }

    //hit
    else if(act == 1){
        string cur = startState;
        while(act == 1){
            int card = draw('p', 0);
            if(pl[0].second > 21){
                playerBust();
                break;
            }
            cur[card - 1] = char(cur[card - 1] + 1);
            cout << "Recommended action: " << action[pH.getAct(cur)] << "(" << pH.getEV(cur) << ")" << "\n";
            cin >> act;
        }
        if(pl[0].second <= 21){
            dealerDraw();
            total += cmpRes(0);
        }
    }
    //split
    else{
        total--;
        bool db[2];
        for(int t = 0; t < 2; t++){
            cout << "Playing hand " << t + 1 << ":\n";
            cout << "Input second card of current hand:\n";
            //replace old p2
            startState[p2-1]--;
            pl[t] = {p1 == 1, p1};
            p2 = draw('p', t);
            startState[p2-1]++;

            avail[p1]++; avail[p2]++;
            player pHSp(sz + 2, avail, d, 0);
            avail[p1]--; avail[p2]--;

            cout << "Recommended action: " << action[pHSp.getAct(startState)] << "(" << pHSp.getEV(startState) << ")" << "\n";
            int act;
            cin >> act;
            //stand - do nothing
            //double
            db[t] = act == 2;
            if(act == 2){
                draw('p', t);
                total--;
            }
            //hit
            else if(act == 1){
                string cur = startState;
                while(act == 1){
                    int card = draw('p', t);
                    if(pl[t].second > 21){
                        cout << "Busted hand " << t + 1 << endl;
                        break;
                    }
                    cur[card - 1] = char(cur[card - 1] + 1);
                    cout << "Recommended action: " << action[pHSp.getAct(cur)] << "(" << pHSp.getEV(cur) << ")" << "\n";
                    cin >> act;
                }
            }
        }
        //compute results after dealer draw
        if(pl[0].second > 21 && pl[1].second > 21)
            playerBust();
        else{
            dealerDraw();
            total += (db[0] ? 2 : 1) * cmpRes(0) + (db[1] ? 2 : 1) * cmpRes(1);
        }
    }
    return total;
}

void dispose(){
    int c = 1;
    char s;
    cout << "Enter extra cards you want to dispose (0 to exit): \n";
    while(1){
        cin >> c;// >> s;
        if(c == 0)
            break;
        if(c < 0 || c > 10)
            continue;
        if(c == 1 || c == 10)
            RTP::cnt--;
        else if(c < 7)
            RTP::cnt++;
        sz--;
        avail[c]--;
        //suited[c][getSuit(s)]--;
        cout << "Disposed card: " << c << endl;
    }
}

void stats(double rtp){
    cout << "Remaining shoe size: " << sz << "\n";
    cout << "Shoe distribution: ";
    for(int i = 1; i <= 10; i++)
        cout << avail[i] << " ";
    cout << "\n";

    //cout << "21+3 RTP: " << 100 * RTP::calc21p3(sz, suited) << "%\n";
    cout << "True count: " << RTP::trueHighLow(sz, avail) << "\n";
    cout << "RTP: " << 100 * rtp << "%\n";
}

int main(){
    ofstream log("winnings.txt", ios::app);
    log << "#\n";

    player::buildHands();
    RTP::cnt = 0;
    makeShoe(8);
    while(1){
        double rtp = RTP::calcRTP(sz, avail);
        stats(rtp);
        cout << "Participate?\n";
        double play = 0;
        cin >> play;
        if(play != 0){
            double win = play * (hand() - 1);
            log << win << " " << play * (rtp - 1) << endl;
        }
        dispose();
        cout << endl;
        //cout << "Total: " << total << "\n";
    }
}
//fix winnnings when double on 10