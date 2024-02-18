#include <iostream>
#include <string>
#include <vector>
using namespace std;


//2023/11/11
enum TokenID{
    TK_SEMICOLON = 0,           //分号
    TK_DERIVES ,           //冒号
    TK_ALSODERIVES,         //或符
    KW_EPSILON,              //epsilon关键字
    TK_SYMBOL,               //字符串
    TK_EOF    ,          //终结符
    TK_UNDEF         //未识别标识符

};

TokenID tokencode = TK_UNDEF;   //获取tokenid
int row = 1;        //记录读取行以便于报错
string token = "";      //token
vector<string> keyword = {"EPSILON","epsilon","Epsilon"};


//判断是否为关键字
bool isKey (string token){
    for(const auto& str : keyword){
        if (str == token){
            return true;
        }
    }
    return false;
};


//判断是否为字母
bool isLetter(char letter)
{
    if ((letter >= 'a'&&letter <= 'z') || (letter >= 'A' &&letter <= 'Z'))
        return true;
    return false;

}
//判断数字
bool isNumber(char number)
{
    if (number >= '0'&& number <= '9')
        return true;
    return false;
}
void output(FILE *outputFile, TokenID tokencode) {
    fprintf(outputFile, "(%d, %s)\n", tokencode, token.c_str());
}

void WA(FILE *fp,FILE *outputFile){
    char rp;//read pointer
    while ((rp = fgetc(fp)) != EOF){
        token = rp;							//存储token
        if (rp == ' ' || rp == '\t' || rp == '\n')      //跳过空格，回行，tab
        {
            if (rp == '\n')				//记录行
                row++;
            continue;
        }
//        else if (isKey(token)){
//        }
        else if (isLetter(rp)|| isNumber(rp)){
            token = "";
            while (isLetter(rp) || isNumber(rp)){
                token.push_back(rp);
                rp = fgetc(fp);
            }
            if(rp == ' ' || rp == '\t' || rp == '\n'){
                fseek(fp, -1L, SEEK_CUR);  //读取完毕当前字符串，但是最后一次循环多读了一个字符，需要回退一个字符
                if (isKey(token)){
                    tokencode =  KW_EPSILON;
                }
                else{
                    tokencode = TK_SYMBOL;
                }
            }
            else{
                tokencode = TK_UNDEF;
                string s1 = "An error appears in row :";
                string s2 = "   the accept part is:";
                cout<<s1<<row<<s2<<token<<'\t';
                token = "UNDEF";
            }

        }

        else switch (rp) {
                /*运算符*/
                case '|':
                    tokencode = TK_ALSODERIVES;        //识别|
                    break;
                case ';':
                    tokencode = TK_SEMICOLON;        //识别;
                    break;
                case ':':
                    tokencode = TK_DERIVES;        //识别:
                    break;
                default:
                    tokencode = TK_UNDEF;
                    token = "UNDEFINE";
            }

        output(outputFile, tokencode);
    }
    tokencode = TK_EOF;
    token = "EOF";
    output(outputFile, tokencode);
}



int main() {
    string stmt = "(tokenID,token)";
    cout << stmt << endl;
    const char* filename = "1.txt";
    FILE *fp = fopen(filename, "r");
    // 打开输出文件
    const char* outputFilename = "output.txt";
    FILE *outputFile = fopen(outputFilename, "w");
    WA(fp, outputFile);
    // 关闭文件
    fclose(fp);
    fclose(outputFile);
    return 0;
}
