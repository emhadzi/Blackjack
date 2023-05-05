#include <bits/stdc++.h>
#include "RTP.cpp"
using namespace std;

const int decks = 8, MAX_PEN = 225; 
int deck[decks * 52];
int cur = 0;
double profit = 0, exProfit = 0;

int sz;
int avail[11];
double total = 0;

pair <bool, int> pl[2], dl;
int p1, p2, d1, d2;

//0 stand, 1 hit, 2 double, 3 split, 4 newHand, -1 exit

void makeShoe(int decks){
    sz = 52 * decks;
    avail[0] = -1;
    for(int i = 1; i < 10; i++)
        avail[i] = 4 * decks;
    avail[10] = 16 * decks;
}

inline int makeSoft(bool soft, int x){
    if(soft && x + 10 < 22)
        x += 10;
    return x;
}

int draw(char side, int ind){
    int c = deck[cur++];
    //cin >> c; //>> s;

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
    avail[d2]--;
    sz--;
    dl.first |= (d2 == 1);
    dl.second += d2;

    while(makeSoft(dl.first, dl.second) < 17)
        draw('d', 0);
}

double hand(){
    total = 0;
    pl[0] = dl = {0, 0};
    p1 = draw('p', 0);
    d1 = draw('d', 0);
    p2 = draw('p', 0);
    d2 = deck[cur++];

    string startState = "aaaaaaaaaa";
    startState[p1-1]++; startState[p2-1]++;
    avail[p1]++; avail[p2]++;

    player pH(sz + 2, avail, d1, 1);
    avail[p1]--; avail[p2]--;

    if(d1 == 1){
        double insurEv = (1.5 * avail[10] / (sz + eps)) - 0.5;
        bool dealerBJ = (d1 == 1 && d2 == 10) || (d1 == 10 && d2 == 1);
        if(insurEv > 0)
            total += 1.5 * dealerBJ - 0.5;
        if(dealerBJ){
            avail[10]--;
            sz--;
            cur++;
            if((p1 == 1 && p2 == 10) || (p1 == 10 && p2 == 1))
                total++;
            return total;
        }
    }

    if((p1 == 1 && p2 == 10) || (p1 == 10 && p2 == 1)){
        total += 2.5;
        return total;
    }

    int act = pH.getAct(startState);
    //cout << "state " << startState << " act " << act << " ";

    //stand
    if(act == 0){
        dealerDraw();
        total += cmpRes(0);
    }

    //double
    else if(act == 2){
        draw('p', 0);
        if(pl[0].second > 21)
            total--;
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
            if(pl[0].second > 21)
                break;
            cur[card - 1] = char(cur[card - 1] + 1);
            act = pH.getAct(cur);
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
            //replace old p2
            startState[p2-1]--;
            pl[t] = {p1 == 1, p1};
            p2 = draw('p', t);
            startState[p2-1]++;

            avail[p1]++; avail[p2]++;
            player pHSp(sz + 2, avail, d1, 0);
            avail[p1]--; avail[p2]--;

            int act = pHSp.getAct(startState);
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
                    if(pl[t].second > 21)
                        break;
                    cur[card - 1] = char(cur[card - 1] + 1);
                    act = pHSp.getAct(cur);
                }
            }
        }
        //compute results after dealer draw
        if(!(pl[0].second > 21 && pl[1].second > 21)){
            dealerDraw();
            total += (db[0] ? 2 : 1) * cmpRes(0) + (db[1] ? 2 : 1) * cmpRes(1);
        }
    }
    return total;
}

double getBet(double rtp){
	if(rtp > 1.01)
        return 12;
    if(rtp > 1.005)
        return 7;
    if(rtp > 1)
        return 5;
    return 0.05;
}

int main(){
    int hands = 0;
	player::buildHands();
    
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
			//double hl = RTP::trueHighLow(sz, avail);
            double betSize = getBet(rtp);
            //cout << "Next 10 cards:\n";
            //for(int i = 0; i < 10; i++)
            //    cout << deck[cur + i] << " ";
            double win = hand();
            profit += betSize * (win - 1);
            //cout << "\n" << hands + 1 << " " << win << "\n"; //<< " " << rtp << " " << hl << " " << betSize * (win - 1) << endl;
            exProfit += betSize * (rtp - 1);
            hands++;
            //int wait;
            //cin >> wait;
        }
        
        cout << shoe + 1 << " / " << hands << " / " << profit << " / " << exProfit << endl;
        cout << "Calculated RTP: " << 1 + (profit / (hands + eps)) << " / Expected RTP: " << 1 + (exProfit / (hands + eps)) << endl << endl;
    }
}
