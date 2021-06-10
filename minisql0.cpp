#include <stdio.h>
#include <fstream>
#include <string.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include "BPlusTree.cpp"

#define type_length_all (name_length + type_length + char_length + isunique_length + isprimary_length + index_length + indexname_length)
#define name_length 20
#define type_length 5
#define char_length 4
#define isunique_length 4
#define isprimary_length 4
#define index_length 4
#define indexname_length 20
#define block_size 0x1000

using namespace std;

#define number_keyword 6
string key_word[] = {"char", "unique", "int", "float", "primary", "key", "\n"};
string operators[] = {"=", "<>", "<", ">", "<=", ">=", "\n"};

struct Index{
	string index_name;
	string table_name;
	string type_name;
	Index * next;
};
Index * index;

/*struct node{
    struct node * next;
    char * value;
};*/
/*struct tree{
    char * value;    
};*/

struct type_node{
    struct type_node *next;
    string name;
    string type;
    int len;
    int begin;
    int isunique;
    int isprimary;
    int index;
    string indexname;
    //tree * root;
    BPlusTree * tree;
};
struct check_node{
    type_node * node;
	string oper;
	string value;
};
struct Table{
	struct Table * next;
	struct node * list;
    struct node * tail;
	struct type_node * type;
	int typenum;
	int blocknum;
	int empty;
    int size;
	string name;
};


int ischar(char ch){
    return (ch == ' ') || (ch == ',') || (ch == '(') || (ch == ')') || (ch == ';') || (ch == '\t') || (ch == '\'');
}

int in(string str, string * a){
    int x = 0;
    transform(str.begin(), str.end(), str.begin(), towlower);
    while(*(a + x) != "\n"){
        if(*(a + x) == str) return 1;
        x++;
    }
    return 0;
}

int findstr(string str, string * a){
    int pos = 0, flag = 1, l = str.length(), sum = 0;
    while(pos < l){
        if(!ischar(str[pos])){
            int len = 1;
            while(!ischar(str[pos + len])) len++;
            a[sum++] = str.substr(pos, len);
            pos = pos + len -1;
        }
        else if(ischar(str[pos])) flag = 1;
        pos++;
    }
    return sum;
}

int strtoint(string str){
	int flag = 1;
	if(str[0] == '-') flag = -1;
    int ret = 0, len = str.length();
    for(int i = (flag == -1);i < len;i++) ret = ret * 10 + str[i] - '0';
    return flag * ret;
}

double strtofloat(string str){
	str = str + '.';
	int flag = 1;
	if(str[0] == '-') flag = -1;
	double ret = 0;
	int len = str.length(), x = (flag == -1);
	int pos = str.find('.') - (flag == -1); 
	double exp = pow(10, pos - 1);
	while(x < len){
		if(str[x] != '.'){
			ret = ret + (str[x] - '0') * exp;
			exp = exp / 10;
		}
		x++;
	}
	return flag * ret;
}

char * strtochar(string str, string type, int len){
	char * ret = new char[len];
	if(type == "char") memcpy(ret, str.c_str(), len);
    else if(type == "int"){
        int x = strtoint(str);
        memcpy(ret, (char *)&x, sizeof(int));
    }
    else if(type == "float"){
        double x = strtofloat(str);
        memcpy(ret, (char *)&x, sizeof(double));
    }
    return ret;
} 
/*
 =  0
 <> 1
 <  2
 >  3
 <= 4
 >= 5
*/
int cmp(char * x, string y, string oper, string type){
	//cout << x << oper << y << "!" << type << endl;
	int op = -1;
	for(int i = 0;i < 6;i++)
	if(oper == operators[i]){
		op = i;
		break;
	}
	if(type == "char"){
        string str = x;
		switch(op){
			case 0: return str == y;
			case 1:	return str !=y ;
			case 2:	return str < y;
			case 3: return str > y;
			case 4:	return str <= y;
			case 5:	return str >= y;
		}
	}
	else if(type == "int"){
        int a, b;
		memcpy((char *)&a, x, sizeof(int));
		b = strtoint(y);
        switch(op){
			case 0: return a == b;
			case 1:	return a != b;
			case 2:	return a < b;
			case 3: return a > b;
			case 4:	return a <= b;
			case 5:	return a >= b;
		}
	}
	else if(type == "float"){
        double a, b;
		memcpy((char *)&a, x, sizeof(double));
		b = strtofloat(y);
		//cout << y << endl; 
		//printf("a = %.2f b = %.2f\n",a ,b);
		switch(op){
			case 0: return a == b;
			case 1:	return a != b;
			case 2:	return a < b;
			case 3: return a > b;
			case 4:	return a <= b;
			case 5:	return a >= b;
		}
	} 
} 

class database{
public:
    Table * table;
    database(){
        table = new Table;
        table->next = NULL;
        table->list = NULL;
        table->type = NULL;
        table->typenum = 0;
    }
	
	void out(char * x, string type, int len){
		if(type == "int"){
			int a;
			memcpy((char *)&a, x, sizeof(int));
			cout << a;
		}
		else if(type == "double"){
			double a;
			memcpy((char *)&a, x, sizeof(double));
			cout << a;
		}
		else if(type == "char"){
			char str[len + 1];
			memcpy(str, x, len);
			str[len] = '\0';
			cout << str;
		}
	}
	
    Table * find_table(Table * k, string name){
    	while(k != NULL){
    		if(k->name == name) return k;
    		k = k->next;
		}
		return NULL;
	}
	
	type_node * find_row(type_node * k, string name){
        while(k != NULL){
            if(k->name == name) return k;
            k = k->next;
        }
        return NULL;
    }
	
	void addindex(node * k, type_node * type, string indexname){
    	//BPlusTree tree(type->type, 5, type->len);
    	type->index = 1;
    	type->indexname = indexname;
    	type->tree = new BPlusTree(type->type, 5, type->len);
    	
    	while(k != NULL){
    		char * x = new char[type->len];
    		memcpy(x, k->value + type->begin, type->len);
    		type->tree->add(x, k);
    		k = k->next;
		}
		//printf("%x %d\n",type->tree, type->tree->root->num);
		/*printf("-------------\n");
		type->tree->print(type->tree->root);
		int x = 10000;
		result * ans = type->tree->query((char *)&x, "=");
		printf("%d\n", ans == NULL);
		printf("-------------\n");*/
	}
    
	int addtable(string name){
        ifstream read(name + ".txt", ios::binary);
        if(!read.good()) return 0;
        Table * newtable = new Table;
        newtable->empty = block_size;
        newtable->next = table->next;
        table->next = newtable;
        newtable->name = name;
        read.read((char *)&newtable->blocknum, sizeof(int));
        read.read((char *)&newtable->typenum, sizeof(int));
        if(newtable->typenum) newtable->type = new type_node;
        else newtable->type = NULL;
        type_node * now = newtable->type;
        int sum = 0;
        for(int i = 0;i < newtable->typenum;i++){
        	//printf("%d\n", i);
            if(i){
                now->next = new type_node;
                now = now->next;
            }
            now->next = NULL;
            //typenode * k = new typenode; 
			
            char * buffer = new char[type_length_all];
            read.read(buffer, type_length_all);
            char * name = new char[name_length];
            memcpy(name, buffer, name_length);
            now->name = name;
            char * type = new char[type_length];
            memcpy(type, buffer + name_length, type_length);
            now->type = type;
            memcpy(&now->len, buffer + name_length + type_length, char_length);
            memcpy(&now->isunique, buffer + name_length + type_length + char_length, isunique_length);
            memcpy(&now->isprimary, buffer + name_length + type_length + char_length + isunique_length, isprimary_length);
            memcpy(&now->index, buffer + name_length + type_length + char_length + isunique_length + isprimary_length, index_length);
            //printf("!!%d\n",buffer[name_length + type_length + char_length + isunique_length + isprimary_length]);
            char * indexname = new char[indexname_length];
            memcpy(indexname, buffer + type_length_all - indexname_length, indexname_length);
            now->indexname = indexname;
            //printf("%s\n", indexname);
            
            now->begin = sum;
            sum = sum + now->len;
            //cout << now->len << " " << now->isunique << " " << now->name << " " << now->index << " " << now->indexname << endl;
            free(buffer);
            free(indexname);
            free(type);
            free(name);
        }
        newtable->size = sum;
        newtable->list = new node;
        node * list = newtable->list;
        int first = 1;
        for(int i = 0;i < newtable->blocknum;i++){
        	//printf("%d %d\n", i, newtable->blocknum);
            char * buffer = new char[block_size];
            read.read(buffer, block_size);
            int offset = 0, cnt = 0;
            while(1){
            	//printf("!!%d\n", cnt++);
                if(!buffer[offset]) break;
                if(!first){
                    list->next = new node;
                    list->next->last = list;
                    list = list->next;
                }
                else{
                	first = 0;
                	list->last = NULL;
				} 
				list->next = NULL;
				list->value = new char[newtable->size]; 
                memcpy(list->value, buffer + offset, newtable->size);
                offset = offset + newtable->size;
            }
            free(buffer);
            newtable->tail = list;
            newtable->empty = block_size - offset;
        }
        now = newtable->type;
        //cout << now->name << " " << now->index << endl;
        while(now != NULL){
        	if(now->index) addindex(newtable->list, now, now->indexname);
        	now = now->next; 
		}
        //????
        read.close();
        return 1;
    }

    void fileout(){
        Table * now = table->next;
        while(now){
            string name = now->name;
            ofstream out(name + ".txt", ios::binary);
            out.write((char *)&now->blocknum, sizeof(int));
            out.write((char *)&now->typenum, sizeof(int));
            type_node * type = now->type;
            while(type){
                out.write(type->name.c_str(), name_length);
                out.write(type->type.c_str(), type_length);
                out.write((char *)&type->len, char_length);
                out.write((char *)&type->isunique, isunique_length);
                out.write((char *)&type->isprimary, isprimary_length);
                out.write((char *)&type->index, index_length);
                out.write(type->indexname.c_str(), indexname_length);
                type = type->next;
            }
            node * list = now->list;
            int empty = block_size;
            char buffer[block_size];
            memset(buffer, 0, sizeof(buffer));
            while(list != NULL){
                if(empty < now->size){
                    out.write(buffer, block_size);
                    empty = block_size;
                    memset(buffer, 0, sizeof(buffer));
                }
                memcpy(buffer + block_size - empty, list->value, now->size);
                empty = empty - now->size;
                list = list->next;
            }
            out.write(buffer, block_size);
            out.close();
            now = now->next;
        }
    }

    void create_table(string * a, int n){
        Table * newtable = new Table;
        newtable->name = a[2];
        newtable->next = table->next;
        newtable->list = NULL;
        newtable->tail = NULL;
        newtable->size = 0;
        newtable->blocknum = 1;
        newtable->typenum = 0;
        newtable->empty = block_size;
        newtable->type = new type_node;
        table->next = newtable;
        int x = 3, isdigit = 0, item = 0, isprimary = 0, first = 1;
        type_node * now = newtable->type;
        while(x < n){
			//cout << x << " " << a[x] << endl;
            if(in(a[x], key_word)){
                if(a[x] == "char"){
                    now->type = "char";
                    isdigit = 1;
                }
                else if(a[x] == "int"){
                    now->type = "int";
                    now->len = sizeof(int);
                    newtable->size += sizeof(int);
                }
                else if(a[x] == "float"){
                    now->type = "float";
                    now->len = sizeof(double);
                    newtable->size += sizeof(double);
                }
                else if(a[x] == "unique") now->isunique = 1;
                else if(a[x] == "key") isprimary = 1;
            }
            else{
                if(isdigit){
                    isdigit = 0;
                    now->len = strtoint(a[x]);
                    newtable->size += now->len;
                }
                else if(isprimary){
                    isprimary = 0;
                    type_node * k = newtable->type;
                    while(k){
                        if(k->name == a[x]){
                            k->isprimary = 1;
                            break;
                        }
                        k = k->next;
                    }
                }
                else{
                	//printf("ok\n");
                    if(!first){
                        now->next = new type_node;
                        now = now->next; 
                    }
                    else first = 0;
                    now->begin = newtable->size;
                    now->index = 0;
                    now->name = a[x];
                    now->isprimary = 0;
                    now->isunique = 0;
                    now->len = 0;
                    now->next = NULL;
                    newtable->typenum++;
                }
            }
			x++;
        }
    }

    void droptable(string * a, int n){
    	string name = a[2];
    	Table * last = table;
    	Table * k = table->next;
    	Index * lastindex = NULL;
    	Index * nowindex = index;
    	while(nowindex != NULL){
    		if(nowindex->table_name == name){
    			if(lastindex == NULL) index = index->next;
    			else lastindex->next = nowindex->next;
			} 
			lastindex = nowindex;
			nowindex = nowindex->next;
		}
		while(k != NULL){
			if(k->name == name){
				last->next = k->next;
				free(k);
				cout << "success" << endl;
				return;
			}
		}
		if(remove(name.c_str())) cout << "success" << endl;
		else cout << "no such table" << endl;
	}
    
    int insert(string * a, int n){
    	//printf("work!\n");
        string name = a[2];
		Table * k = table->next;
		k = find_table(k, name);
		if(k == NULL){
			if(!addtable(name)){
				cout << "no such table!" << endl;
				return 0;
			}
			k = table->next;
			k = find_table(k, name); 
		}
		if((n - 4) != k->typenum){
			cout << "Column number mismatch!" << endl;
			return 0;
		}
		/*type_node * typenow = k->type;
		for(int i = 1;i <= k->type_num;i++)
		if(typenow->isunique || typenow->isprimary){
			if(typenow->index){
				result * ans = typenow->tree->query()
			}
		}*/
        /**/
		node * newnode = new node;
		newnode->value = new char[k->size]; 
		type_node * typenow = k->type;
		for(int i = 1;i <= k->typenum;i++){
			//cout << typenow->name << " " << typenow->type << " " << typenow->len << " " << a[i + 3] << " " << typenow->begin << endl;
			if(typenow->type == "char") memcpy(newnode->value + typenow->begin, a[i + 3].c_str(), typenow->len);
            else if(typenow->type == "int"){
                int x = strtoint(a[i + 3]);
                memcpy(newnode->value + typenow->begin, (char *)&x, sizeof(int));
            }
            else if(typenow->type == "float"){
                double x = strtofloat(a[i + 3]);
                memcpy(newnode->value + typenow->begin, (char *)&x, sizeof(double));
            }
            if(typenow->isunique || typenow->isprimary){
            	//printf("!!%x %x %d %d\n", typenow->tree, table->next->type->next->tree, typenow->tree->root->num, table->next->type->next->tree->root->num);
				char * query = new char[typenow->len];
            	memcpy(query, newnode->value + typenow->begin, typenow->len);
            	if(index){
            		result * ans = typenow->tree->query(query, "=");
            		if(ans){
            			cout << typenow->name << " is unique or primary key, but ";
						out(query, typenow->type, typenow->len);
						cout << " is already exists." << endl;
            			return 0;
					}
				}
				else{
					node * search = k->list;
					char * nowvalue = new char[typenow->len];
					while(search != NULL){
						memcpy(nowvalue, search->value + typenow->begin, typenow->len);
						if(cmp(nowvalue, a[i + 3], "=", typenow->type)){
							cout << typenow->name << " is unique or primary key, but ";
							out(query, typenow->type, typenow->len);
							cout << " is already exists." << endl;
            				return 0;
						}	
						search = search->next; 
					}
				}
			}
            typenow = typenow->next;
		}
		typenow = k->type;
		for(int i = 1;i <= k->typenum;i++){
			if(typenow->index){
				char * query = new char[typenow->len];
	            memcpy(query, newnode->value + typenow->begin, typenow->len);
	            typenow->tree->add(query, newnode);
	            /*printf("-------------\n");
				typenow->tree->print(typenow->tree->root);
				printf("-------------\n");*/
			}
			typenow = typenow->next;
		}
		if(k->tail){
			k->tail->next = newnode;
			newnode->last = k->tail; 
	        k->tail = k->tail->next;
	        k->tail->next = NULL;
	        //k->tail->value = new char[k->size];
	    } 
	    else{
	    	k->list = newnode;
	    	k->tail = k->list;
	    	k->tail->next = NULL;
	    	k->list->last = NULL;
	    	//k->tail->value = new char[k->size];
		} 
		
        if(k->empty < k->size){
        	k->empty = block_size - k->size;
        	k->blocknum++;
		}
		else k->empty -= k->size;
    }

    int deletes(string * a, int n){
        string name =  a[2];
        Table * target = table->next;
        target = find_table(target, name);
        if(target == NULL){
            if(!addtable(name)){
                cout << "no such table!" << endl;
                return 0;
            }
            target = table->next;
            target = find_table(target, name);
        }
        vector <check_node> check;
        check.clear();
        result * ans = NULL;
        int flag_index = 1;
        for(int i = 4;i < n;i++)
        if(in(a[i], operators)){
            type_node * k = find_row(target->type, a[i - 1]);
            if(flag_index == 1 && k->index == 1 && a[i] != "<>"){
            	flag_index = 0;
            	ans = k->tree->query(strtochar(a[i + 1], k->type, k->len), a[i]);
			}
            else check.push_back((check_node){k, a[i], a[i + 1]});
        }
        node * last = NULL;
        node * nowlist = target->list;
        result * nowresult = ans;
        node * now;
        while(1){
        	if((nowlist == NULL && flag_index == 1) || (nowresult == NULL && flag_index == 0)) break;
        	if(flag_index) now = nowlist;
        	else now = nowresult->to;
            int flag = 1;
            for(int i = 0;i < check.size();i++){
            	//printf("ok\n");
            	//printf("!!%d\n", check[i].node->len);
                char buffer[check[i].node->len + 1];
                memcpy(buffer, now->value + check[i].node->begin, check[i].node->len);
                
                buffer[check[i].node->len] = 0;
                if(!cmp(buffer, check[i].value, check[i].oper, check[i].node->type)){
                    flag = 0;
                    break;
                }
            }
            //printf("%d\n", flag);
            if(flag){
            	if(target->empty + target->size == block_size){
            		target->empty = block_size - block_size / target->size * target->size;
            		target->blocknum--;
				}
				else target->empty += target->size;
				/*if(now->next == NULL){
					node * 
				}
				else{
					memcpy(now->value, now->next->value, target->size);
					now->next = now->next->next;
				}*/
                node * k =now;
                if(!k->last){
					target->list = now->next;
					now->next->last = now->last; 
				}
                else if(!now->next){
					target->tail = last;
					now->last->next = now->next;
				}
                else{
                	now->last->next = now->next;
					now->next->last = now->last; 
				}
                //now = now->next;
                //free(k);
            }
    		if(flag_index) nowlist = nowlist->next;
    		else nowresult = nowresult->next;
        }
    }

    void select(string * a, int n){
        string name;
        for(int i = 0;i < n;i++)
        if(a[i] == "from"){
            name =  a[i + 1];
            break;
        }
        Table * target = table->next;
        target = find_table(target, name);
        if(target == NULL){
            if(!addtable(name)){
                cout << "no such table!" << endl;
                return;
            }
            target = table->next;
            target = find_table(target, name);
        }
        vector <type_node *> out;
        out.clear();
        if(a[1] == "*"){
            type_node * k = target->type;
            for(;k != NULL;k = k->next) out.push_back(k);
        }
        else{
            for(int i = 1;a[i] != "from";i++){
                type_node * k = find_row(target->type, a[i]);
                out.push_back(k);
            }
        }
        vector <check_node> check;
        check.clear();
        result * ans = NULL;
        int flag_index = 1;
        for(int i = 0;i < n;i++)
        if(in(a[i], operators)){
            type_node * k = find_row(target->type, a[i - 1]);
            if(flag_index == 1 && k->index == 1 && a[i] != "<>"){
            	flag_index = 0;
            	ans = k->tree->query(strtochar(a[i + 1], k->type, k->len), a[i]);
			}
            else check.push_back((check_node){k, a[i], a[i + 1]});
        }
        node * nowlist = target->list;
        result * nowresult = ans;
        node * now;
        //printf("!!!!%d\n", check.size());
        while(now != NULL){
        	if((nowlist == NULL && flag_index == 1) || (flag_index == 0 && nowresult == NULL)) break;
        	if(flag_index) now = nowlist;
        	else now = nowresult->to;
            int flag = 1;
            for(int i = 0;i < check.size();i++){
                char buffer[check[i].node->len + 1];
                memcpy(buffer, now->value + check[i].node->begin, check[i].node->len);
                buffer[check[i].node->len] = 0;
                if(!cmp(buffer, check[i].value, check[i].oper, check[i].node->type)){
                    flag = 0;
                    break;
                }
            }
            if(flag){
                for(int i = 0;i < out.size();i++){
                    if(out[i]->type == "char"){
                        char str[out[i]->len + 1];
                        str[out[i]->len] = 0;
                        memcpy(str, now->value + out[i]->begin, out[i]->len);
                        cout << str << " ";
                    }
                    else if(out[i]->type == "int"){
                        int x;
                        memcpy((char *)&x, now->value + out[i]->begin, out[i]->len);
                        cout << x << " ";
                    }
                    else if(out[i]->type == "float"){
                        double x;
                        memcpy((char *)&x, now->value + out[i]->begin, out[i]->len);
                        cout << x << " ";
                    }
                }
                cout << endl;
            }
            if(flag_index) nowlist = nowlist->next;
            else nowresult = nowresult->next;
        }
    }
	
    void create_index(string * a, int n){
        string table_name = a[4];//；
        Table * nowtable = table->next;
        nowtable = find_table(nowtable, table_name);
        if(nowtable == NULL){
            if(!addtable(table_name)){
                cout << "no such table!" << endl;
                return;
            }
            nowtable = table->next;
            nowtable = find_table(nowtable, table_name);
        }
        string type_name = a[5];
        type_node * type = find_row(nowtable->type, type_name);
        //cout << type->name << " " << type->index << endl;
        if((type->isunique || type->isprimary) && !type->index){
        	//BPlusTree tree(type->type, 5, type->len);
        	//type->tree = &tree;
        	//printf("what%d\n", nowtable->list == NULL);
			addindex(nowtable->list, type, a[2]);
		}
        else if(type->index){
        	cout << a[5] << " has index already!" << endl;
        	return;
		}
		else{
        	cout << a[5] << " is not unique or primary key!" << endl;
        	return;
		}
		/*type->indexname = a[2];
        node * now = nowtable->list;
        BPlusTree tree(type->type, 5, type->len);
        type->tree = &tree;
        //memset(root->value, 0, block_size);
        while(now != NULL){
        	char * x;
        	memcpy(x, now->value + type->begin, type->len);
            tree.add(x, now);
            now = now->next;
        }*/
        Index * newindex = new Index;
        newindex->index_name = a[2];
        newindex->table_name = table_name;
        newindex->type_name = type_name;
        newindex->next = index;
        index = newindex;
    }
   
    void dropindex(string * a, int n){
    	string name = a[2];
    	Index * k = index;
    	Index * last = NULL;
    	while(k->index_name != name){
    		last = k; 
			k = k->next;
		}
		if(k == NULL){
			cout << "no such index" << endl;
			return;
		}
		if(!last) index = index->next;
		else last->next = k->next;
		Table * nowtable = table->next;
		nowtable = find_table(nowtable, k->table_name);
		if(!nowtable){
			addtable(k->table_name);
			nowtable = table->next;
			nowtable = find_table(nowtable, k->table_name);
		}
		type_node * nowtype = find_row(nowtable->type, k->type_name);
		nowtype->index = 0;
	}
    
	void print(){
        Table * now = table->next;
        cout << now->name << endl;
        type_node * k = now->type;
        for(;k;k = k->next) cout << k->name << " " << k->type << " " << k->len << " " << k->isprimary << " " << k->isunique << endl;
        node * list = now->list;
        while(list){
        	for(int i = 0;i < now->size;i++) printf("%02X ", list->value[i]);printf("\n"); 
			list = list->next;
		}
    }
};


void readindex(){
	index = NULL;
	ifstream in("index.txt");
	Index * now = new Index;
	while(in >> now->index_name >> now->table_name >> now->type_name){
		now->next = index;
		index = now;
		now = new Index;
	}
	
}

void writeindex(){
	Index * now = index;
	ofstream out("index.txt");
	while(now){
		out << now->index_name << " " << now->table_name << " " << now->type_name << endl;
		now = now->next;
	}
}

int main(){
    string command = "";
    string a[50];
    readindex();
    database db;
    //printf("%lf\n", strtofloat("-17.05"));
    ifstream in("test.txt"); 
    while(1){
    	string str;
        if(!getline(in, str, '\n')) break;
        command = command + str;
        int len = command.length();
        if(command[len - 1] == ';'){
            int sum = findstr(command, a);
            //for(int i = 0;i < sum;i++) cout << a[i] << endl;
            command = "";
            if(a[0] == "create" && a[1] == "table") db.create_table(a, sum);
            //else if(a[0] == "create" && a[1] == "index" && a[3] == "on") db.create_index(a, sum);
            else if(a[0] == "insert" && a[1] == "into" && a[3] == "values") db.insert(a, sum); 
            else if(a[0] == "drop" && a[1] == "table") db.droptable(a, sum);
            else if(a[0] == "drop" && a[1] == "index") db.dropindex(a, sum);
            else if(a[0] == "delete" && a[1] == "from") db.deletes(a, sum);
            else if(a[0] == "select") db.select(a, sum);
            else if(a[0] == "create" && a[1] == "index" && a[3] == "on") db.create_index(a, sum);
            //break;
            //system("pause");
        }
        /*int a;
        cin >> a;
        cout << a + 3 << endl;*/
        //db.fileout(); 
    }
    writeindex();
    db.fileout();
}
