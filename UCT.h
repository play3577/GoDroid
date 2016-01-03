#include <math.h>
#include <string.h>
#include <stack>
#include <assert.h>
#include <queue>
#include <fstream>
#include "board.h"
#include "montecarlo.h"

#ifndef _UCT_H
#define _UCT_H

#define largeFloat 1000000000000.0

using namespace std;

class UCT {
private:
	struct Node {
		int total;
		int win;
		int AMAFwin;
		double value() {
			return double(win)/total;
		}
		Node *parent, *lchild, *sibling;
		//board currentBoard;
		int move;
		bool player;
		char color;
		double ucbValue;

		Node(bool _play) {
			win = 0;
			AMAFwin = 0;
			total = 0;
			parent = NULL;
			lchild = NULL;
			sibling = NULL;
			player = _play;
			move = -1;
			ucbValue = -1;
		}
		double V(double explore_coeff) {
			//printf("Tn: %d Total: %d ", Tn, total);
			return value() - value() * value() + sqrt(2 * explore_coeff / total);
		}
		double getUCBValue(double explore_coeff) {
			if (total == 0) return largeFloat;
			//printf("V: %f\n", V(explore_coeff));
			//return value() + sqrt(explore_coeff / total * min(0.25, V(explore_coeff)));
			return value() + sqrt(explore_coeff / total * 2);
		}
		void addChild(Node *newChild) {
			if (lchild == NULL) {
				lchild = newChild;
				lchild->parent = this;
			}
			else {
				newChild->sibling = lchild;
				lchild = newChild;
				lchild->parent = this;
			}
		}
		void removeChild(Node *delChild) {
			if (lchild == delChild) {
				lchild = lchild->sibling;
				return;
			}
			Node *actChild;
			actChild = lchild;
			while (actChild->sibling != NULL) {
				if (actChild->sibling == delChild) {
					actChild->sibling = actChild->sibling->sibling;
					return;
				}
				actChild = actChild->sibling;
			}
		}
		void freeSubtree(Node *p) {
			if (p == NULL) return;
			p = p->lchild;
			Node *tmp;
			while (p != NULL) {
				freeSubtree(p);
				tmp = p;
				p = p->sibling;
				delete tmp;
			}
		}
		void updateWin() {
			win++;
			total++;
		}
		void updateLoss() {
			total++;
		}
		bool isLeaf() {
			if (lchild == NULL) return 1;
			return 0;
		}
		Node* findBestChild() {
			Node* bestChild = NULL;
			double explore_coeff = log(double(total)); //log为0时可能出错
			
			double bestUrgency = -largeFloat;
			
			Node *p = lchild;
			while (p != NULL) {
				double childUrgency = p->getUCBValue(explore_coeff);
				p->ucbValue = childUrgency;
				if (childUrgency >= bestUrgency) {
					bestUrgency = childUrgency;
					bestChild = p;
				}
				p = p->sibling;
			}
			return bestChild;
		}
		Node* findBestMove() {
			Node* bestMove = NULL;
			Node *p = lchild;
			double bestValue = p->value();
			while (p != NULL) {
				if (p->value() >= bestValue) {
					bestValue = p->value();
					bestMove = p;
				}
				p = p->sibling;
			}
			return bestMove;
		}
		bool removeWorstChild() {
			Node *worstChild = NULL;
			double worstUrgency = largeFloat;
			double explore_coeff = 2 * log(double(total));
			Node *p = lchild;
			while (p != NULL) {
				double childUrgency = p->getUCBValue(explore_coeff);
				if (childUrgency < worstUrgency) {
					worstUrgency = childUrgency;
					worstChild = p;
				}
				p = p->sibling;
			}
			if (worstChild == lchild && worstChild->sibling == NULL) return 0;
			removeChild(worstChild);
			freeSubtree(worstChild);
			delete worstChild;
		}
	};
	Node *root;
	board rootBoard;
public:
	UCT(board &inBoard, bool player) {
		root = new Node(!player);
		rootBoard = inBoard;
	}
	~UCT() {
		root->freeSubtree(root);
		delete root;
		/*while (garbage != NULL) {
			Node *p = garbage;
			garbage = garbage->sibling;
			delete p;
		}*/
	}
	board getBoard(Node *p) {
		board currentBoard(rootBoard);
		stack<int> back;
		Node *q = p;
		while (q->parent != NULL) {
			back.push(q->move);
			q = q->parent;
		}
		bool player = root->player;
		if (!back.empty())
			back.pop();
		while (!back.empty()) {
			int num = back.top();
			back.pop();
			int coordX = num / SIZE + 1;
			int coordY = num % SIZE + 1;
			player = !player;
			currentBoard.play(player, coordX, coordY);
		}
		return currentBoard;
	}
	void createAllChildrenIfNone(Node *p,bool player) {
		//assert(p != NULL);
		if (p->lchild == NULL) {
			board currentBoard(getBoard(p));
			set<short>* ataripositionalley = player?(&currentBoard.ataripositionforblack):(&currentBoard.ataripositionforwhite);
			set<short>* ataripositionenemy = (!player)?(&currentBoard.ataripositionforblack):(&currentBoard.ataripositionforwhite);

			std::set<short>::iterator iter;
			if (!ataripositionalley->empty())
				for (iter=ataripositionalley->begin();iter!=ataripositionalley->end();iter++){
					//printf("atari position alley:%d length:%d\n",*iter,ataripositionalley->size());
					if (currentBoard.valid_test(currentBoard.get_kaku(*iter),!player)){
						bool tmpPlayer = !(p->player);
						Node *tmp= new Node(tmpPlayer);
						short pos = *iter;
						tmp->move = pos-1;
						p->addChild(tmp);
					}
				}
			if (!ataripositionenemy->empty())
				for (iter=ataripositionenemy->begin();iter!=ataripositionenemy->end();iter++){
					//printf("atari position enemy:%d length:%d\n",*iter,ataripositionenemy->size());
					if (currentBoard.valid_test(currentBoard.get_kaku(*iter),!player)){
						bool tmpPlayer = !(p->player);
						Node *tmp= new Node(tmpPlayer);
						short pos = *iter;
						tmp->move = pos-1;
						p->addChild(tmp);
					}
				}
			if (p->lchild == NULL){
				std::set<short>* validset = (player?(&currentBoard.validsetforwhite):(&currentBoard.validsetforblack));
				if (!validset->empty()){
					for (iter=validset->begin();iter!=validset->end();iter++){
						bool tmpPlayer = !(p->player);
						Node *tmp= new Node(tmpPlayer);
						short pos = *iter;
						tmp->move = pos-1;
						p->addChild(tmp);
					}
				}
			}
		}
	}
	void update(Node *s, bool winner) {
		while (s != NULL) {
			if (s->player == winner) s->updateWin();
			else s->updateLoss();
			s = s->parent;
		}
	}
	void playOneSequenceInMoGo(bool player) {
		Node* p = root;
		createAllChildrenIfNone(p,player);
		do {
			p = p->findBestChild();
			//if (p == NULL) return;
			if (p != NULL && p->isLeaf()) {
				if (p->total == 0) break;
				createAllChildrenIfNone(p,player);
				p = p->findBestChild();
				break;
			}
		} while (1);
		if (p != NULL) {
			board currentBoard(getBoard(p));
			montecarlo m(currentBoard,player);
			update(p, m.getWinner());
		}
		else {
			board currentBoard(getBoard(p->parent));
			montecarlo m(currentBoard,player);
			update(p->parent, m.getWinner());
		}	
	}
	int getNextMove() {
		Node *tmp = root->findBestMove(); //此处在第一层节点未完全展开时存在bug
		ofstream out("log2.txt", ios::app);
		int i = tmp->move / 13 + 1;
		int j = tmp->move % 13 + 1;
		out << "player" << tmp->player << ' ' << i << ' ' << j << " UCB: " << tmp->ucbValue << endl;
		return tmp->move;
	}
	void showTree(bool flag = 1) {
		rootBoard.showboardtofile();
		Node *p = root;
		queue<Node *> q;
		q.push(p);
		ofstream out("log.txt",ios::ate);
		Node *tmp = p->lchild;
		out <<"step"<<endl;
		while (!q.empty()) {
			p = q.front();
			q.pop();
			if (p->total && p!=root)
				//printf("%d/%d ", p->win, p->total);
				out <<"player" << p->player <<"father: "<< (p->parent->move)/13+1<<' '<<(p->parent->move)%13+1<<" move " << (p->move)/13+1<<' '<<(p->move)%13+1<<' ' << p->win << '/' << p->total << "  winning rate: "<< double(p->win)/p->total << "  UCB Value: " << p->ucbValue <<endl;
			Node *tmp = p->lchild;
			while (tmp != NULL) {
				q.push(tmp);
				tmp = tmp->sibling;
			}
		}
	}
	void showTotal() {
		printf("%d/%d\n", root->win, root->total);
	}

};

#endif