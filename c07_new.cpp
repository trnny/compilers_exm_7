#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <set>
#include <map>
#include <vector>
#include <stack>

using namespace std;

/**
 * 判断两集合是否相等
 */
template <typename T>
bool set_equals(const set<T>& s1, const set<T>& s2){
    auto i1 = s1.begin(), i2 = s2.begin();
    while(i1 != s1.end() && i2 != s2.end()){
        if(*i1 != *i2) return false;
        i1++;
        i2++;
    }
    if(i1 != s1.end() || i2 != s2.end()) return false;
    return true;
}

/**
 * 判断dfaI是否是终止状态
 * nfaZ是nfa的终止状态集
 */
bool setZ(const set<int>& nfaZ, const set<int>& dfaI){
    auto iter = nfaZ.begin();
    while (iter != nfaZ.end())
    {
        if(dfaI.find(*iter) != dfaI.end()) return true;
        iter++;
    }
    return false;
}

/**
 * 给定一个状态集,获得状态集编号
 * 若不在状态集则加入
 */
int setNUM(vector<set<int>>& s, const set<int>& t){
    int i = 0;
    auto iter = s.begin();
    while(iter != s.end()){
        if(set_equals(*iter, t)) 
            return i;
        i++;
        iter++;
    }
    s.push_back(t);
    return i;
}

/**
 * 将源字符串rstr以分隔串sep切割,得到的子串存入strv中
 */
void split(const string& rstr, vector<string>& strv, const char* sep = "") {
    strv.clear();
    int s = strlen(sep);
    if(s == 0){
        char* t = (char*)rstr.c_str();
        while (*t)
        {
            strv.push_back({*t,0});
            t++;
        }
        return;
    }
    int beg = 0, end = 0;
    while(1) {
        end = rstr.find(sep, beg);
        if(end == string::npos){
            strv.push_back(rstr.substr(beg));
            return;
        }
        strv.push_back(rstr.substr(beg, end - beg));
        beg = end + s;
    }
}

class NFA{
public:
    /**
     * 其中I是一个集合
     * 表示I中每个状态经过任意条ε弧后，所能达到的状态的集合。
     */
    set<int> e_closure(const set<int>& I) const{
        if(E.find("ε") == E.end()) 
            return I;
        set<int> res;
        stack<int> T;
        auto iter = I.begin();
        while (iter != I.end())
        {
            T.push(*iter);
            iter++;
        }
        while(!T.empty()){
            int t = T.top();
            res.insert(t);
            T.pop();
            auto a = _nfa.at(t).at("ε");
            auto iter = a.begin();
            while (iter != a.end())
            {
                T.push(*iter);
                iter++;
            }
        }
        return res;
    };
    /**
     * 集合中只有一个状态时可以用状态而不用集合
     */
    set<int> e_closure(int I = 0) const {
        set<int> res;
        if(E.find("ε") == E.end()) {
            res.insert(I);
            return res;
        }
        stack<int> T;
        T.push(I);
        while(!T.empty()){
            int t = T.top();
            res.insert(t);
            T.pop();
            auto a = _nfa.at(t).at("ε");
            auto iter = a.begin();
            while (iter != a.end())
            {
                T.push(*iter);
                iter++;
            }
        }
        return res;
    };
    // ---- nfa ----
    map<int, map<string, set<int>>> _nfa;
    // 所有状态的集合
    set<int> K;
    // 所有可接受的输入符号的集合
    set<string> E;
    /**
     * 其中I是一个集合，a是一条弧。
     * 表示I中每个状态经过一条a弧后，所能达到的状态的集合。
     */
    set<int> Move(const set<int> & I, string a) const{
        set<int> res;
        auto iter = I.begin();
        while (iter != I.end())
        {
            auto l = _nfa.at(*iter).at(a);
            auto i = l.begin();
            while(i != l.end()){
                res.insert(*i);
                i++;
            }
            iter++;
        }
        return res;
    }
    // K中的初始状态
    int S;
    /* K中的终止状态集合 */
    set<int> Z;
};

/**
 * nfa数据清掉
 */
void clearNFA(NFA& nfa){
    nfa._nfa.clear();
    nfa.K.clear();
    nfa.E.clear();
    nfa.S = 0;
    nfa.Z.clear();
}

/**
 * 从文件名fname读取nfa信息保存到nfa中
 */
bool readNFA(string fname, NFA& nfa) {
    ifstream input(fname);
    if (!input.is_open()) {
        cout << "打开文件" << fname << "失败!" << endl;
        return false;
    }
    clearNFA(nfa);
    vector<string> temp_vec;
    string temp_str;
    getline(input, temp_str);
    split(temp_str, temp_vec, ",");
    int sta_n = atoi(temp_vec[0].c_str());  // 读取:状态数
    int inc_n = temp_vec.size() - 1;
    if(inc_n < 1){
        cout << "文件内容有误!" << endl;
        input.close();
        return false;
    }
    vector<string> input_char_v;
    for(int i = 0;i < inc_n; i++){          // 读取:输入字母表
        temp_str = temp_vec[i+1];
        nfa.E.insert(temp_str);
        input_char_v.push_back(temp_str);
    }
    for(int i = 0;i < sta_n; i++){          // 读取:状态转换表
        nfa.K.insert(i);
        if(!getline(input, temp_str)){
            cout << "文件内容有误!" << endl;
            input.close();
            return false;
        }
        split(temp_str, temp_vec, ";");
        map<string, set<int>> t1;
        for(int i = 0;i < inc_n; i++){
            set<int> t2;
            if(temp_vec[i] != "void"){
                temp_str = temp_vec[i];
                vector<string> temp_str_int;
                split(temp_str, temp_str_int, ",");
                for(int i = 0; i < temp_str_int.size(); i++){
                    t2.insert(atoi(temp_str_int[i].c_str()));
                }
            }
            t1[input_char_v[i]] = t2;
        }
        nfa._nfa[i] = t1;
    }
    nfa.S = 0;
    if(getline(input, temp_str)){       // 读取:接受状态集合
        split(temp_str, temp_vec, ",");
        for(int i = 0;i < temp_vec.size();i++){
            nfa.Z.insert(atoi(temp_vec[i].c_str()));
        }
    }
    input.close();
    return true;
}

class DFA{
public:
    // ------ dfa ------
    map<int, map<string, int>> _dfa;
    // 所有状态集合
    set<int> K;
    // 所有可接受的输入符号的集合
    set<string> E;
    // K中的初始状态
    int S;
    // K中的终止状态集合
    set<int> Z;
};

/**
 * 将dfa数据清除
 */
void clearDFA(DFA& dfa){
    dfa._dfa.clear();
    dfa.K.clear();
    dfa.E.clear();
    dfa.S = 0;
    dfa.Z.clear();
}

/**
 * 将NFA转换成DFA
 */
void NFA2DFA(const NFA& nfa, DFA& dfa){
    clearDFA(dfa);
    dfa.E = nfa.E;
    dfa.E.erase("ε");
    set<int> I;
    vector<set<int>> s;
    setNUM(s, nfa.e_closure());
    int i = 0;
    dfa.S = 0;
    while(i < s.size()){
        dfa.K.insert(i);
        I = s[i];
        if(setZ(nfa.Z, I)) 
            dfa.Z.insert(i);
        auto iter = dfa.E.begin();
        while (iter != dfa.E.end())
        {
            dfa._dfa[i][*iter] = setNUM(s, nfa.e_closure(nfa.Move(I, *iter)));
            iter++;
        }
        i++;
    }
}

/**
 * 把DFA状态转换表在控制台输出
 * dfa状态编号用与A中对应的字符替换输出
 */
void printDFA(const DFA& dfa, const char* A = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"){
    cout << "  状态  ";
    auto iter = dfa.E.begin();
    while (iter != dfa.E.end())
    {
        cout << "   " << *iter << "   ";
        iter++;
    }
    auto i = dfa.K.begin();
    while (i != dfa.K.end())
    {
        cout << "\n   ";
        if(*i < strlen(A))
            cout << A[*i];
        else
            cout << *i;
        cout << "    ";
        iter = dfa.E.begin();
        while (iter != dfa.E.end())
        {
            cout << "   ";
            int p = dfa._dfa.at(*i).at(*iter);
            if(p < strlen(A))
                cout << A[p];
            else
                cout << p;
            cout << "   ";
            iter++;
        }
        i++;
    }
    cout << endl;
};

int main(){
    NFA nfa;
    DFA dfa;
    readNFA("input.nfa", nfa);
    NFA2DFA(nfa, dfa);
    printDFA(dfa);
    cout << "--------------------- \n";
    readNFA("test1_input.nfa", nfa);
    NFA2DFA(nfa, dfa);
    printDFA(dfa);
    cout << "--------------------- \n";
    readNFA("test2_input.nfa", nfa);
    NFA2DFA(nfa, dfa);
    printDFA(dfa);
    cout << "--------------------- \n";
    readNFA("ic.nfa", nfa);
    NFA2DFA(nfa, dfa);
    printDFA(dfa);
    return 0;
}