#include <bits/stdc++.h>
#include "RTP.cpp"
using namespace std;

const int decks = 8, MAX_PEN = 220;

int sz, cur;
int avail[11], deck[500];

pair <bool, int> pl[2], dl;
bool db[2];
int p1, p2, d, d2;
double profit = 0, exProfit = 0, m1 = 0, m2 = 0;

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
    //cout << "Draw " << side << ": ";
    int c = deck[cur++];
    //cin >> c;
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

    //cout << "Dealer drawing: \n";
    addCard(dl, d2);
    bool hiddenBJ = (d == 10 && d2 == 1);
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

    //cout << "Recommended action: " << action[pH.getAct(startState)] << "(" << pH.getEV(startState) << ")" << "\n";
    int act = pH.getAct(startState);
    //cin >> act;

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
            //cout << "Recommended action: " << action[pH.getAct(cur)] << "(" << pH.getEV(cur) << ")" << "\n";
            act = pH.getAct(cur);
        }
    }
    //split
    else{
        for(int t = 0; t < 2; t++){
            //cout << "Playing hand " << t + 1 << ":\n";
            //cout << "Input second card of current hand:\n";
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

    //cout << "Input p1, d, p2: \n";
    p1 = draw('p');
    d = draw('d');
    addCard(dl, d);
    p2 = draw('p');
    d2 = deck[cur++];

    //ace dealer upcard
    if(d == 1){
        double insurEv = (1.5 * avail[10] / (sz + eps)) - 0.5;
        //cout << "Recommended insurance: " << (insurEv > 0 ? "Yes (" : "No (") << insurEv << ")\n";
        //cout << "Did the dealer show Blackjack?: ";
        bool dealerBJ = d2 == 10;
        //cin >> dealerBJ;
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
        //cout << "Did the dealer show Blackjack?: ";
        bool dealerBJ = d2 == 1;
        //cin >> dealerBJ;
        if(dealerBJ)
            return 1;
        return 2.5;
    }

    hand(0, p1, p2, 1);
    return dealerDraw() - 0.5 * insur;
}

void makeShoe(int decks){
    sz = 52 * decks;
    avail[0] = -1;
    for(int i = 1; i < 10; i++)
        avail[i] = 4 * decks;
    avail[10] = 16 * decks;
}

int main(){
    int hands = 0;
	player::buildHands();
    ofstream lg("rtpLog5.txt", ios::app);

    for(int shoe = 0; shoe < 100000; shoe++){
        for(int i = 1; i < 10; i++){
            for(int j = 0; j < 4 * decks; j++)
                deck[(i-1) * (4 * decks) + j] = i;
        }
        for(int i = 0; i < 16 * decks; i++)
            deck[36 * decks + i] = 10;
        shuffle(deck, deck + decks * 52, default_random_engine(time(0)));   
        makeShoe(8);
		//cout << RTP::calcRTP(sz, avail);
		
        cur = 0;
        while(cur < MAX_PEN){
			double rtp = RTP::calcRTP(sz, avail);
            lg << cur << " " << rtp << "\n";
            //cout << "Next 10 cards:\n";
            //for(int i = 0; i < 10; i++)
            //    cout << deck[cur + i] << " ";
            double win = round();
            profit += win - 1;
            //cout << "\n" << hands + 1 << " " << win << "\n"; //<< " " << rtp << " " << hl << " " << betSize * (win - 1) << endl;
            exProfit += rtp - 1;
            m1 += max(rtp - 1, 0.0);
            if(rtp > 1)
                m2 += (rtp - 1) * (rtp - 1);
            hands++;
            //int wait;
            //cin >> wait;
        }
        
        lg.flush();
        cout << shoe + 1 << " / " << hands << " / " << profit << " / " << exProfit << endl;
        cout << "Calculated RTP: " << 1 + (profit / (hands + eps)) << " / Expected RTP: " << setprecision(10) << 1 + (exProfit / (hands + eps)) << endl;
        cout << "m1: " << setprecision(10) << (m1 / (hands + eps)) << endl;
        cout << "m2: " << setprecision(10) << (m2 / (hands + eps)) << endl << endl;
    }
}
