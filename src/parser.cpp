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
vector<TokenID> tokenset;
int tokenpoint=0;


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
//void output(TokenID tokencode){
//    if(tokencode == TK_UNDEF){
//        cout<<'A token was not defined in'<<row<<'row'<<endl;           //若没定义则报错
//    }
//   else{
//    cout << '(' << tokencode << ',' << token <<")" << endl;
//   }
//}
void WA(FILE *fp){
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

 //       output(tokencode);
        tokenset.push_back(tokencode);
    }
    tokencode = TK_EOF;
    token = "EOF";
 //   output(tokencode);
    tokenset.push_back(tokencode);
}

void ReportError(){
    printf("there is an error\n");
}


bool SymbolListnewtest(int tokenpointtest){
    if(tokenset[tokenpointtest] == TK_SYMBOL){
        tokenpointtest++;
//        printf("SymbolListnew   --->  SYMBOL SymbolListnew\n");
        return SymbolListnewtest(tokenpointtest);
    }  else{
//        printf("SymbolListnew   --->  NULL STRING\n");
        return true;
    }

}
bool SymbolListtest(int tokenpointtest){
//    printf("RightHandSide   --->   SymbolList\n");
    if(tokenset[tokenpointtest] == TK_SYMBOL){
        tokenpointtest++;
//        printf("SymbolList   --->  SYMBOL SymbolListnew\n");
        return SymbolListnewtest(tokenpointtest);
    }  else
    return false;
}

bool RightHandSidetest(int tokenpointtest){
    if(SymbolListtest(tokenpointtest)){
        return true;
    } else if(tokenset[tokenpointtest] == KW_EPSILON){
        tokenpointtest++;
//        printf("RightHandSide   --->   EPSILON\n");
        return true;
    }
    else
    return false;
}

bool ProductionSetnewtest(int tokenpointtest){
    if (tokenset[tokenpointtest] == TK_ALSODERIVES){
//        printf("ProductionSetnew   --->   ALSODERIVES  RightHandSide  ProductionSetnew\n");
        tokenpointtest++;
        if(!RightHandSidetest(tokenpointtest)){
            return false;
        }
        else
            return ProductionSetnewtest(tokenpointtest);
    }
    else{
//        printf("ProductionSetnew   --->   NULL STRING\n");
        return true;
    }
}
bool ProductionSettest(int tokenpointtest){
//    printf("ProductionSet   --->    SYMBOL  DERIVES  RightHandSide  ProductionSetnew\n");
    if (tokenset[tokenpointtest] == TK_SYMBOL){
        tokenpointtest++;
        if(tokenset[tokenpointtest] == TK_DERIVES){
            tokenpointtest++;
            if(!RightHandSidetest(tokenpointtest)){
                return false;
            }
            else return ProductionSetnewtest(tokenpointtest);
        }
        else
//            ReportError();
        return false;
    }
    else
//        printf("Debug\n");
//    cout<<tokenpoint<<endl;ReportError();
    return false;
}

bool SymbolListnew(){
    if(tokenset[tokenpoint] == TK_SYMBOL){
        tokenpoint++;
        printf("SymbolListnew   --->  SYMBOL SymbolListnew\n");
        return SymbolListnew();
    }  else{
        printf("SymbolListnew   --->  NULL STRING\n");
        return true;
    }

}
bool SymbolList(){
    printf("RightHandSide   --->   SymbolList\n");
    if(tokenset[tokenpoint] == TK_SYMBOL){
        tokenpoint++;
        printf("SymbolList   --->  SYMBOL SymbolListnew\n");
        return SymbolListnew();
    }  else ReportError();
    return false;
}

bool RightHandSide(){
    if(SymbolList()){
//        printf("Debug\n");
//        cout<<tokenpoint<<endl;
        return true;
    } else if(tokenset[tokenpoint] == KW_EPSILON){
        tokenpoint++;
        printf("RightHandSide   --->   EPSILON\n");
        return true;
    } else ReportError();
    return false;
}
bool ProductionSetnew(){
    if (tokenset[tokenpoint] == TK_ALSODERIVES){
        printf("ProductionSetnew   --->   ALSODERIVES  RightHandSide  ProductionSetnew\n");
        tokenpoint++;
        if(!RightHandSide()){
            return false;
        }
        else
        return ProductionSetnew();
    }
    else{
//        printf("Debug\n");
//        cout<<tokenpoint<<endl;
        printf("ProductionSetnew   --->   NULL STRING\n");
        return true;
    }
}
bool ProductionSet(){
    printf("ProductionSet   --->    SYMBOL  DERIVES  RightHandSide  ProductionSetnew\n");
   if (tokenset[tokenpoint] == TK_SYMBOL){
       tokenpoint++;
//       printf("Debug\n");
//       cout<<tokenpoint<<endl;
       if(tokenset[tokenpoint] == TK_DERIVES){
           tokenpoint++;
//           printf("Debug\n");
//           cout<<tokenpoint<<endl;
           if(!RightHandSide()){

              return false;
           }

           else
//               printf("Debug\n");
//           cout<<tokenpoint<<endl;
               return ProductionSetnew();
       }

       else ReportError();
       return false;
   }
   else
       ReportError();
   return false;
}


bool ProductionListnew(){
    printf("ProductionListnew  --->  ProductionSet   SEMICOLON   ProductionListnew\n");
    int testpoint = tokenpoint;
    if(ProductionSettest(testpoint)){
        ProductionSet();
        if(tokenset[tokenpoint] == TK_SEMICOLON){
            tokenpoint++;
            return ProductionListnew();} else       ReportError();
        return false;
//        printf("Debug\n");
//           cout<<tokenpoint<<endl;

    }
    else{
        printf("ProductionListnew  --->    NULL STRING\n");
    return true;
    }

}
bool ProductionList(){
    printf("ProductionList  --->    ProductionSet   SEMICOLON   ProductionListnew\n");
        int testpoint = tokenpoint;
    if(ProductionSettest(testpoint)){
           ProductionSet();
        if(tokenset[tokenpoint] == TK_SEMICOLON)
            tokenpoint++;
            return ProductionListnew();
        }
        else

            ReportError();
            return false;
    }
bool Grammar(){
    printf("Grammar     --->    ProductionList\n");
    return  ProductionList();
}
bool GA() {
    printf("start analyze\n");
    if(Grammar()){
        printf("success\n")     ;

    } else{
        printf("fail\n")     ;
    }
}


int main(int argc, char *argv[]) {
    const char *filename;
    filename = argv[1];
    FILE *fp = fopen(filename, "r");
    WA(fp);
    fclose(fp);
    GA();

    return 0;
}