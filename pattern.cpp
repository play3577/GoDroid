#include "pattern.h"
#include <iostream>
#include <bitset>

unsigned short toShort(int pattern[8]){
    unsigned short res = 0;
    for(int i = 0; i < 8; ++i){
        res += (pattern[i]*pow(4,i));
    }
    return res;
}

unsigned short word(unsigned short orig, int num){
    unsigned short res = orig/pow(4,num);
    return res % 4;
}

unsigned short clockwise(unsigned short orig){
    int trans[8] = {2,4,7,1,6,0,3,5};
    unsigned short res = 0;
    for(int i = 0; i < 7; ++i)
        res += (word(orig,trans[i])*pow(4,i));
    return res;
}

unsigned short exchange(unsigned short orig){
    int color;
    for(int i = 0; i < 7; ++i){
        color = word(orig,i);
        orig -= color * pow(4,i);
        color = color == 2 ? 1 : color;
        color = color == 1 ? 2 : color;
        orig += color * pow(4,i);
    }
    return orig;
}

unsigned short symmetryX(unsigned short orig){
    int trans[8] = {2,1,0,4,3,7,6,5};
    unsigned short res = 0;
    for(int i = 0; i < 7; ++i)
        res += (word(orig,trans[i])*pow(4,i));
    return res;
}

unsigned short symmetryY(unsigned short orig){
    int trans[8] = {5,6,7,3,4,0,1,2};
    unsigned short res = 0;
    for(int i = 0; i < 7; ++i)
        res += (word(orig,trans[i])*pow(4,i));
    return res;
}

void insert(unsigned short pat){
    for(int i = 0; i < 4; ++i){
        for(int j = 0; j < 2; ++j){
            patterns.insert(pat);
            patterns.insert(symmetryX(pat));
            patterns.insert(symmetryY(pat));
            patterns.insert(symmetryX(symmetryY(pat)));
            pat = exchange(pat);
        }
		pat = clockwise(pat);
    }
}

void insertWhite(unsigned short pat){
    for(int i = 0; i < 4; ++i){
        patternsWhite.insert(pat);
        patternsWhite.insert(symmetryX(pat));
        patternsWhite.insert(symmetryY(pat));
        patternsWhite.insert(symmetryX(symmetryY(pat)));
		pat = clockwise(pat);
    }
	pat = exchange(pat);
    for(int i = 0; i < 4; ++i){
        patternsBlack.insert(pat);
        patternsBlack.insert(symmetryX(pat));
        patternsBlack.insert(symmetryY(pat));
        patternsBlack.insert(symmetryX(symmetryY(pat)));
		pat = clockwise(pat);
    }
}

void insertBlack(unsigned short pat){
    for(int i = 0; i < 4; ++i){
        patternsBlack.insert(pat);
        patternsBlack.insert(symmetryX(pat));
        patternsBlack.insert(symmetryY(pat));
        patternsBlack.insert(symmetryX(symmetryY(pat)));
		pat = clockwise(pat);
    }
	pat = exchange(pat);
    for(int i = 0; i < 4; ++i){
        patternsWhite.insert(pat);
        patternsWhite.insert(symmetryX(pat));
        patternsWhite.insert(symmetryY(pat));
        patternsWhite.insert(symmetryX(symmetryY(pat)));
		pat = clockwise(pat);
    }
}

void erase(unsigned short pat){
    for(int i = 0; i < 4; ++i){
        for(int j = 0; j < 2; ++j){
            patterns.erase(pat);
            patterns.erase(symmetryX(pat));
            patterns.erase(symmetryY(pat));
            patterns.erase(symmetryX(symmetryY(pat)));
            pat = exchange(pat);
        }
    }
}

void initPatterns(){
    unsigned short pat;
    //hane
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            for(int k = 0; k < 3; ++k){
                int pattern[8] = {2,1,2,0,0,i,j,k};
                pat = toShort(pattern);
                insert(pat);
                int pattern1[8] = {2,1,i,2,0,j,0,k};
                pat = toShort(pattern1);
                insert(pat);
            }
            int pattern2[8] = {2,1,0,0,0,i,0,j};
            pat = toShort(pattern2);
            insert(pat);
            int pattern3[8] = {2,1,1,0,0,i,0,j};
            pat = toShort(pattern3);
            insertBlack(pat);
        }
    }

    //cut1
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            for(int k = 0; k < 3; ++k){
                for(int x = 0; x < 3; ++x){
                    for(int y = 0; y < 3; ++y){
                        int pattern[8] = {2,1,i,1,j,k,x,y};
                        pat = toShort(pattern);
                        insert(pat);
                    }
                }
            }
        }
    }
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            for(int k = 0; k < 3; ++k){
                int pattern[8] = {2,1,i,1,1,j,0,k};
                pat = toShort(pattern);
                erase(pat);
                int pattern1[8] = {2,1,i,1,0,j,1,k};
                pat = toShort(pattern1);
                erase(pat);
            }
        }
    }

    //cut2
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            for(int k = 0; k < 2; ++k){
                for(int x = 0; x < 2; ++x){
                    for(int y = 0; y < 2; ++y){
                        int pattern[8] = {i,2,j,1,1,k*2,x*2,y*2};
                        pat = toShort(pattern);
                        insert(pat);
                    }
                }
            }
        }
    }

    //boarder
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            int pattern[8] = {2,0,i,1,j,3,3,3};
            pat = toShort(pattern);
            insert(pat);
        }
    }
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            for(int k = 0; k < 2; ++k){
                int pattern[8] = {i,2,j,k,1,3,3,3};
                pat = toShort(pattern);
                insert(pat);
            }
        }
    }
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            for(int k = 0; k < 3; ++k){
                int pattern[8] = {i,2,1,j,k,3,3,3};
                pat = toShort(pattern);
                insertBlack(pat);
            }
        }
    }
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            for(int k = 0; k < 2; ++k){
                int pattern[8] = {i,2,1,j,k,3,3,3};
                pat = toShort(pattern);
                insertWhite(pat);
            }
        }
    }
    for(int i = 0; i < 3; ++i){
        int pattern[8] = {i,2,1,1,2,3,3,3};
        pat = toShort(pattern);
        insertWhite(pat);
    }
}

bool matchPattern(unsigned short pat, int color){
	return ((patterns.find(pat)!=patterns.end()) || ((color == 1) && (patternsWhite.find(pat)!=patternsWhite.end())) || ((color == 2) && (patternsBlack.find(pat)!=patternsBlack.end())));
}

int findPattern(board *brd, int color, int lastx, int lasty){
	srand(time(0));
	int deltax[8] = {-1,0,1,-1,1,-1,0,1};
	int deltay[8] = {-1,-1,-1,0,0,1,1,1};
	int x[8], y[8];
	int cnt = 0;
	for(int i = 0; i < 8; ++i){
		int centerx = lastx + deltax[i];
		int centery = lasty + deltay[i];
		int pattern[8];
		if(centerx > 0 && centerx < 14 && centery > 0 && centery < 14){
			for(int j = 0; j < 8; ++j){
				pattern[j] = brd->get_cell(centerx+deltax[j],centery+deltay[j]);
			}
			short patt = toShort(pattern);
			if(matchPattern(patt, color)){
				x[cnt] = centerx;
				y[cnt] = centery;
				cnt++;
			}
		}
	}
	if(cnt){
		int r = rand() % cnt;
		return x[r] * 13 + y[r];
	}
	return 0;
}

int main(){
    int pattern[8] = {3,3,3,3,3,3,3,3};
    cout << "hello" << endl;
    unsigned short pat = toShort(pattern);
    cout << pat << endl;
    cout << bitset<sizeof(int)*8>(pat) << endl;
}