
#ifndef LOGMANAGER
#define LOGMANAGER
#include <bits/stdc++.h>
#endif

using namespace std;

string out(char * x, string type, int len){
  ostringstream oss;
  string str_out;
  if(type == "int"){
    int a;
    memcpy((char *)&a, x, sizeof(int));
    oss << a;
    str_out = oss.str();
  }
  else if(type == "double"){
    double a;
    memcpy((char *)&a, x, sizeof(double));
    oss << a;
    str_out = oss.str();
  }
  else if(type == "char"){
    char str[len + 1];
    memcpy(str, x, len);
    str[len] = '\0';
    str_out = str;
  }
  return str_out;
}

class LogManager{
public:
  int STATUS;
  string MSG;
  LogManager(){
    this->STATUS = 0;
    this->MSG = "";
  };
  void setStatus(int status);
  void setMsg(string msg);
  void logMsg();
  void setOutMsg(string name, char* query, string type, int len);
};



void LogManager::setStatus(int status){
  this->STATUS = status;
}

void LogManager::setMsg(string msg){
  this->MSG = msg;
}

void LogManager::setOutMsg(string name, char* query, string type, int len){
  string str = name + " IS UNIQUE OR PRIMARY KEY, BUT " + out(query, type, len) + " IS ALREADY EXISTS.";
  this->MSG = str;
}
/*
  2xx:函数运行正确
  3xx:指令格式出错 syntax error
  4xx:SQL逻辑错误 sql error
  5xx:程序错误 internal error
*/
void LogManager::logMsg(){
  switch (this->STATUS)
  {
  case 200:
    cout << "SUCCESS: ";
    cout << this->MSG << endl;
    break;
  case 300:
    cout << "SYNC ERROR: ";
    cout << this->MSG << endl;
    break;
  case 400:
    cout << "SQL ERROR: ";
    cout << this->MSG << endl;
    break;
  case 401:
    cout << "SQL ERROR: TABLE IS ALREADY EXISTS" << endl;
    break;
  case 402:
    cout << "SQL ERROR: NO SUCH TABLE EXISTS" << endl;
    break;
  case 403:  
    cout << "SQL ERROR: " + this->MSG <<endl;
    break;
  case 404:
    cout << "SQL ERROR: INDEX" + this->MSG + "IS ALREADY EXISTS" <<endl;
    break;
  case 405:
    cout << "SQL ERROR: " + this->MSG + "IS UNIQUE OR PRIMARY KEY" <<endl;
    break;
  default:
    cout << "UNDEFINED STATUS CODE" << endl;
    break;
  }

}