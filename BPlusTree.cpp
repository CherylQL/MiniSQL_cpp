#include <bits/stdc++.h>

using namespace std;

struct node{
    struct node * next;
    struct node * last; 
    char * value;
};
struct treenode {
    int num;
    int isleaf;
    char * value[20];
    treenode * fa;
    int No;
    treenode * son[20];
    treenode * next;
    node * to[20];
};
struct result{
	node * to;
	result * next;
};


class BPlusTree {
public:
    treenode * root;
    treenode * begin;
    string type;
    int n;
    int len;
    treenode * build(int isleaf){
        treenode * ret = new treenode;
        ret->isleaf = isleaf;
        ret->num = 0;
        ret->next = NULL;
        ret->fa = NULL;
        ret->No = 0;
        //ret->value = new char * [n + 1];
        //ret->son = new treenode * [n + 1];
        //ret->son = malloc(sizeof(treenode *) * (n + 1));
        for(int i = 0;i <= n;i++) ret->value[i] = new char[len];
        for(int i = 0;i <= n;i++) ret->son[i] = new treenode;
            //memset(re)
            //son[i] = NULL;
        return ret;
    }
	BPlusTree(string Type, int N, int Len){
        n = N;
        type = Type;
        len = Len;
		root = build(1);
		begin = root;
    }
    int cmp(char * x, char * y){
		if(type == "char"){
			char stra[len + 1], strb[len + 1];
			memcpy(stra, x, len);
			memcpy(strb, y, len);
			stra[len] = '\0';
			strb[len] = '\0';
			string a = stra;
			string b = strb;
			//cout << a << " " << b << " " << (a >= b) << endl;
			return a >= b;
		}
		else if(type == "int"){
			int a, b;
			memcpy((char *)&a, x, sizeof(int));
			memcpy((char *)&b, y, sizeof(int));
			return a >= b;
		}
		else if(type == "float"){
			double a, b;
			memcpy((char *)&a, x, sizeof(double));
			memcpy((char *)&b, y, sizeof(double));
			return a >= b;
		}
        return 0;
	}
    int find(treenode * now, char * x){
    	if(!now->num)  return -1;
        int l = 0, r = now->num - 1;
        while(l < r){
            int mid = (l + r) >> 1;
            if(cmp(x, now->value[mid])) l = mid + 1;
            else r = mid - 1;
        }
        /*pos =  2
        0 1 2 3 4
        1 2 3 5*/
        if(!cmp(x, now->value[l])) l = l -1;
        return l;
    }
    void insert(treenode * now, int pos, treenode * Son, node * to, char * x){
    	int a;
    	//printf("%d\n", pos);
    	memcpy((char *)&a, x, len);
        now->num++;
        for(int i  = now->num - 1;i > pos  + 1; i--){
            memcpy(now->value[i], now->value[i - 1], len);
            now->son[i] = now->son[i - 1];
            now->to[i] = now->to[i - 1];
            if(now->son[i]) now->son[i]->No++;
        }
		memcpy(now->value[pos + 1], x, len);
        now->son[pos + 1] = Son;
        now->to[pos + 1] = to;
    }
    void add(char * x, node * to){
    	//printf("ok\n");
        treenode * now = root;
        char * k = new char[len + 1];
        memcpy(k, x, len);
        k[len] = '\0';
		//printf("%s %d\n", k, len);
		//k = x;
        //int t;
        //memcpy((char *)&t, k, len);
        while(1){
            int pos = find(now, k);
            //printf("pos = %d\n", pos);
            //treenode * newnode = NULL;
			/*if(pos == -1 && t == 0){
                int k;
                k = 1;
			}*/
			//printf("%d\n", pos);
            if(now->isleaf){
                insert(now, pos, NULL, to, k);
                //printf("%d %d ok\n", now->num, n);
                while(now->num > n){
                    int numl = now->num / 2;
                    int numr = now->num - numl;
                    treenode * newnode = build(now->isleaf);
                    newnode->next = now->next;
                    now->next = newnode;
                    for(int i = numl;i < now->num;i++){
                        memcpy(newnode->value[i - numl], now->value[i], len);
						//newnode->value[i - numl] = now->value[i];//
						if(now->isleaf) newnode->to[i - numl] = now->to[i];
                        if(now->son[i]){
							now->son[i]->fa = newnode;
							now->son[i]->No = i - numl; 
						}
                        newnode->son[i - numl] = now->son[i];
                    }
                    now->num = numl;
                    newnode->num = numr;
                    //printnode(newnode);
                    if(now->fa == NULL){
                        treenode * rt = build(0);
                        now->fa = rt;
                        now->No = 0;
                        newnode->fa = rt;
                        newnode->No = 1;
                        memcpy(rt->value[0], now->value[0], len);
						//rt->value[0] = now->value[0];//
                        //rt->value[1] = newnode->value[0];//
                        memcpy(rt->value[1], newnode->value[0], len);
                        rt->son[0] = now;
                        rt->son[1] = newnode;
                        rt->num = 2;
                        root = rt;
                        break;
                    }
                    else{
                        //now = now->fa;
                        newnode->fa = now->fa;
                        pos = find(now->fa, newnode->value[0]);
                        insert(now->fa, pos, newnode, NULL, newnode->value[0]);
                        //now->fa->value[now->No] = now->value[0];
                        memcpy(now->fa->value[now->No], now->value[0], len);
                        newnode->No = now->No + 1/*pos + 1*/;
                        now = now->fa;
                    }
                }
                break;
            }
            if(pos == -1) pos = 0;
            now = now->son[pos];
        }
        //print(root); 
    }
    /*pos =  2
    0 1 2 3 4
    1 2 3 5*/
    void remove(treenode * now, int pos){
        now->num--;
        for(int i = pos;i < now->num;i++){
            //now->value[i] = now->value[i + 1];//
            memcpy(now->value[i], now->value[i + 1], len);
            now->son[i] = now->son[i + 1];
            now->to[i] = now->to[i + 1]; 
            if(now->son[i]) now->son[i]->No--;
        }
    }
    void deletes(char * x){
        treenode * now = root;
        while(1){
            int pos = find(now, x);
            if(now->isleaf){
                if(pos == -1) return;
				else if(cmp(now->value[pos], x)) remove(now ,pos);
				else return;
                while(1){
                    if(now->num < (n + 1) / 2 && now->fa != NULL){
                        int flag = 0;
                        if(now->No && now->fa->son[now->No - 1]->num > (n + 1) / 2){
                            treenode * left = now->fa->son[now->No - 1];
                            now->num++;
                            for(int i = now->num - 1;i > 0;i--){
                                now->son[i] = now->son[i - 1];
                                //now->value[i] = now->value[i - 1];
                                if(now->isleaf) now->to[i] = now-> to[i - 1]; 
                                memcpy(now->value[i], now->value[i - 1], len); 
                                if(now->son[i]) now->son[i]->No++; 
                            }
                            now->son[0] = left->son[left->num - 1];
                            memcpy(now->value[0], left->value[left->num - 1], len);
                            if(now->isleaf) now->to[0] = left->to[left->num - 1];
							//now->value[0] = left->value[left->num - 1];
                            if(now->son[0]){
                            	now->son[0]->No = 0;
                            	now->son[0]->fa = now;
							}
                            left->num--;
                            memcpy(now->fa->value[now->No], now->value[0], len);
							//now->fa->value[now->No] = now->value[0];
                            break;
                        }
                        else if(now->No != now->fa->num - 1 && now->fa->son[now->No + 1]->num > (n + 1) / 2){
                            treenode * right = now->fa->son[now->No + 1];
                            now->num++;
                            memcpy(now->value[now->num - 1], right->value[0], len);
							//now->value[now->num - 1] = right->value[0];
                            now->son[now->num - 1] = right->son[0];
                            if(now->isleaf) now->to[now->num - 1] = right->to[0];
                            if(right->son[0]){
                            	right->son[0]->fa = now;
                            	right->son[0]->No = now->num - 1;
							}
                            right->num--;
                            for(int i = 0;i < right->num;i++){
                                memcpy(right->value[i], right->value[i + 1], len);
								//right->value[i] = right->value[i + 1];//
                                right->son[i] = right->son[i + 1];
                                if(now->isleaf) right->to[i] = right->to[i + 1];
                                if(right->son[i]) right->son[i]->No--;
                            }
                            memcpy(right->fa->value[right->No], right->value[0], len);
							//right->fa->value[right->No] = right->value[0];
                            break;
                        }
                        else{
                            treenode * left;
                            treenode * right;
                            if(now->No){
                                left = now->fa->son[now->No - 1];
                                right = now;
                            }
                            else{
                                left = now;
                                right = now->fa->son[now->No + 1];
                            }
                            for(int i = 0;i < right->num;i++){
                            	memcpy(left->value[i + left->num], right->value[i], len);
                                //left->value[i + left->num] = right->value[i];
                                left->son[i + left->num] = right->son[i];
                                if(now->isleaf) left->to[i + left->num] = right->to[i];
                                if(right->son[i]){
									right->son[i]->No = i + left->num;
									right->son[i]->fa = left;
								}
                            }
                            left->num += right->num;
                            left->next = right->next;
                            remove(now->fa, right->No);
                        }
                        now = now->fa;
                    }
                    else if(now->fa == NULL && now->num == 1){
                        now->son[0]->fa = NULL;
                        root = now->son[0];
                        free(now);
                        break;
                    }
                } 
                break;
            }
            now = now->son[pos];
        }
    }
    result * query(char * x, string op){
    	treenode * now = root;
    	while(1){
    		int pos = find(now, x);
    		//printf("%d\n", pos);
    		if(now->isleaf){
    			//printf("ok\n");
    			treenode * start;
    			treenode * end;
    			int startpos, endpos;
				result * ret = NULL;
				result * nowresult = NULL;
    			if(op == ">" || op== ">="){
    				start = now;
    				if(pos == -1) startpos = 0;
    				else if(cmp(now->value[pos], x)) startpos = pos + (op == ">");
    				else startpos = pos + 1;
    				end = NULL;
    				endpos = 0;
				}
				else if(op == "<=" || op == "<"){
					start = begin;
					startpos = 0;
					end = now;
					if(pos == -1) return NULL;
					else if(cmp(now->value[pos], x)) endpos = pos - (op == "<");
					else endpos = pos;
				}
				else if(op == "="){
					if(pos == -1) return NULL;
					if(cmp(now->value[pos], x)){
						result * newresult = new result;
						newresult->next = NULL;
						newresult->to = now->to[pos];
						return newresult;
					}
					else return NULL;
					
				}
				//printf("endpos = %d\n", endpos);
				while(1){
					if(startpos == start->num){
						start = start->next;
						startpos = 0;
					}
					if(start == NULL) break;
					if(start == end && startpos > endpos) break;
					result * newresult = new result;
					newresult->next = NULL;
					newresult->to = start->to[startpos];
					if(nowresult == NULL){
						nowresult = newresult;
						ret = newresult;
					}
					else{
						nowresult->next = newresult;
						nowresult = nowresult->next;
					}
					/*int x;
					memcpy((char *)&x, start->value[startpos], len);
					if(type == "int") printf("%d\n", x);
					else if(type == "int") printf("%.2f\n", (double)(*(start->value[startpos])));
					else if(type == "char") cout << start->value[startpos] << endl;*/
					startpos++;
					if(start == end && startpos > endpos) break;
				}
				return ret;
			}
    		if(pos == -1) now = now->son[0];
			else now = now->son[pos];
		}
	}
    void printnode(treenode * k){
    	for(int i = 0;i < k->num;i++)
    	if(type == "int"){
    			int x;
    			memcpy((char *)&x, k->value[i], sizeof(int));
    			printf("%d %d-", k->No, x);
			}
			else if(type == "float"){
				double x;
				memcpy((char *)&x, k->value[i], sizeof(double));
				printf("%d %.2f-", k->No, x);
			}
			else if(type == "char"){
				char str[len + 1];
				memcpy(str, k->value[i], len);
				str[len] = '\0';
				string out = str;
				//string str = k->value[i];
				cout << k->No << ' ' << out << '-';
			}
		printf("\n");
	}
    void print(treenode * now){
    	//printf("Debug\n");
    	if(now->isleaf){
    		for(int i = 0;i < now->num;i++)
    		if(type == "int"){
    			int x;
    			memcpy((char *)&x, now->value[i], sizeof(int));
    			printf("%d ",x);
			}
			else if(type == "float"){
				double x;
				memcpy((char *)&x, now->value[i], sizeof(double));
				printf("%.2f ",x);
			}
			else if(type == "char"){
				char str[len + 1];
				memcpy(str, now->value[i], len);
				str[len] = '\0';
				string out = str;
				cout << out << ' ';
			}
			printf("\n");
			return;
		}
		for(int i = 0;i < now->num;i++){
			int x;
			memcpy((char *)&x, now->value[i], sizeof(int));
			printf("%d--%d ", now->son[i]->No, x);
		}
		printf("\n");
		for(int i = 0;i < now->num;i++) print(now->son[i]);
	}
};

/*int main(){
	//BPlusTree(string Type, int N, int Len)
	//ofstream out("data.txt");
	BPlusTree * kk;
	BPlusTree Tree("int", 5, 4);
	kk = &Tree;
	printf("%d\n", kk->len);
	srand((unsigned)time(NULL));
	int a[100];
	for(int i = 1;i <= 25;i++){
		//scanf("%d", &a[i]);
		//a[i] = rand() % 200;
		//printf("%d\n", a[i]);
		//out << a[i] << endl;
		int x;
		scanf("%d", &x);
		Tree.add((char *)&(x));
		Tree.print(Tree.root);
		int a;
		memcpy((char *)&a, Tree.root->value[0], sizeof(int));
		printf("a = %d\n",a);
		//system("pause");
	}
	int s = 88;
	Tree.query((char *)&s, "<");

	for(int i = 1;i < 20;i++){
		int x;
		scanf("%d", &x);
		Tree.deletes((char *)&x);
		Tree.print(Tree.root);
	}
	//for(int i = 26;i <= 50;i++){
		
	//}
}*/
