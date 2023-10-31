#include <bits/stdc++.h>
#include "dealer.cpp"
using namespace std;

const double eps = 0.000000001;

class player{
private:
	inline static vector <string> dec;
	inline static unordered_map <string, int> enc;
	inline static vector <int> hands[2][22];
	inline static int draws[3100];

	int shoeSize;
	int *avail;
	int dUpcard;
	dealer deal;
	
	double ev[3100]; //3083 unique player hands
	int opt[3100]; //0 stand, 1 hit, 2 double, 3 split
	
	int makeSoft(bool soft, int x){
        if(soft && x + 10 < 22)
            x += 10;
        return x;
    }
    
    //shoeSize does not include d and p1, p2
    inline double pCard(string& state, int missing, int c){
		//cout << curShoeSize << " ";
		int sz = shoeSize - missing;
		int a = avail[c] - (state[c-1] - 'a');
		if(dUpcard == 1){
			if(c == 10)
				return (eps + a) / (sz - 1);
			int a10 = avail[10] - (state[9] - 'a');
			return (eps + a * (sz - a10 - 1)) / ((sz - 1.0) * (sz - a10));
		}
		/*for US blackjack
		if(dUpcard == 10){
			if(c == 1)
				return (eps + a) / (sz - 1);
			int a1 = avail[1] - (state[0] - 'a');
			return (eps + a * (sz - a1 - 1)) / ((sz - 1.0) * (sz - a1));
		}
		*/

		return (eps + a) / sz; 
	}
	
	static void buildHandsRec(bool soft, int sum){
		hands[soft][sum].push_back(dec.size() - 1);
		string cur = dec.back();
		int curDraws = draws[dec.size() - 1];

		for(int i = 1; i <= 10; i++){
			if(sum + i <= 21){
				cur[i-1] = char(int(cur[i-1]) + 1);
				if(enc.find(cur) == enc.end()){
					dec.push_back(cur);
					enc[cur] = dec.size() - 1;
					draws[dec.size() - 1] = curDraws + 1;
					buildHandsRec(soft || i == 1, sum + i);
				}
				cur[i-1] = char(int(cur[i-1]) - 1);
			}
		}
	}
	
	void calcEV(){
		for(int s = 1; s >= 0; s--){
			for(int h : hands[s][21]){
				deal = dealer(avail, dUpcard, shoeSize, dec[h], draws[h]);
				ev[h] = 2 - deal.tot[21] - 2 * deal.bj;
				opt[h] = 0;
			}
			for(int t = 20; t >= 0; t--){
				for(int h : hands[s][t]){
					/*
					ev[h] = opt[h] = 0;
					//check if valid hand
					for(int i = 0; i < 10; i++){
						if(dec[h][i] - 'a' > avail[i+1])
							ev[h] = opt[h] = -1;
					}
					if(opt[h] == -1){
						cout << dec[h] << " ";
						continue;
					}*/
					//stand
					int st = makeSoft(s, t);
					deal = dealer(avail, dUpcard, shoeSize, dec[h], draws[h]);
					double sum = deal.bust;
					for(int i = 17; i < st; i++)
						sum += deal.tot[i];
					double standEv = 2 * sum + deal.tot[st];
					//hit
					double hitEv = eps;
					for(int nxt = 1; nxt <= 10; nxt++){
						int st = makeSoft(s || nxt == 1, t + nxt);
						if(st <= 21){
							string nxtStr = dec[h];
							nxtStr[nxt-1] = char(int(nxtStr[nxt-1]) + 1);
							int nxtHand = enc[nxtStr];
							hitEv += pCard(dec[h], draws[h], nxt) * ev[nxtHand];
						}
					}
					opt[h] = hitEv > standEv;
					ev[h] = max(hitEv, standEv);
				}
			}	
		}
	}
	
	double calcDouble(int p1, int p2, string& state){
		double doubleEv = eps;
		for(int nxt = 1; nxt <= 10; nxt++){
			if(p1 + p2 + nxt > 21)
				continue;
			int st = makeSoft(p1 == 1 || p2 == 1 || nxt == 1, p1 + p2 + nxt);
			state[nxt-1]++;
			deal = dealer(avail, dUpcard, shoeSize, state, 3);
			state[nxt-1]--;
			double sum = deal.bust;
			for(int i = 17; i < st; i++)
				sum += deal.tot[i];
			//playtech peek if dealer has hidden bj double bet is returned
			double standEv = 2 * sum + deal.tot[st] + deal.bj / 2;
			doubleEv += pCard(state, 2, nxt) * standEv;
		}
		return 2 * doubleEv - 1;
	}

public:

	static void buildHands(){
		if(!dec.empty())
			return;
		dec.push_back("aaaaaaaaaa");
		enc[dec.back()] = 0;
		draws[0] = 0;
		buildHandsRec(0, 0);
	}

	//before calling constructor call static function buildHands()
	//only dealer card should be removed from avail(sz = prevSz - 1, avail = prevAvail, avail[d]--)
	player(int sz, int* avail, int d, bool canSplit){
		this->shoeSize = sz;
		this->avail = avail;
		this->dUpcard = d;

		calcEV();
		//only consider starting hands for doubling
		for(int p1 = 1; p1 <= 10; p1++){
			for(int p2 = p1; p2 <= 10; p2++){
				string str = "aaaaaaaaaa";
				str[p1-1]++; str[p2-1]++;
				int handID = enc[str];
				double doubleEv = calcDouble(p1, p2, str);
				opt[handID] = (ev[handID] < doubleEv ? 2 : opt[handID]);
				ev[handID] = max(ev[handID], doubleEv);
			}
		}

		if(!canSplit)
			return;

		string str = "baaaaaaaaa";
		avail[1]--;
		double splitAces = calcDouble(1, 0, str);
		avail[1]++;

		str[0]++;
		int handID = enc[str];
		opt[handID] = (ev[handID] < splitAces ? 3 : opt[handID]);
		ev[handID] = max(splitAces, ev[handID]);

		for(int i = 2; i <= 10; i++){
			//no split
			str = "aaaaaaaaaa";
			str[i-1] = 'c';
			handID = enc[str];

			//with split
			avail[i]--;
			str[i-1] = 'b';
			string start = str;			
			player splitting(sz - 1, avail, d, 0);

			double splitEv = eps;
			for(int nxt = 1; nxt <= 10; nxt++){
				str[nxt-1]++;
				splitEv += pCard(start, 2, nxt) * splitting.getEV(str);
				str[nxt-1]--; 
			}
			avail[i]++;

			splitEv = 2 * splitEv - 1;
			opt[handID] = (ev[handID] < splitEv ? 3 : opt[handID]);
			ev[handID] = max(splitEv, ev[handID]);
		}
	}

	//ev of 1 card hands are not accurate
	double getEV(const string& cur){
		return ev[enc[cur]];
	}

	int getAct(const string& cur){
		return opt[enc[cur]];
	}

};

/*
int sz = 207;
int avail[11]{-1, 15, 16, 16, 16, 16, 16, 16, 16, 16, 64};

int main(){
	//p1 = 6, p2 = 4, d = 9
	player::buildHands(); 
    player pH(sz, avail, 1, 1);
	string str = "abaaaabaaa";
	cout << pH.getAct(str) << " " << pH.getEV(str) - 1 << endl;
}
*/
