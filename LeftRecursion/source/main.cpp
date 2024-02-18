#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <cstring>

using namespace std;
const int maxn=100+5;
int row;
// 数据结构用于表示产生式规则
class Production{
public:
    string left;  //产生式左部
    vector<vector<string>> right; //产生式右部
    Production(const string& str){
        left=str;
        right.clear();
    }
    void push(const string& str,int n){
        right[n].push_back(str);
    }
//    void print(){
//        printf("%s->",left.c_str());
//        set<string>::iterator it = right.begin();
//        printf("%s",it->c_str());
//        it++;
//        for (;it!= right.end();it++ )
//            printf("|%s",it->c_str());
//        cout<<endl;
//    }
};

map<string,int> mp;  //记录每个production的下标
string token = "";      //token
vector<Production> allProductions;  //每一个产生式
string start;   //文法G[s]
vector<string> NT;
vector<string> T;
char rp;
void init(){
    mp.clear();
    allProductions.clear();
    start="S";
}
//判断是否是全大写
bool isAllUpperCase(const std::string& str) {
    for (char c : str) {
        if (!std::isupper(c)) {
            return false;
        }
    }
    return true;
}


void processLine(const string& line) {
    stringstream ss(line);
    ss >> token;
//    cout<<token<<endl;
    while (!ss.eof()){
        Production production(token);
        ss >> token;
        if (token != ":") {
            printf("false\n");
//            cout<<token<<endl;
            return;
        }
        int n = 0;
        vector<string> righttemp;
        production.right.push_back(righttemp);
        while (ss >> token && token != ";"){
////            int n = 0;
//            vector<string> righttemp;
//            production.right.push_back(righttemp);
            if (token == "|") {
                n++;
                vector<string> righttemp1;
                production.right.push_back(righttemp1);
                continue;
            } else {
                // 将右侧部分添加到当前 Production 的 right 中
                production.push(token,n);
//                cout<<token<<endl;
            }
        }
        allProductions.push_back(production);
        ss >> token;
    }
    return;
}
//消除间接左递归
void eliminateIndirectLeftRecursion(){
    for(int i = 0; i < allProductions.size(); i++){
        for(int j = 0; j < i; j++){
            vector<vector<string>> ans;
            vector<string> temp;
            vector<vector<string>>& righti = allProductions[i].right;
            vector<vector<string>>& rightj = allProductions[j].right;
            string ch = allProductions[j].left; //取所有Aj产生式的左部的非终结符
            for(const auto& productioni : righti){
                if(productioni[0] == ch){
                    for(const auto& productionj : rightj){
                        vector<string> temp = productionj;
                        temp.insert(temp.end(), productioni.begin() + 1, productioni.end());
                        ans.push_back(temp);
                    }
                } //如果当前产生式右部的非终结符和Aj相同
            }
            while (!righti.empty()) {
                // 检查 righti 中第一个字符串的第一个字符是否不等于 ch
                if (righti.begin()->at(0) != ch) {
                    // 存储当前没有替换的产生式右部
                    ans.push_back(*righti.begin());
                }
                // 删除 righti 中的第一个元素
                righti.erase(righti.begin());
            }
            for(int k = 0; k < ans.size(); k++)  //将替换过的产生式右部进行更新操作
                righti.push_back(ans[k]);
        }
    }
}
//消除直接左递归
void eliminateDirectLeftRecursion(){
    for(int i = 0;i < allProductions.size(); i++){
        string  ch = allProductions[i].left; // 取左部终结符
        vector<vector<string>>& righttmp = allProductions[i].right;  //拿到当前右部
        string tmp = allProductions[i].left+"New"; //对非终结符更改
        bool flag = true;
        for(const auto& production: righttmp){
            if(production.begin()->compare(ch) == 0){
                allProductions.push_back(Production(tmp));
                mp[tmp]=allProductions.size();
                flag = false;
                break;
            }
        }
        if(flag) continue; //对于非终结符不相同的产生式我们可以直接跳过
        int idx = mp[tmp] - 1;
        vector<vector<string>>& right = allProductions[i].right;
        vector<vector<string>> ans;
        vector<vector<string>>& tmpSet = allProductions[idx].right;
        while(!right.empty()){
            if(right.begin() -> at(0) == ch){
                vector<string> te = vector<string>(right.begin()->begin() + 1, right.begin()->end());
                te.push_back(tmp);
                tmpSet.push_back(te);
            }
            else{
                vector<string> tp = vector<string>(right.begin()->begin() , right.begin()->end());
                tp.push_back(tmp);
                ans.push_back(tp);
            }
            right.erase(right.begin());   //删除掉原本产生式右部
        }
        tmpSet.push_back(vector<string>{"Σ"});  //添加空字符
        for(int k=0;k<ans.size();k++)
            right.push_back(ans[k]);    //更新加入新的产生式右部
    }
}

void outputProductionsToConsole() {
    for (const Production& production : allProductions) {
        cout << production.left << " -> ";

        for (size_t i = 0; i < production.right.size(); ++i) {
            for (size_t j = 0; j < production.right[i].size(); ++j) {
                cout << production.right[i][j];
                if (j < production.right[i].size() - 1) {
                    cout << " ";
                }
            }

            if (i < production.right.size() - 1) {
                cout << " | ";
            }
        }

        cout << endl;
    }
}
void outputProductions(const string& filename) {
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        cerr << "Unable to open file: " << filename << endl;
        return;
    }

    for (size_t i = 0; i < allProductions.size(); ++i) {
        outputFile << allProductions[i].left << " : ";

        for (size_t j = 0; j < allProductions[i].right.size(); ++j) {
            for (size_t k = 0; k < allProductions[i].right[j].size(); ++k) {
                outputFile << allProductions[i].right[j][k];
                if (k < allProductions[i].right[j].size() - 1) {
                    outputFile << " ";
                }
            }

            if (j < allProductions[i].right.size() - 1) {
                outputFile << " \n\t\t| \t";
            } else {
                outputFile << "\n\t\t ; ";
            }
        }

        if (i < allProductions.size() - 1) {
            outputFile << "\n";
        }
    }

    outputFile.close();
}



int main() {
    string inputFileName, outputFileName;

    // 提示用户输入输入文件名
    cout << "Enter the input file name: ";
    cin >> inputFileName;

    ifstream file(inputFileName);
//    ifstream file("/home/zxp/CLionProjects/ll1/1.txt");
    if (!file.is_open()) {
        cerr << "Error: Unable to open the file.\n";
        return 1;
    }
    stringstream fileContent;
    string line;
    // 读取文件内容并保存在 stringstream 中
    while (getline(file, line)) {
        fileContent << line << "\n";
    }
    // 关闭文件
    file.close();
    // 处理文件内容
    cout << "Enter the output file name: ";
    cin >> outputFileName;
    cout << "Start LeftRecursion\n ";
    processLine(fileContent.str());
    int idx=allProductions.size()-1;
    start=allProductions[idx].left;
    eliminateIndirectLeftRecursion(); //消除间接左递归
    eliminateDirectLeftRecursion();
    outputProductions(outputFileName);
    outputProductionsToConsole();
    return 0;
}
