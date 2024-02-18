#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <stack>


using namespace std;

const int maxn=100+5;
int row;
// 数据结构用于表示产生式规则
class Production {
public:
    string left;  //产生式左部
    vector<vector<string>> right; //产生式右部
    Production(const string &str) {
        left = str;
        right.clear();
    }

    void push(const string &str, int n) {
        right[n].push_back(str);
    }
};
class DividedProduction {
public:
    std::string left;  // 单句产生式左部
    std::vector<std::string> right; // 单句产生式右部

    DividedProduction(const std::string& left, const std::vector<std::string>& right)
            : left(left), right(right) {}
    bool operator<(const DividedProduction& other) const {
        if (left != other.left) {
            return left < other.left;
        }
        return right < other.right;
    }
};
namespace std {
    template <>
    struct hash<DividedProduction> {
        size_t operator()(const DividedProduction& p) const {
            // 使用 std::hash 来计算哈希值，可以替换为更复杂的逻辑
            return std::hash<std::string>{}(p.left);
        }
    };

    template <>
    struct equal_to<DividedProduction> {
        bool operator()(const DividedProduction& lhs, const DividedProduction& rhs) const {
            // 简单地比较左部字符串
            return lhs.left == rhs.left;
        }
    };
}
unordered_map<string, set<string>> FIRST;
unordered_map<string, set<string>> FOLLOW;
multimap<DividedProduction, set<string>> SELECT;
map<pair<string ,string>, vector<string>> TABLE;// 预测分析表
vector<vector<string>> allProductionsRight;//全部右部
string startSymbol;
string token = "";      //token
vector<Production> allProductions;  //每一个产生式
vector<DividedProduction> alldividedproduction;
string start;   //文法G[s]
vector<string> NT;
vector<string> T;
char rp;
void init(){
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
void setstartSymbol(){
    if (!allProductions.empty()) {
        startSymbol = allProductions[0].left;
    } else {
        std::cerr << "Error: No productions available." << std::endl;
    }
}
bool isStringValid(const std::string& input) {
    const std::string validChars = ";|:Σ"; // 包含的有效字符

    for (char c : input) {
        if (validChars.find(c) == std::string::npos) {
            // 如果找不到当前字符在validChars中，说明不是有效字符
            return false;
        }
    }
    // 所有字符都是有效字符
    return true;
}
bool isStringInNT(const string& str) {
    return find(NT.begin(), NT.end(), str) != NT.end();
}
bool isStringInT(const string& str) {
    return find(T.begin(), T.end(), str) != T.end();
}
bool isValidNT(string token){
    if(isAllUpperCase(token) || isStringInNT(token)||  isStringValid(token)){
        return false;
    } else return true;
}
bool isValidT(string token){
    if(!isAllUpperCase(token) || isStringInT(token)||  isStringValid(token)){
        return false;
    } else return true;
}
//if(!isAllUpperCase(token)){
//NT.push_back(token);
//}
vector<vector<string>> getAllProductionsRight() {
    vector<vector<string>> allRight;
    for (const Production& production : allProductions) {
        allRight.insert(allRight.end(), production.right.begin(), production.right.end());
    }
    return allRight;
}
size_t getCount() {
    size_t totalElementCount = 0;

    // 遍历全局 unordered_map
    for (const auto& entry : FOLLOW) {
        // 对每个 set<string> 进行累加
        totalElementCount += entry.second.size();
    }

    return totalElementCount;
}
void processLine(const string& line) {
    stringstream ss(line);
    ss >> token;
    if(isValidNT(token)){
        NT.push_back(token);
    }
//    cout<<token<<endl;
    while (!ss.eof()){
        Production production(token);
        ss >> token;
        if (isValidNT(token)) {
            NT.push_back(token);
        }else if(isValidT(token)){T.push_back(token);}

        if (token != ":") {
            printf("false\n");
//            cout<<token<<endl;
            return;
        }
        int n = 0;
        vector<string> righttemp;
        production.right.push_back(righttemp);
        while (ss >> token && token != ";"){
            if (isValidNT(token)) {
                NT.push_back(token);
            } else if(isValidT(token)){T.push_back(token);}
            if (token == "|") {
                n++;
                vector<string> righttemp1;
                production.right.push_back(righttemp1);
                continue;
            } else {
                production.push(token,n);
            }
        }
        allProductions.push_back(production);
        ss >> token;
        if (isValidNT(token)) {
            NT.push_back(token);
        }else if(isValidT(token)){T.push_back(token);}

    }
    allProductionsRight = getAllProductionsRight();
    return;
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
vector<vector<string>> getRight(const string& nonTerminal){
    vector<vector<string>> tempRight;
    for (const Production& production : allProductions) {
        if (production.left == nonTerminal) {
            tempRight.insert(tempRight.end(), production.right.begin(), production.right.end());
        }
    }
    return tempRight;
}
bool deriveemptystring(string str){
    vector<vector<string>> rights = getRight(str);
    for(const auto& innerVector : rights){
        vector<string > production = innerVector;
        for(size_t i = 0; i < production.size(); ++i){
            if(production[i] == "Σ"){
                return true;
            }
            else if(isValidNT(production[i])){
                if (deriveemptystring(production[i])){
                    // 在这里判断是否是production的最后一个字符串
                    if(i == production.size() - 1){
                        // str 是 production 的最后一个字符串
                        return true;
                    }
                    continue;
                } else break;
            } else break;
        }
    }
    return false;
}

set<string> getNTFirstSet(string str){
    // 求解思路
    //1、获取vn的右部，进行分析
    //2、遍历右部，一个个右部分析，各自求解first集加入到results
    //2.1 遍历当前右部（一个个字符分析）
    //如果第一个字符是终结符，加入first集合并且跳出；（这里会添加多余的空符）
    //如果是非终结符，则递归处理；
    //如果非终结符可以推空还需要循环处理该右部的下一字符(如果有)
    //3、遍历结束，最后如果该字符不能推空，就要删除results中的空符；返回结果

    set<string> results;
    vector<vector<string>> rights = getRight(str);
    if(!rights.empty()){
        for(const auto& innerVector : rights){
            vector<string > production = innerVector;
            for (size_t i = 0; i < production.size(); ++i) {
                if (isStringInNT(production[i])) {
                    if (FIRST.find(production[i]) == FIRST.end()) {
                        set<string> resultnew = getNTFirstSet(production[i]);
                        results.insert(resultnew.begin(), resultnew.end());
                        FIRST.insert(pair<string, set<string>>(production[i], resultnew));
                    } else {
                        set<string> resultinsert = FIRST[production[i]];
                        results.insert(resultinsert.begin(), resultinsert.end());
                    }
                    // 如果这个字符可以推空，且后面还有字符，那么还需要处理下一个字符
                    if (deriveemptystring(production[i]) &&   i != production.size() - 1) {
                        continue;
                    } else break;
                } else{
                    results.insert(production[i]);
                    break;
                }
            }
        }
    }
            if(deriveemptystring(str)){
                results.erase("~");
            }

    return results;
}

void getFirstSet(){
    for (const auto& element : NT){
        set<string > first = getNTFirstSet(element);
        FIRST.insert(pair<string, set<string>>(element, first));
    }
    cout << endl << setw(50) << right << setfill('=') << "FISRT集分析====================" << endl;
    for (const auto& entry : FIRST) {
        cout << "FIRST[" << entry.first << "]: {";
        for (const auto& element : entry.second) {
            cout << element << " ";
        }
        cout << "}\n";
    }
}

set<string> getNTFollowSet(string str){
    //求解思路：
    //1、对于开始符号，把#加到results
    //2、遍历当前文法所有的右侧表达式，
    //2.1 遍历当前右部进行分析，如果发现了vn，则可进行下一步骤以获取results元素
    //如果当前字符vn是最后一个字符，说明位于句尾，则把#加入
    //否则遍历vn后的字符
    // 如果再次遇到vn，回退并退出循环进入外部循环
    // 如果遇到终结符，直接加入到results，并break退出循环
    // 否则就是非终结符，那么求其first集合，去掉空后加入到results
    // 此时还要考虑是继续循环还是跳出循环：
    //如果当前字符可以推空，而且不是最后一个字符，说明还要继续分析下一个字符
    //如果可以推空但是是最后一个字符，那么把#加入results
    //如果不可以推空，直接跳出循环即可（可以推空，后面字符的first集合才有可能作为vn的follow集合）
    //3、遍历完成，返回results；
    set<string> results;
    if (str == startSymbol){
        results.insert("#");
    }
    for (const auto& productionwhole : allProductions){
        vector<vector<string>> productionRight = getRight(productionwhole.left);
        for(const auto production :productionRight){
            for(auto it = production.begin(); it != production.end();){
                const auto& tmp = *it;
                if (tmp == str) {
                    if (it == std::prev(production.end())) {
                        results.insert("#");
                        break;
                    } else{
                        while (it != production.end()) {
                            ++it;
                            const auto& nextTmp = *it;
//                            if (nextTmp == str) {
//                                --it;
//                                break;
//                            }
                            if(isStringInNT(nextTmp)){
                                set<string> tmpresult = FIRST[nextTmp];
                                tmpresult.erase("Σ");
                                results.insert(tmpresult.begin(),tmpresult.end());
                                if(deriveemptystring(nextTmp)){
                                    if (it == std::prev(production.end())){
                                        results.insert("#");
                                        break;// 跳出循环
                                    }
                                } else break;

                            } else{
                                results.insert(nextTmp);
                                break;// 跳出循环
                            }
                        }
                    }
                }
                else {
                    ++it;
                }
            }
        }

    }
    return results;
}
void completeFollowSet(string str) {
    for (const auto &productionwhole: allProductions) {
        vector<vector<string>> productionRight = getRight(productionwhole.left);
        for(const auto production :productionRight){
            for(auto it = production.begin(); it != production.end();){
                const auto& tmp = *it;
                if (tmp == str){
                    if (it == std::prev(production.end())){
                        string left = productionwhole.left;
                        set<string> tmp_follow = FOLLOW[left]; // 获取左部的follow集合
                        set<string> follows = FOLLOW[tmp];// 获取自己的原来的follow集合
                        follows.insert(tmp_follow.begin(),tmp_follow.end());
                        FOLLOW[str]= follows;
                        break;
                    } else{
                        for (++it; it != production.end(); ++it){
                            const auto& nextTmp = *it;
                            if(deriveemptystring(nextTmp)){
                                if (it != std::prev(production.end())){
                                    continue;
                                } else{
                                    string left = productionwhole.left;
                                    set<string> tmp_follow = FOLLOW[left]; // 获取左部的follow集合
                                    set<string> follows = FOLLOW[tmp];// 获取自己的原来的follow集合
                                    follows.insert(tmp_follow.begin(),tmp_follow.end());
                                    FOLLOW[str]= follows;
                                    break;
                                }
                            } else break;
                        }
                    }
                }
                ++it;
            }
        }
    }
}
void buildFollowSet(){
    for (const auto& element : NT){
        set<string >follows = getNTFollowSet(element);
        FOLLOW.insert(pair<string, set<string>>(element, follows));

    }
    int oldcount =getCount();
    int newcount =-1;
    while (oldcount !=newcount){
        oldcount = getCount();
        for(const auto& element : NT){
            completeFollowSet(element);
        }
        newcount = getCount();
    }
    cout << endl << setw(50) << right << setfill('=') << "FOLLOW集分析====================" << endl;
    for (const auto& entry : FOLLOW) {
        cout << "FOLLOW[" << entry.first << "]: {";
        for (const auto& element : entry.second) {
            cout << element << " ";
        }
        cout << "}\n";
    }

}
void splitProductions() {
    // Iterate through each Production in allproduction
    for (const auto& production : allProductions) {
        const std::string& left = production.left;
        const std::vector<std::vector<std::string>>& right = production.right;

        // Iterate through each right-hand side in the production
        for (const auto& rightPart : right) {
            // Create a DividedProduction object for each right-hand side
            DividedProduction dividedProduction(left, rightPart);

            // Store the DividedProduction in alldividedproduction
            alldividedproduction.push_back(dividedProduction);
        }
    }
}
set<string>getSelect(DividedProduction production){
    // 1、得到产生式的left和right
    // 2、遍历右部产生式，首先分析右部第一个字符:right[0]
    // 如果是终结符：（如果为空符，则把follow(left)加入results，否则直接把该符号加入到results）,然后break
    // 如果是非终结符：把first(right[0])-'~'加入到results；如果还可以推空，则要继续往后看（continue）
    set<string> results;
    string left = production.left;
    vector<string> right = production.right;
        for (auto it = right.begin(); it != right.end(); ++it) {
            const auto &productiontmp = *it;
            if (isStringInNT(productiontmp)) {
                set<string> ch = FIRST.find(productiontmp)->second;
                ch.erase("Σ");
                results.insert(ch.begin(), ch.end());
                if (deriveemptystring(productiontmp)) {
                    if (next(it) == right.end()) {
                        set<string> ch = FOLLOW.find(left)->second;
                        results.insert(ch.begin(), ch.end());
                    } else{
                        continue;
                    }
                } else{
                    break;
                }
            } else{
                if (productiontmp == "Σ"){
                    set<string> ch = FOLLOW.find(left)->second;
                    results.insert(ch.begin(), ch.end());
                } else{
                    results.insert(productiontmp);
                }
                break;
            }
        }
    return results;
}

void buildSelect() {
    for (const auto production: alldividedproduction) {
        set<string> selects = getSelect(production);
        SELECT.insert(make_pair(production, selects));
    }
    cout << endl << setw(50) << right << setfill('=') << "SELECT集分析====================" << endl;
    for (const auto& entry : SELECT) {
        const DividedProduction& dividedProduction = entry.first;
        const std::set<std::string>& selectSet = entry.second;

        // Print the left-hand side of the divided production
        std::cout << dividedProduction.left << " -> ";

        // Print the right-hand side of the divided production
        for (const auto& symbol : dividedProduction.right) {
            std::cout << symbol << " ";
        }

        // Print the select set
        std::cout << " {";
        for (const auto& symbol : selectSet) {
            std::cout << symbol << " ";
        }
        std::cout << "}" << std::endl;
    }

}
bool haveIntersection(const std::set<std::string>& set1, const std::set<std::string>& set2) {
    std::set<std::string> intersection;
    std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(intersection, intersection.begin()));

    return !intersection.empty();
}

void isLL1(){
    // 通过嵌套循环SELECT集合，判断不同的表达式但左部相同时的SELECT集合之间相交是否有交集
    // 如果有交集说明不是LL1，否则是LL1分析
    for (auto production1 = SELECT.begin(); production1 != SELECT.end(); ++production1){
        for (auto production2 = SELECT.begin(); production2 != SELECT.end(); ++production2){
            string left1 = production1->first.left;
            string left2 = production2->first.left;
            if(left1 == left2){
                if(production1->first.right != production2->first.right){
                    if(haveIntersection(production1->second,production2->second)){
                        cout << "经过分析，您输入的文法不符合LL(1)文法，请修改后重试" << endl;  exit(0); // 直接退出

                    }

                }
            }
        }
    }
    cout << setw(50) << right << setfill('=') << "经过分析，您输入的文法符合LL(1)文法====================" << endl << endl;
}
void buildTable() {
    cout << "正在为您构造分析表" << endl;
    string left;
    vector<string> right;
    set<string> selectset;

    for (auto iter = SELECT.begin(); iter != SELECT.end(); ++iter) {
        left = iter->first.left;
        right = iter->first.right;
        selectset = iter->second;

        for (const auto& select : selectset) {
            // 过滤掉右部中的空元素
            right.erase(std::remove(right.begin(), right.end(), ""), right.end());
            TABLE.emplace(make_pair(left, select), right);
        }
    }
}

void printTable(){
    cout << setw(60) << right << setfill('=') << "预测分析表====================" << endl;
    cout << setw(9) << left << setfill(' ') << "NT/T";
//    for (const auto& str : T) {
//        std::cout << str << " ";
//    }
    T.push_back("#");
    vector<string > t = T;
    for(const auto str :t){
        cout << setw(12) << left << "        " << str;
    }
    cout << endl << endl;
    for (const auto& str1 : NT) {
        std::cout << std::setw(7) << std::left << str1<< " " ;
        for (const auto& str2 : t) {
            for (const auto &entry: SELECT) {
                const DividedProduction &dp = entry.first;
                const set<string> &symbols = entry.second;
                if (dp.left == str1 && symbols.find(str2) != symbols.end()) {
                    for (const auto &str2: t) {
                        auto pos = std::find(t.begin(), t.end(), str2);
                        if (pos != t.end()) {
                            int index = std::distance(t.begin(), pos);
                            std::cout << std::string(10 * index, '   ');
                        }
                    }
                    for (const auto& element : dp.right) {
                        std::cout << element << " ";
                    }

                }

            }

        }  cout<<endl;
    }
}


std::string reverseAndJoin(const stack<string>& inputStack) {
    stack<string> reversedStack = inputStack; // 复制输入栈，以免修改原始数据
    // 逆序栈
    stack<string> tempStack;
    while (!reversedStack.empty()) {
        tempStack.push(reversedStack.top());
        reversedStack.pop();
    }
    reversedStack = tempStack;
    // 使用空格连接字符串
    ostringstream oss;
    while (!reversedStack.empty()) {
        oss << reversedStack.top() << " ";
        reversedStack.pop();
    }
    // 获取最终结果字符串
    string result = oss.str();
    // 去除末尾多余的空格
    result.pop_back();
    return result;
}
std::string concatenateWithSpace(const std::vector<std::string>& inputVector) {
    // 使用ostringstream来拼接字符串
    std::ostringstream oss;

    // 遍历向量，将每个字符串加入到oss中，并在字符串之间加入空格
    for (const auto& str : inputVector) {
        oss << str << " ";
    }

    // 获取最终结果字符串
    std::string result = oss.str();

    // 去除末尾多余的空格
    result.pop_back();

    return result;
}
void tableAnalyze(){
    // 求解思路：
    //1、构建先进后出栈，将#、S进栈
    //2、遍历句子，一个个符号送a;和栈顶送X，进入分析
    // 2.1 如果X是终结符
    // 如果和a相等，说明匹配成功：X出栈，并读取下一个字符
    // 否则是无法匹配：失败退出
    // 2.2 如果X是末尾符
    // a也是末尾符，接受分析字符串：成功退出
    // a不是末尾符，不接受分析字符串，失败退出
    // 2.3 否则X就是非终结符
    // 查找预测分析表，看是否有表达式
    // 如果没有，分析出错，失败退出
    // 如果有，X元素出栈，表达式逆序进栈，继续循环句子且要重复分析a
    //3、遍历完成，程序结束
    cout << "构造完成，请输入您要分析的字符串..." << endl;
    vector<std::string> teststr;
    ifstream inputFile("/home/zxp/Desktop/LL1 Analyze/testString.txt"); // 替换为你的文件名
    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            std::istringstream iss(line);
            std::string token;
            while (iss >> token) {
                teststr.push_back(token);
            }
        }
        inputFile.close();
    } else {
        std::cerr << "Unable to open the file." << std::endl;
    }
//    // 打印读取的字符串
//    for (const auto& str : teststr) {
//        std::cout << str;
//    }
    teststr.push_back("#");
    cout << "正在分析..." << endl;
    cout << endl << setw(170) << right << setfill('=') << "分析过程==============================================================================================" << endl;
    cout << setw(16) << left << setfill(' ') << "步骤";
    cout << setw(80) << left << setfill(' ') << "分析栈";
    cout << setw(80) << left << setfill(' ') << "剩余输入串";
    cout << setw(16) << left << setfill(' ') << "分析情况" << endl;


    stack<string> analyze;//分析栈
    analyze.push("#");
    analyze.push(startSymbol );
    int step =1;
    stack<string > remain = analyze;//剩余分析栈
    vector<string> testremain = teststr;//剩余输入串
    string str_situation = "待分析"; // 分析情况

    cout << setw(10) << left << setfill(' ') << step;
    cout << setw(80) << left << setfill(' ') << reverseAndJoin(remain);
    cout << setw(80) << left << setfill(' ') << concatenateWithSpace(testremain);
    cout << setw(16) << left << setfill(' ') << str_situation << endl;

    for(auto it=teststr.begin();it != teststr.end();){
        string tmp =*it;
        string TMP = analyze.top();
        if (isStringInT(TMP) && !( TMP == "#" && tmp == "#") ){
            if(TMP == tmp ){
                analyze.pop();
                for (auto it = testremain.begin(); it != testremain.end(); ++it) {
                    if (*it == tmp) {
                        testremain.erase(it);
                        break;
                    }
                }
                string msg =" 成功匹配" + tmp;
                str_situation = msg;
                ++it;

            }else { // 无法匹配，分析出错
                cout << "分析出错:" <<TMP<<"和" <<tmp<< "不匹配" << endl;
                exit(-1); // 出错退出
            }
        } else if (TMP  == "#"){
            if (tmp == "#") // 当前符号也是最后一个符号 ， 接受分析结果
            {
                cout << "分析结束，当前文法接受您输入的字符串" << endl;
                exit(0); // 成功退出
            }
            else {
                cout << "分析出错，文法结束输入串未结束" << endl;
                exit(-1);
            }
        } else{
            if(TABLE.find(pair<string ,string>(TMP,tmp)) == TABLE.end()){
                if(!deriveemptystring(TMP)){
                    cout << "分析出错,找不到表达式" << endl;
                    exit(-1); // 失败退出
                } else{
                    analyze.pop();
                    str_situation.clear();
                    str_situation+=TMP;
                    str_situation+="->";
                    str_situation+="Σ";

                }
            } else{
                analyze.pop();
                vector<string> str1 = TABLE.find(pair<string ,string>(TMP,tmp))->second;
                str_situation.clear();
                str_situation+=TMP;
                str_situation+= "->";
                string str2= concatenateWithSpace(str1);
                str_situation+=str2;
                reverse(str1.begin(),str1.end());
                for (auto iiter = str1.begin(); iiter != str1.end(); ++iiter)
                {
                    if (*iiter != "Σ")
                    {
                        analyze.push(*iiter);
                    }
                }


            }
        }
        ++step; // 步骤数加1
        remain = analyze;   // 置剩余栈为当前栈
        cout << setw(10) << left << setfill(' ') << step;
        cout << setw(80) << left << setfill(' ') <<reverseAndJoin(remain);
        cout << setw(80) << left << setfill(' ') << concatenateWithSpace(testremain);
        cout << setw(16) << left << setfill(' ') << str_situation << endl;


    }



}
    ;



    int main() {
        string inputFileName, outputFileName;
//        cout << "Enter the input file name: ";
//        cin >> inputFileName;
        inputFileName ="/home/zxp/Desktop/LL1Generater/2.txt";
        ifstream file(inputFileName);
        if (!file.is_open()) {
            cerr << "Error: Unable to open the file.\n";
            return 1;
        }
        stringstream fileContent;
        string line;
        while (getline(file, line)) {
            fileContent << line << "\n";
        }
        file.close();
        processLine(fileContent.str());
        setstartSymbol();
        outputProductionsToConsole();
        cout << "Strings in NT:" << endl;
        for (const string& str : NT) {
            cout << str << endl;
        }

        splitProductions();
//        for (const auto& dividedProduction : alldividedproduction) {
//            // Print the left-hand side of the divided production
//            std::cout << dividedProduction.left << " -> ";
//
//            // Print the right-hand side of the divided production
//            for (const auto& symbol : dividedProduction.right) {
//                std::cout << symbol << " ";
//            }
//
//            std::cout << std::endl;
//        }
        getFirstSet();
        buildFollowSet();
        buildSelect();
        isLL1();
        buildTable();
        for (const auto& entry : TABLE) {
            const auto& key = entry.first;
            const auto& value = entry.second;

            std::cout << "Key: {" << key.first << ", " << key.second << "} -> Value: {";

            for (const auto& production : value) {
                std::cout << production << ", ";
            }

            std::cout << "}" << std::endl;
        }
        printTable();
        tableAnalyze();
        return 0;
    }