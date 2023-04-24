#include "player.cpp"
using namespace std;

class RTP{
private:
    static constexpr double BJRAT = 2.5;

public:
    inline static int cnt;
    RTP(int sz, int* avail){}

    static double calc21p3(int sz, int suited[11][4]){
        double rtp = 0;
        //flush : 5
        for(int s = 0; s < 4; s++){
            int suitTot = 0;
            for(int i = 1; i < 11; i++)
                suitTot += suited[i][s];
            rtp += (5 * suitTot * (suitTot - 1.0) * (suitTot - 2.0)) / (sz * (sz - 1) * (sz - 2)); 
        }

        //straight : 10
        //three of a kind : 30
        //straight flush : 40
        //suited three of a kind : 100
        return rtp;
    }

    static double calcRTP(int sz, int* avail){
        //player::buildHands(); 

		double rtp = 0;

        for(int d = 1; d <= 10; d++){
            double prd = (eps + avail[d]--) / sz; 
            player pH(sz - 1, avail, d, 1);
            string state = "aaaaaaaaaa";

            for(int p1 = 1; p1 <= 10; p1++){
                state[p1-1]++;
                double pr1 = prd * (eps + avail[p1]--) / (sz - 1);

                for(int p2 = p1; p2 <= 10; p2++){
                    state[p2-1]++;
                    double pr2 = pr1 * (eps + avail[p2]--) / (sz - 2);

                    if(!((p1 == 1 && p2 == 10) || (p1 == 10 && p2 == 1))){
                        double shownBJ = (d == 1) * (eps + avail[10]) / (sz - 3);
                        double startEv = pH.getEV(state);
                        double insur = (d == 1) * max(0.0, (1.5 * avail[10] / (sz + eps)) - 0.5);

                        rtp += (p1 == p2 ? 1 : 2) * (1 - shownBJ) * pr2 * (startEv + insur);
                    }

                    state[p2-1]--;
                    avail[p2]++;
                }

                state[p1-1]--;
                avail[p1]++;
            }

            avail[d]++;
        }
        double pBJ = 2 * ((eps + avail[1]) * avail[10]) / (sz * (sz-1));
        double bothBJ = 2 * pBJ * ((eps + avail[1] - 1) * (avail[10] - 1)) / ((sz - 2) * (sz - 3)); 
        rtp += pBJ * BJRAT + bothBJ * (1 - BJRAT);
        return rtp;
    }
    
    static double trueHighLow(int sz, int* avail){
		return (52 * cnt) / (sz + eps); 
	}
};

/*
int sz = 236;
int avail[11]{-1, 19, 15, 15, 19, 16, 18, 15, 22, 25, 72};

void makeShoe(int decks){
    sz = 52 * decks;
    avail[0] = -1;
    for(int i = 1; i < 10; i++)
        avail[i] = 4 * decks;
    avail[10] = 16 * decks;
}

int main(){
    //makeShoe(1);
    cout << 100 * (1 - RTP::calcRTP(sz, avail));
    //game.printActEV(); 
}
*/
