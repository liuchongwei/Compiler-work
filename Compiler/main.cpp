#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

#define FMAX 100//源文件最大行数
#define LMAX 80 //行缓冲区最大容量
#define NMAX 30//支持的最大整数长度
#define TMAX 200//符号表最大长度
#define FN "source_code.txt" //文件名
#define OUTFILE "target_code.asm"
#define AL 30//标识符最大长度
#define NORW 13//保留字个数
#define PMAX 100//最大分程序数
#define CMAX 450//四元式个数
#define PARAMAX 10//函数参数个数

#define INITSTACK 0x7fffeffc
#define INITDATA 0x10010000

char symbol[][AL] = {
	"BECOMES","PLUS","MINU","MULT","DIV","NUM","ZERO",//NUM无符号整数
	"EQL","NEQ","GTR","GEQ","LSS","LEQ",
	"CONSTSY","INTSY","CHARSY","VOIDSY","MAINSY",
	"IFSY","ELSESY","DOSY","WHILESY","FORSY","SCANFSY","PRINTFSY","RETURNSY",
	"LPARENT","RPARENT","LBRACK","RBRACK","LBRACE","RBRACE","COMMA","SEMICOLON","STRCON","CHARCON",
	"IDENT","NUL"
};

FILE *FP;            //文件指针
ofstream rsT , miD;
char line[LMAX];//行缓冲区
//char SFile[FMAX][LMAX];//源文件
char token[AL];//标识符
char word[NORW][AL]; //记录所有保留字
char ch;

int cc = 0;			//character count
int ll = 0;			//line length
int ln = 0;			//line num
int num = 0;

char wsym[NORW][AL];//记录保留字对应的类型
char sym[AL];//记录token中的标识符类型
//char id[AL];

struct value {
	char c[2];
	int n;
};

struct funcelm{
    int returnnum;
    int paratyp[6];//0为int,1为char
};

struct tabitem {
	char name[AL];
	char obj[AL];
	char typ[AL];
	value v;
	int addr;
	int para;
	funcelm parastyp;
	int size;
};

struct maintable {
    tabitem table_elm[TMAX];//符号表
    int tx;         //符号标指针
    int proc_num;   //分程序数目
    int proc_index[PMAX];//分程序索引
};

maintable table;//符号表
int paranum = 0;//实参形参个数

struct mcode{
    char op[AL] , v1[50] , v2[AL] , v3[AL];
};

mcode midcode[CMAX];
int mpc = 0;//中间代码指针
int tmpnum = 0;//临时寄存器数目
int labnum = 0;//标签数目
vector<char*> tmpreg;//临时寄存器
char curobj[20];
char curtyp[20];
char op[AL] , v1[50] , v2[AL] , v3[AL];


/////////////////////////////////////////////////
int tpc = 0;
struct addrtab{
    char name[AL];
    int addr;
    int glob;
    int paranum;
    char typ[10];
};//变量常量地址表

addrtab addrtable[TMAX];
int ax = 0;
int fx = 0;//函数数目

int sp = 0;//相对地址
int hp = 4;//第一个data为"\n"
int fp = 0;//帧指针
int ismain = 0;
int isglob = 0;//是不是全局变量、常量
int tmplab = 0;
int vtmpreg = 2;//t2~t7
int ptmpreg = 2;
/////////////////////////////////////////////////////////
/*
char strs[30][50];
int strnum = 0;*/

int Expression();
int rFuncCall(int adr);
int Sentence();
int Sentences();

int pushdata(int length , char* value);
int pushstack(int length);
int insertadrtab(char* name , int adr , char* typ , int glob);
int FUNCOBJasm();
int varaddr(char *name);

char* num2str(int i){
    char* numstr = new char[11];
    sprintf(numstr , "%d" , i);
    return numstr;
}

int isLetter(){
	if((ch>='a' && ch <='z') || (ch>='A' && ch<='Z') || ch == '_'){
		return 1;
	}else{
		return 0;
	}
}

int isDigit(){
	if(ch>='0' && ch<='9')
		return 1;
	else
		return 0;
}

int isNum(char* str){
    int i = 0;
    while(str[i] != '\0'){
        if(str[i]<'0' || str[i]>'9'){
            if(i == 0 && (str[i] == '+' || str[i] == '-'))
                ;
            else
                return 0;
        }
        i++;
    }
    return 1;
}

int isChar(char* str){
    if(strcmp(str , "+") == 0)
		return 1;
    if(strcmp(str , "-") == 0)
        return 1;
    if(strcmp(str , "*") == 0)
        return 1;
    if(strcmp(str , "/") == 0)
        return 1;
/*    if((str[0]>='a' && str[0] <='z') || (str[0]>='A' && str[0]<='Z') || str[0] == '_')
		return 1;*/
    return 0;
}

void error(int el , int en){
	printf("error : in line %d col %d\t%d\n" , ln , el , en);
	exit(0);
}

void getch(){
	if(cc == ll){
		ll = 0;
		cc = 0;
		if(feof(FP)){
			ch = '\0';
			return ;
		}
		if(fgets(line,LMAX,FP) != NULL)
		{
//			strcpy(SFile[ln],line);
			ll = strlen(line);
		 	line[ll] = '\0';  /*去掉换行符*/
		 	ln++;
//		 	printf("%s %d \n",line,ll);
			printf("%s\n",line);
		}
	}
	ch = line[cc++];
}

int getsym(){
	int k = 0;//用于将ch存入token ; 用于记录整数位数
	int i;//用于查找保留字

	while(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'){
		getch();
	}

	if(isLetter()){
		while(isLetter() || isDigit()){
			//TODO 如果标识符长度大于最大长度怎么办
			if(k<AL){
				token[k++] = ch;
			} else {
				error(cc-AL , 1);
				while(isLetter() || isDigit()){
					getch();
				}
				return 1;
			}
			getch();
		}
		token[k] = '\0';
		strcpy(sym,"IDENT");
		for(i = 0 ; i < NORW ; i++) {
			if(strcmp(token , word[i]) == 0){
				strcpy(sym,wsym[i]);
				break;
			}
		}
	} else if(isDigit()){
		if(ch == '0'){
			getch();
			if(isDigit()){
				error(cc,2);
				while(isDigit()){
					getch();
				} //TODO 跳读
				return 1;
			} else {
				strcpy(sym,"ZERO");
				token[k++] = '0';
				num = 0;
			}
		} else {
			strcpy(sym,"NUM");//无符号整数
			num = 0;
			while(isDigit()){
				if(k<NMAX){
					token[k++] = ch;
					num = num*10 + ch-'0';
				} else {
					error(cc-NMAX , 3);
					while(isDigit()){
						getch();
					}//TODO 跳读
					return 1;
				}
				getch();
			}
		}
	} else if(ch == '=') {
		token[k++] = '=';
		getch();
		if(ch == '='){
			strcpy(sym,"EQL");
			token[k++] = '=';
			getch();
		}else{
			strcpy(sym,"BECOMES");
		}
	} else if(ch == '>') {
		token[k++] = '>';
		getch();
		if(ch == '='){
			strcpy(sym,"GEQ");
			token[k++] = '=';
			getch();
		}else{
			strcpy(sym,"GTR");
		}
	} else if(ch == '<') {
		token[k++] = '<';
		getch();
		if(ch == '='){
			strcpy(sym,"LEQ");
			token[k++] = '=';
			getch();
		}else{
			strcpy(sym,"LSS");
		}
	} else if(ch == '!') {
		token[k++] = '!';
		getch();
		if(ch == '='){
			strcpy(sym,"NEQ");
			token[k++] = '=';
			getch();
		}else{
			error(cc-strlen(token)-1 , 20);
		}
	} else if(ch == '\"'){
		k = 0;
		token[k] = '\0';
		getch();
		while(ch == ' ' || ch == 32 || ch == 33 || (ch <= 126 && ch >= 35)){
            if(k >= 50)
                error(cc-strlen(token)-1 , 20);
			token[k++] = ch;
			getch();
		}
		if(ch == '\"'){
			strcpy(sym,"STRCON");
			getch();
		}
		else{
			error(cc-strlen(token)-1 , 20);
		}
	}else if(ch == '\''){
		getch();
		if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || isDigit() || isLetter()){
			token[k++] = ch;
			getch();
		}else{
			error(cc-strlen(token)-1 , 20);
		}
		if(ch == '\''){
			strcpy(sym,"CHARCON");
			getch();
		} else {
			error(cc-strlen(token)-1 , 20);
		}
	} else {
		if ( ch == '+' ){
			strcpy(sym,"PLUS");
		}
		else if ( ch == '-' ){
			strcpy(sym,"MINU");
		}
		else if ( ch == '*' ){
			strcpy(sym,"MULT");
		}
		 else if ( ch == '/' ){
			strcpy(sym,"DIV");
		}
		else if ( ch == ',' ){
			strcpy(sym,"COMMA");
		}
		else if ( ch == ';' ){
			strcpy(sym,"SEMICOLON");
		}
		else if ( ch == '{' ){
			strcpy(sym,"LBRACE");
		}
		else if ( ch == '[' ){
			strcpy(sym,"LBRACK");
		}
		else if ( ch == '(' ){
			strcpy(sym,"LPARENT");
		}
		else if ( ch == '}' ){
			strcpy(sym,"RBRACE");
		}
		else if ( ch == ']' ){
			strcpy(sym,"RBRACK");
		}
		else if ( ch == ')' ){
			strcpy(sym,"RPARENT");
		} else {
			strcpy(sym,"NUL");
		}
		token[k++] = ch;
		getch();
	}

	token[k] = '\0';
//	printf("%s\t%s\n" , token , sym);

	return 0;
}

int skipback(int tmpcc , int tmpln){
	if(ln == tmpln){
		cc = tmpcc + 1;
		ch = line[tmpcc];
	} else if (ln > tmpln){
		printf("what fuck\n");
	} else {
		return -1;
	}
	return 0;
}

int enter(char* name, char* obj, char* typ, value v , int addr , int para , int size){
    if(table.tx > TMAX){
        error(cc - strlen(token) , 20);//符号表溢出
    }

    if(strcmp(obj,"FUNCOBJ") == 0){
        int j;
        for(j = 0 ; j < table.proc_num ; j++){
            if(strcmp(table.table_elm[j].name , name) == 0){
                error(cc - strlen(token) , 20);
                return -1;
            }
        }
        table.proc_index[table.proc_num++] = table.tx;
    }


	tabitem i;

	strcpy(i.name , name);
	strcpy(i.obj , obj);
	strcpy(i.typ , typ);
	i.v = v;
	i.addr = table.tx;
	i.para = para;
	i.size = size;

	table.table_elm[table.tx++] = i;
	return table.tx-1;
}
/*
char* enterstrs(char* s){
    char strlab[10];
    strcpy(strs[strnum++] , s);
    strcpy(strlab , "STR");
    strcat(strlab , num2str(strnum-1));
    return strlab;
}*/

int Position(char* name , int tag){
	int i;

    if(tag == 0){//如果是函数调用
        for(i = 0 ; i < table.proc_num ; i++ ){
            if(strcmp(table.table_elm[table.proc_index[i]].name , name) == 0){
                return table.table_elm[table.proc_index[i]].addr;
            }
        }
    } else if(tag == 1){//如果是常量或变量
        i = table.proc_index[table.proc_num-1]+1;
        while(i < table.tx){
            if(strcmp(table.table_elm[i].name , name) == 0){
                return table.table_elm[i].addr;
            }
            i++;
        }
        for(i = 0 ; i < table.proc_index[0] ; i++){
            if(strcmp(table.table_elm[i].name , name) == 0){
                return table.table_elm[i].addr;
            }
        }
    }
    error(cc - strlen(token) , 20);
    return -1;
}

int Flush(){
    while(strcmp(table.table_elm[table.tx-1].obj , "FUNCOBJ") != 0){
        table.tx--;
        value v;
        table.table_elm[table.tx].addr = 0;
        table.table_elm[table.tx].para = 0;
        table.table_elm[table.tx].size = 0;
        table.table_elm[table.tx].v = v;
        memset(table.table_elm[table.tx].name , '\0' , sizeof(table.table_elm[table.tx].name));
        memset(table.table_elm[table.tx].obj , '\0' , sizeof(table.table_elm[table.tx].obj));
        memset(table.table_elm[table.tx].typ , '\0' , sizeof(table.table_elm[table.tx].typ));
    }
    return 0;
}

int fillmidcode(){
    strcpy(midcode[mpc].op , op);
    strcpy(midcode[mpc].v1 , v1);
    strcpy(midcode[mpc].v2 , v2);
    strcpy(midcode[mpc].v3 , v3);
    mpc++;
    return 0;
}

char* nexttmp(){
    char* tmp = (char*)malloc(sizeof(char) * 20);
    sprintf(tmp, "$t%d", tmpnum++);
    tmpreg.push_back(tmp);
    return tmp;
}

char* nextlab(){
    char* lab = new char[11];
    strcpy(lab , "lable_");
    strcat(lab , num2str(labnum++));
    return lab;
}

int free_tmpreg(){
    int i;
    for(i = 0;i < tmpnum;i++){
        free(tmpreg[i]);
    }
    return 0;
}

int ConstDef(int tp){
	printf("This is 常量定义\n");

	char tmptoken[AL];
	char tmpop[AL] = "PLUS";
	char obj[] = "CONSTOBJ";
	char typ[AL];
	value v;

	if(tp == 1){
		strcpy(typ , "INTSY");
	} else {
		strcpy(typ , "CHARSY");
	}

	if(strcmp(sym , "IDENT") == 0){
		strcpy(tmptoken , token);
		getsym();
	} else {
		error(cc - strlen(token) , 20);
	}

	if(strcmp(sym , "BECOMES") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 21);
	}

	if(strcmp(typ , "INTSY") == 0){
		if(strcmp(sym , "PLUS") == 0){
			strcpy(tmpop , "PLUS");
			getsym();
		} else if(strcmp(sym , "MINU") == 0){
			strcpy(tmpop , "MINU");
			getsym();
		}

		if(strcmp(sym , "NUM") == 0 || strcmp(sym , "ZERO") == 0){
			if(strcmp(tmpop , "PLUS") == 0){
				v.n = num;
			} else {
				v.n = 0 - num;
			}
			enter(tmptoken, obj, typ, v, 0, 0, 0);

//			fillmidcode(obj , typ , tmptoken , num2str(v.n));
            strcpy(op , obj);
            strcpy(v1 , typ);
            strcpy(v2 , tmptoken);
            strcpy(v3 , num2str(v.n));
            fillmidcode();

			getsym();
			return 0;
		} else {
			error(cc - strlen(token) , 22);
		}
	} else {
		if(strcmp(sym , "CHARCON") == 0){
			strcpy(v.c , token);
			enter(tmptoken, obj, typ, v, 0, 0, 0);

//			fillmidcode(obj , typ , tmptoken , v.c);
            strcpy(op , obj);
            strcpy(v1 , typ);
            strcpy(v2 , tmptoken);
            strcpy(v3 , v.c);
            fillmidcode();

			getsym();
			return 0;
		} else {
			error(cc - strlen(token) , 22);
		}
	}

	return -1;
}

int ConstSta(){
	printf("This is 常量说明\n");

	if(strcmp(sym , "CONSTSY") != 0){
		error(cc - strlen(token) , 17);
	}

	while(strcmp(sym , "CONSTSY") == 0){

		getsym();

		if(strcmp(sym , "INTSY") == 0){
			getsym();
			ConstDef(1);
			while(strcmp(sym , "COMMA") == 0){
				getsym();
				ConstDef(1);
			}
		} else if (strcmp(sym , "CHARSY") == 0) {
			getsym();
			ConstDef(2);
			while(strcmp(sym , "COMMA") == 0){
				getsym();
				ConstDef(2);
			}
		} else {
			error(cc - strlen(token) , 19);
		}

		if(strcmp(sym , "SEMICOLON") == 0){
			getsym();
		} else {
			error(cc - strlen(token) , 18);
		}
	}

	return 0;
}

int VarDef(int tp){
	printf("This is 变量定义\n");

	char tmptoken[AL];
	char obj[] = "VAROBJ";
	char typ[AL];
	value v;

	if(tp == 1){
		strcpy(typ , "INTSY");
	} else {
		strcpy(typ , "CHARSY");
	}

	if(strcmp(sym , "IDENT") == 0){
		strcpy(tmptoken , token);
		getsym();
	} else {
		error(cc - strlen(token) , 20);
	}

	if(strcmp(sym , "COMMA") == 0 || strcmp(sym , "SEMICOLON") == 0){
		enter(tmptoken, obj, typ, v, 0, 0, 0);

		strcpy(op , obj);
		strcpy(v1 , typ);
		strcpy(v2 , tmptoken);
        strcpy(v3 , "\0");
		fillmidcode();

		return 0;
	} else if (strcmp(sym , "LBRACK") == 0) {
		getsym();

		if(strcmp(sym , "NUM") == 0){
			enter(tmptoken, obj, typ, v, 0, 0, num);

//			fillmidcode('A'+obj , typ , tmptoken , num2str(num));
            strcpy(op , "AVAROBJ");
            strcpy(v1 , typ);
            strcpy(v2 , tmptoken);
            strcpy(v3 ,  num2str(num));
            fillmidcode();

			getsym();
		} else {
			error(cc - strlen(token) , 21);
		}

		if(strcmp(sym , "RBRACK") == 0){
			getsym();
		} else {
			error(cc - strlen(token) , 22);
		}

	} else {
		error(cc - strlen(token) , 23);
	}

	return -1;
}

int VarSta(){
	printf("This is 变量说明\n");

	int tmpcc = 0;
	int tmpln = 0;

	while(strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0){
		tmpcc = cc - strlen(token) - 1;
		tmpln = ln;

		getsym();

		if(strcmp(sym , "IDENT") != 0){
			error(cc-strlen(token) , 10);//int和char后不是标识符
		}

		getsym();

		if(strcmp(sym , "COMMA") == 0 || strcmp(sym , "SEMICOLON") == 0 || strcmp(sym , "LBRACK") == 0){
			skipback(tmpcc , tmpln);
		} else if (strcmp(sym , "LPARENT") == 0){
			skipback(tmpcc , tmpln);
			getsym();
			return 0;
		} else {
			error(cc-strlen(token) , 20);
		}

		getsym();

		if(strcmp(sym , "INTSY") == 0){
			getsym();
			VarDef(1);
			while(strcmp(sym , "COMMA") == 0){
				getsym();
				VarDef(1);
			}
		} else if (strcmp(sym , "CHARSY") == 0) {
			getsym();
			VarDef(2);
			while(strcmp(sym , "COMMA") == 0){
				getsym();
				VarDef(2);
			}
		} else {
			error(cc - strlen(token) , 19);
		}

		if(strcmp(sym , "SEMICOLON") == 0){
			getsym();
		} else {
			error(cc - strlen(token) , 18);
		}
	}
	return -1;
}

int Parameters(int tabadr){
	char typ[AL];
	char obj[] = "VAROBJ";
	value v;
	int i = 0;

	if(strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0){
		strcpy(typ , sym);
		getsym();
	} else {
//		printf("没有参数\n");
		return 0;
	}

	printf("This is 参数\n");

	if(strcmp(sym , "IDENT") == 0){
		enter(token, obj, typ, v, 0, 0, 0);//TODO 插入失败输出错误程序结束

		strcpy(op , "PARA");
		strcpy(v1 , typ);
		strcpy(v2 , token);
        strcpy(v3 , "\0");
		fillmidcode();

		table.table_elm[tabadr].para++;

		if(strcmp(typ , "INTSY") == 0)
            table.table_elm[tabadr].parastyp.paratyp[i] = 0;
        else
            table.table_elm[tabadr].parastyp.paratyp[i] = 1;
        i++;

		getsym();
	} else {
		error(cc - strlen(token) , 20);
	}

	while(strcmp(sym , "COMMA") == 0){
        if(i >= 5){
            error(cc - strlen(token) , 20);
        }
		getsym();
		if(strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0){
			strcpy(typ , sym);
			getsym();
		} else {
			error(cc - strlen(token) , 20);
		}

		if(strcmp(sym , "IDENT") == 0){
			enter(token, obj, typ, v, 0, 0, 0);//TODO 插入失败输出错误程序结束

            strcpy(op , "PARA");
            strcpy(v1 , typ);
            strcpy(v2 , token);
            strcpy(v3 , "\0");
            fillmidcode();

			table.table_elm[tabadr].para++;

			if(strcmp(typ , "INTSY") == 0)
                table.table_elm[tabadr].parastyp.paratyp[i] = 0;
            else
                table.table_elm[tabadr].parastyp.paratyp[i] = 1;
            i++;

			getsym();
		} else {
			error(cc - strlen(token) , 20);
		}
	}

	return 0;
}

int Factor(){
	printf("This is 因子\n");

	char tmptoken[AL];
	int adr;

	if(strcmp(sym , "IDENT") == 0){
		strcpy(tmptoken , token);
		getsym();
	} else if(strcmp(sym , "PLUS") == 0 || strcmp(sym , "MINU") == 0 || strcmp(sym , "NUM") == 0 || strcmp(sym , "ZERO") == 0){
	    if(strcmp(sym , "NUM") == 0){
            strcpy(curobj , token);
            strcpy(curtyp , "INTCON");
            getsym();
        } else if(strcmp(sym , "ZERO") == 0){
            strcpy(curobj , token);
            strcpy(curtyp , "INTCON");
            getsym();
        }else if(strcmp(sym , "PLUS") == 0){
            getsym();
            if(strcmp(sym , "NUM") == 0){
                strcpy(curobj , token);
                strcpy(curtyp , "INTCON");
                getsym();
            } else {
                error(cc-strlen(token)-1 , 20);
            }
        }else if(strcmp(sym , "MINU") == 0){
            getsym();
            if(strcmp(sym , "NUM") == 0){
                strcpy(curobj , num2str(-1*num));
                strcpy(curtyp , "INTCON");
                getsym();
            } else {
                error(cc-strlen(token)-1 , 20);
            }
        }
		return 0;
	} else if(strcmp(sym , "CHARCON") == 0){
		strcpy(curobj , num2str((int)token[0]));
		strcpy(curtyp , "CHARCON");
		getsym();
		return 0;
	} else if(strcmp(sym , "LPARENT") == 0){
		getsym();
		Expression();

		if(strcmp(sym , "RPARENT") == 0){
			getsym();
			return 0;
		} else {
			error(cc - strlen(token) , 20);
			return -1;
		}
	} else {
		error(cc - strlen(token) , 20);
		return -1;
	}


	if(strcmp(sym , "LBRACK") == 0){
        adr = Position(tmptoken , 1);

        if(adr == -1){
            error(cc-strlen(token)+1 , 20);
            return -1;
        }

		getsym();
		Expression();

		if(!(strcmp(curtyp , "INTSY") == 0 || strcmp(curtyp , "INTCON") == 0)){
            error(cc-strlen(token)+1 , 20);
		}

		if(strcmp(sym , "RBRACK") == 0){

            strcpy(op , "AVALUE");
            strcpy(v1 , tmptoken);
            strcpy(v2 , curobj);
            strcpy(v3 , nexttmp());
            fillmidcode();

            strcpy(curobj , v3);
            strcpy(curtyp , table.table_elm[adr].typ);

			getsym();
		} else {
			error(cc - strlen(token) , 20);
			return -1;
		}
	} else if(strcmp(sym , "LPARENT") == 0){
	    adr = Position(tmptoken , 0);

	    if(adr == -1){
            error(cc-strlen(token)+1 , 20);
            return -1;
        }

		if(strcmp(table.table_elm[adr].typ , "VOIDSY") == 0 && strcmp(table.table_elm[adr].obj , "FUNCOBJ") == 0){
			error(cc-strlen(token)-1 , 20);
			return -1;
		} else if ((strcmp(table.table_elm[adr].typ , "INTSY") == 0 || strcmp(table.table_elm[adr].typ , "CHARSY") == 0) && strcmp(table.table_elm[adr].obj , "FUNCOBJ") == 0){
			rFuncCall(adr);
		}
	} else {
        //标识符
        adr = Position(tmptoken , 1);
        if(adr != -1){
            strcpy(curobj , tmptoken);
            strcpy(curtyp , table.table_elm[adr].typ);
        } else {
            error(cc-strlen(token)-1 , 10);//未找到标识符
            return -1;
        }
	}

	return 0;
}

int Item(){
    char tmpop[10] , tmpv1[10];
	printf("This is 项\n");
	//test
	Factor();

	while(strcmp(sym , "MULT") == 0 || strcmp(sym , "DIV") == 0){
        strcpy(tmpop ,  sym);
        strcpy(tmpv1 , curobj);

		getsym();
		Factor();

        strcpy(op , tmpop);
        strcpy(v1 , tmpv1);
        strcpy(v2 , curobj);
		strcpy(v3 , nexttmp());
		fillmidcode();

		strcpy(curobj , v3);
		strcpy(curtyp , "INTSY");
	}
    strcpy(tmpop,"");
	return 0;
}

int Expression(){
	printf("This is 表达式\n");
	//test
	char tmpop[10] , tmpv1[10];

	if(strcmp(sym , "PLUS") == 0 || strcmp(sym , "MINU") == 0){
		strcpy(tmpop , sym);
		getsym();
	}

	Item();

    if(strcmp(tmpop , "PLUS") == 0 || strcmp(tmpop , "MINU") == 0){
        strcpy(op , tmpop);
        strcpy(v1 , "0");
        strcpy(v2 , curobj);
        strcpy(v3 , nexttmp());
        fillmidcode();

        strcpy(curobj , v3);
		strcpy(curtyp , "INTSY");
    }

	while(strcmp(sym , "PLUS") == 0 || strcmp(sym , "MINU") == 0){
        strcpy(tmpop , sym);
        strcpy(tmpv1 , curobj);

		getsym();
		Item();

        strcpy(op , tmpop);
        strcpy(v1 , tmpv1);
        strcpy(v2 , curobj);
		strcpy(v3 , nexttmp());
		fillmidcode();

		strcpy(curobj , v3);
		strcpy(curtyp , "INTSY");
	}

	//TODO 生成四元式，需要tmpop
    strcpy(tmpop,"");
	return 0;
}

int valuepara(int adr){
    int i = 0;//值参数个数
	printf("This is 值参数表\n");
	if(strcmp(sym , "RPARENT") == 0){
		return 0;
	}

	Expression();

    if(i < table.table_elm[adr].para)
        i++;
    else
        error(cc-strlen(token)+1 , 20);

    if(strcmp(curtyp , "INTSY") == 0 || strcmp(curtyp , "INTCON") == 0){
        if(table.table_elm[adr].parastyp.paratyp[i-1] != 0)
            error(cc-strlen(token)+1 , 20);
    } else if(strcmp(curtyp , "CHARSY") == 0 || strcmp(curtyp , "CHARCON") == 0){
        if(table.table_elm[adr].parastyp.paratyp[i-1] != 1)
            error(cc-strlen(token)+1 , 20);
    }

    strcpy(op , "VPARA");
    strcpy(v1 , curobj);
    strcpy(v2 , table.table_elm[adr].name);
    strcpy(v3 , "\0");
    fillmidcode();

	while(strcmp(sym , "COMMA") == 0){
		getsym();
		Expression();

		if(i < table.table_elm[adr].para)
            i++;
        else
            error(cc-strlen(token)+1 , 20);

        if(strcmp(curtyp , "INTSY") == 0 || strcmp(curtyp , "INTCON") == 0){
            if(table.table_elm[adr].parastyp.paratyp[i-1] != 0)
                error(cc-strlen(token)+1 , 20);
        } else if(strcmp(curtyp , "CHARSY") == 0 || strcmp(curtyp , "CHARCON") == 0){
            if(table.table_elm[adr].parastyp.paratyp[i-1] != 1)
                error(cc-strlen(token)+1 , 20);
        }

        strcpy(op , "VPARA");
        strcpy(v1 , curobj);
        strcpy(v2 , table.table_elm[adr].name);
        strcpy(v3 , "\0");
        fillmidcode();
	}
	return i;
}

int vFuncCall(int adr){
	printf("This is 无返回值函数调用\n");
	int paranum = 0;
	if(strcmp(sym , "LPARENT") == 0){
		getsym();
		paranum = valuepara(adr);
	} else {
		error(cc - strlen(token) , 20);
	}

    if(paranum != table.table_elm[adr].para){
        error(cc-strlen(token)+1 , 20);
    }

	if(strcmp(sym , "RPARENT") == 0){

        strcpy(op , "CALL");
        strcpy(v1 , table.table_elm[adr].name);
        strcpy(v2 , "\0");
        strcpy(v3 , "\0");
        fillmidcode();

		getsym();
		return 0;
	} else {
		error(cc - strlen(token) , 20);
	}

	return -1;
}

int rFuncCall(int adr){
	printf("This is 有返回值函数调用\n");
	int paranum = 0;

	if(strcmp(sym , "LPARENT") == 0){
		getsym();
		paranum = valuepara(adr);
	} else {
		error(cc - strlen(token) , 20);
	}

    if(paranum != table.table_elm[adr].para){
        error(cc-strlen(token)+1 , 20);
    }

	if(strcmp(sym , "RPARENT") == 0){

        strcpy(op , "CALL");
        strcpy(v1 , table.table_elm[adr].name);
        strcpy(v2 , "\0");
        strcpy(v3 , nexttmp());
        fillmidcode();

        strcpy(curobj , v3);
        strcpy(curtyp , table.table_elm[adr].typ);

		getsym();
		return 0;
	} else {
		error(cc - strlen(token) , 20);
	}

	return -1;
}

int Condition(){
    char tmpop[10] , tmpv1[10];
	printf("This is 条件\n");
	Expression();

	if(strcmp(sym , "EQL") == 0){
		strcpy(tmpop , sym);
		strcpy(tmpv1 , curobj);

		getsym();
		Expression();

        strcpy(op , tmpop);
        strcpy(v1 , tmpv1);
        strcpy(v2 , curobj);
		strcpy(v3 , nexttmp());
		fillmidcode();
		strcpy(curobj , v3);
	} else if(strcmp(sym , "NEQ") == 0) {
		strcpy(tmpop , sym);
		strcpy(tmpv1 , curobj);

		getsym();
		Expression();

        strcpy(op , tmpop);
        strcpy(v1 , tmpv1);
        strcpy(v2 , curobj);
		strcpy(v3 , nexttmp());
		fillmidcode();
		strcpy(curobj , v3);
	} else if(strcmp(sym , "GTR") == 0) {
		strcpy(tmpop , sym);
		strcpy(tmpv1 , curobj);

		getsym();
		Expression();

        strcpy(op , tmpop);
        strcpy(v1 , tmpv1);
        strcpy(v2 , curobj);
		strcpy(v3 , nexttmp());
		fillmidcode();
		strcpy(curobj , v3);
	} else if(strcmp(sym , "GEQ") == 0) {
		strcpy(tmpop , sym);
		strcpy(tmpv1 , curobj);

		getsym();
		Expression();

        strcpy(op , tmpop);
        strcpy(v1 , tmpv1);
        strcpy(v2 , curobj);
		strcpy(v3 , nexttmp());
		fillmidcode();
		strcpy(curobj , v3);
	} else if(strcmp(sym , "LSS") == 0) {
		strcpy(tmpop , sym);
		strcpy(tmpv1 , curobj);

		getsym();
		Expression();

        strcpy(op , tmpop);
        strcpy(v1 , tmpv1);
        strcpy(v2 , curobj);
		strcpy(v3 , nexttmp());
		fillmidcode();
		strcpy(curobj , v3);
	} else if(strcmp(sym , "LEQ") == 0) {
		strcpy(tmpop , sym);
		strcpy(tmpv1 , curobj);

		getsym();
		Expression();

        strcpy(op , tmpop);
        strcpy(v1 , tmpv1);
        strcpy(v2 , curobj);
		strcpy(v3 , nexttmp());
		fillmidcode();
		strcpy(curobj , v3);
	} else if (strcmp(sym , "RPARENT") == 0 || strcmp(sym , "SEMICOLON") == 0){
        strcpy(op , "NEQ");
        strcpy(v1 , curobj);
		strcpy(v2 , "0");
		strcpy(v3 , nexttmp());
		fillmidcode();
		strcpy(curobj , v3);
	}

	return 0;
}

int IfSentence(){
	printf("This is 条件语句\n");
	char lab1[10] , lab2[10];
	strcpy(lab1 , nextlab());
	strcpy(lab2 , nextlab());

	if(strcmp(sym , "IFSY") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "LPARENT") == 0){
		getsym();
		Condition();

		strcpy(op , "JNE");
        strcpy(v1 , curobj);
        strcpy(v2 , lab2);
        strcpy(v3 , "\0");
        fillmidcode();

	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "RPARENT") == 0){
		getsym();
		Sentence();
		strcpy(op , "JMP");
        strcpy(v1 , "\0");
        strcpy(v2 , lab1);
        strcpy(v3 , "\0");
        fillmidcode();
	} else {
		error(cc - strlen(token) , 24);
	}

    strcpy(op , "LAB");
    strcpy(v1 , "\0");
    strcpy(v2 , lab2);
    strcpy(v3 , "\0");
    fillmidcode();

	if(strcmp(sym , "ELSESY") == 0){
		getsym();
		Sentence();
	}

    strcpy(op , "LAB");
    strcpy(v1 , "\0");
    strcpy(v2 , lab1);
    strcpy(v3 , "\0");
    fillmidcode();

	return 0;
}

int WhileSentence(){
	printf("This is 循环语句\n");
	char lab[10];
	strcpy(lab , nextlab());

	if(strcmp(sym , "DOSY") == 0){
        strcpy(op , "LAB");
        strcpy(v1 , "\0");
        strcpy(v2 , lab);
        strcpy(v3 , "\0");
        fillmidcode();

		getsym();
		Sentence();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "WHILESY") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "LPARENT") == 0){
		getsym();
		Condition();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "RPARENT") == 0){
        strcpy(op , "JEQ");
        strcpy(v1 , curobj);
        strcpy(v2 , lab);
        strcpy(v3 , "\0");
        fillmidcode();

		getsym();
		return 0;
	} else {
		error(cc - strlen(token) , 24);
	}
	return -1;
}

int ForSentence(){
	printf("This is 循环语句\n");
	char tmptoken1[AL];
	char tmptoken2[AL];
	char tmptoken3[AL];
	char tmpop[AL];

	char lab1[10] , lab2[10] , lab3[10] , lab4[10];
	strcpy(lab1 , nextlab());
	strcpy(lab2 , nextlab());
	strcpy(lab3 , nextlab());
	strcpy(lab4 , nextlab());

	if(strcmp(sym , "FORSY") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "LPARENT") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "IDENT") == 0){
		strcpy(tmptoken1 , token);

		if(Position(tmptoken1 , 1) >= 0){
           getsym();
		} else {
            error(cc-strlen(token)+1 , 20);
            return -1;
		}

	} else {
		error(cc - strlen(token)+1 , 24);
		return -1;
	}

	if(strcmp(sym , "BECOMES") == 0){
		getsym();
		Expression();

/*        if(!(strcmp(curtyp , "INTSY") == 0 || strcmp(curtyp , "INTCON") == 0)){
            error(cc - strlen(token) , 24);
        }*/

        strcpy(op , "BECOM");
        strcpy(v1 , curobj);
        strcpy(v2 , "\0");
        strcpy(v3 , tmptoken1);
        fillmidcode();

	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "SEMICOLON") == 0){
		getsym();
        strcpy(op , "LAB");
        strcpy(v1 , "\0");
        strcpy(v2 , lab1);
        strcpy(v3 , "\0");
        fillmidcode();


		Condition();

//		fillmidcode("JNE" , curobj , lab2 , "\0");
		strcpy(op , "JNE");
        strcpy(v1 , curobj);
        strcpy(v2 , lab2);
        strcpy(v3 , "\0");
        fillmidcode();
//		fillmidcode("JMP" , "\0" , lab4 , "\0");
        strcpy(op , "JMP");
        strcpy(v1 , "\0");
        strcpy(v2 , lab4);
        strcpy(v3 , "\0");
        fillmidcode();

	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "SEMICOLON") == 0){
		getsym();
//		fillmidcode("LAB" , "\0" , lab3 , "\0");
        strcpy(op , "LAB");
        strcpy(v1 , "\0");
        strcpy(v2 , lab3);
        strcpy(v3 , "\0");
        fillmidcode();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "IDENT") == 0){
		strcpy(tmptoken2 , token);
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "BECOMES") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "IDENT") == 0){
		strcpy(tmptoken3 , token);
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "PLUS") == 0 || strcmp(sym , "MINU") == 0){
		strcpy(tmpop , sym);
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "NUM") == 0){
	    strcpy(v3 , nexttmp());
        if(strcmp(tmpop , "PLUS") == 0){
//            fillmidcode("PLUS" , tmptoken3 , token , v3);
            strcpy(op , "PLUS");
            strcpy(v1 , tmptoken3);
            strcpy(v2 , token);
            fillmidcode();
            strcpy(curobj , v3);
        } else if (strcmp(tmpop , "MINU") == 0) {
//          fillmidcode("MINU" , tmptoken3 , token , v3);
            strcpy(op , "MINU");
            strcpy(v1 , tmptoken3);
            strcpy(v2 , token);
            fillmidcode();

            strcpy(curobj , v3);
        }

//      fillmidcode("BECOM" , curobj , "\0" , tmptoken2);
        strcpy(op , "BECOM");
        strcpy(v1 , curobj);
        strcpy(v2 , "\0");
        strcpy(v3 , tmptoken2);
        fillmidcode();
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "RPARENT") == 0){
//        fillmidcode("JMP" , "\0" , lab1 , "\0");
        strcpy(op , "JMP");
        strcpy(v1 , "\0");
        strcpy(v2 , lab1);
        strcpy(v3 , "\0");
        fillmidcode();
		getsym();

//		fillmidcode("LAB" , "\0" , lab4 , "\0");
        strcpy(op , "LAB");
        strcpy(v1 , "\0");
        strcpy(v2 , lab4);
        strcpy(v3 , "\0");
        fillmidcode();
		Sentence();

//        fillmidcode("JMP" , "\0" , lab3 , "\0");
        strcpy(op , "JMP");
        strcpy(v1 , "\0");
        strcpy(v2 , lab3);
        strcpy(v3 , "\0");
        fillmidcode();
//		fillmidcode("LAB" , "\0" , lab2 , "\0");
        strcpy(op , "LAB");
        strcpy(v1 , "\0");
        strcpy(v2 , lab2);
        strcpy(v3 , "\0");
        fillmidcode();
		return 0;
	} else {
		error(cc - strlen(token) , 24);
	}

	return -1;
}

int BecomeSentence(char *name){
	printf("This is 赋值语句\n");
	int adr = Position(name , 1);
	char tmpv2[10];

	if(strcmp(table.table_elm[adr].obj , "CONSTOBJ") == 0){
        error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "LBRACK") == 0){
		if(table.table_elm[adr].size == 0){
			error(cc - strlen(token) , 24);
			return -1;
		}

		getsym();
		Expression();//如果不是整数

        strcpy(tmpv2 , curobj);

		if(strcmp(sym , "RBRACK") == 0){
			getsym();
		} else {
			error(cc - strlen(token) , 24);
		}

		if(strcmp(sym , "BECOMES") == 0){
			getsym();
		} else {
			error(cc - strlen(token) , 24);
		}

		Expression();
/*		fillmidcode("VARRAY" , v1 , v2 , table.table_elm[adr].name);

        if(strcmp(table.table_elm[adr].typ , "INTSY") == 0){
            if(strcmp(curtyp , "INTSY") == 0 || strcmp(curtyp , "INTCON") == 0){
                ;
            } else
                error(cc - strlen(token) , 24);
        } else if(strcmp(table.table_elm[adr].typ , "CHARSY") == 0){
            if(strcmp(curtyp , "CHARSY") == 0 || strcmp(curtyp , "CHARCON") == 0){
                ;
            } else
                error(cc - strlen(token) , 24);
        }*/

        strcpy(op , "VARRAY");
        strcpy(v1 , curobj);
        strcpy(v2 , tmpv2);
        strcpy(v3 , table.table_elm[adr].name);
        fillmidcode();
		return 0;
	} else if(strcmp(sym , "BECOMES") == 0){
		if(table.table_elm[adr].size != 0){
			error(cc - strlen(token) , 24);
			return -1;
		}

		getsym();
		Expression();

/*		fillmidcode("BECOM" , curobj , "\0" , table.table_elm[adr].name);

        if(strcmp(table.table_elm[adr].typ , "INTSY") == 0){
            if(strcmp(curtyp , "INTSY") == 0 || strcmp(curtyp , "INTCON") == 0){
                ;
            } else
                error(cc - strlen(token) , 24);
        } else if(strcmp(table.table_elm[adr].typ , "CHARSY") == 0){
            if(strcmp(curtyp , "CHARSY") == 0 || strcmp(curtyp , "CHARCON") == 0){
                ;
            } else
                error(cc - strlen(token) , 24);
        }*/

        strcpy(op , "BECOM");
        strcpy(v1 , curobj);
        strcpy(v2 , "\0");
        strcpy(v3 , table.table_elm[adr].name);
        fillmidcode();
		return 0;
	} else {
		error(cc - strlen(token) , 24);
	}

	return -1;
}

int ScanfSentence(){
	printf("This is 读语句\n");
	if(strcmp(sym , "SCANFSY") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "LPARENT") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "IDENT") == 0){
//        fillmidcode("SCANF" , "\0" , token , "\0");
        strcpy(op , "SCANF");
        strcpy(v1 , "\0");
        strcpy(v2 , token);
        strcpy(v3 , "\0");
        fillmidcode();
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	while(strcmp(sym , "COMMA") == 0){
		getsym();

		if(strcmp(sym , "IDENT") == 0){
//			fillmidcode("SCANF" , "\0" , token , "\0");
            strcpy(op , "SCANF");
            strcpy(v1 , "\0");
            strcpy(v2 , token);
            strcpy(v3 , "\0");
            fillmidcode();
			getsym();
		} else {
			error(cc - strlen(token) , 24);
		}

	}

	if(strcmp(sym , "RPARENT") == 0){
		getsym();
		return 0;
	} else {
		error(cc - strlen(token) , 24);
	}

	return -1;
}

int PrintfSentence(){
	printf("This is 写语句\n");
	char tmpstr[AL];
	int flag = 0;

	if(strcmp(sym , "PRINTFSY") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "LPARENT") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "STRCON") == 0){
		strcpy(tmpstr , token);
		getsym();

		if(strcmp(sym , "COMMA") == 0){
            getsym();
			Expression();
			flag = 1;
		}
	} else if (strcmp(sym , "PLUS") == 0 || strcmp(sym , "MINU") == 0 || strcmp(sym , "IDENT") == 0 || strcmp(sym , "CHARCON") == 0 || strcmp(sym , "NUM") == 0 || strcmp(sym , "ZERO") == 0 || strcmp(sym , "LPARENT") == 0){
		Expression();
		flag = 2;
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "RPARENT") == 0){
        if(flag == 0){
//            fillmidcode("PRINTF" , tmpstr , "\0" , "\0");
            strcpy(op , "PRINTF");
            strcpy(v1 , tmpstr);
            strcpy(v2 , "\0");
            strcpy(v3 , "\0");
            fillmidcode();
        } else if (flag == 1) {
//            fillmidcode("PRINTF" , tmpstr , curobj , curtyp);
            strcpy(op , "PRINTF");
            strcpy(v1 , tmpstr);
            strcpy(v2 , curobj);
            if(strcmp(curtyp , "CHARCON") == 0 || strcmp(curtyp , "INTCON") == 0)
                strcpy(v3 , curtyp);
            else
                strcpy(v3 , "\0");
            fillmidcode();
        } else if (flag == 2){
//            fillmidcode("PRINTF" , "\0" , curobj , curtyp);
            strcpy(op , "PRINTF");
            strcpy(v1 , "\0");
            strcpy(v2 , curobj);
            if(strcmp(curtyp , "CHARCON") == 0 || strcmp(curtyp , "INTCON") == 0)
                strcpy(v3 , curtyp);
            else
                strcpy(v3 , "\0");
            fillmidcode();
        }
		getsym();
		return 0;
	} else {
		error(cc - strlen(token) , 24);
	}

	return -1;
}

int ReturnSentence(){
	printf("This is 返回语句\n");
	if(strcmp(sym , "RETURNSY") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "LPARENT") == 0){
		getsym();
	} else {
	    strcpy(op , "RET");
        strcpy(v1 , "\0");
        strcpy(v2 , "\0");
        strcpy(v3 , "\0");
        fillmidcode();
		return 0;
	}

	if (strcmp(sym , "PLUS") == 0 || strcmp(sym , "MINU") == 0 || strcmp(sym , "IDENT") == 0 || strcmp(sym , "CHARCON") == 0 || strcmp(sym , "NUM") == 0 || strcmp(sym , "ZERO") == 0 || strcmp(sym , "LPARENT") == 0){
		Expression();
	} else {
		error(cc - strlen(token) , 24);
	}
/*
    if(strcmp(curtyp , "INTSY") == 0 || strcmp(curtyp , "INTCON") == 0){
        if(strcmp(table.table_elm[table.proc_index[table.proc_num-1]].typ , "INTSY") != 0)
            error(cc - strlen(token) , 24);
    } else if(strcmp(curtyp , "CHARSY") == 0 || strcmp(curtyp , "CHARCON") == 0){
        if(strcmp(table.table_elm[table.proc_index[table.proc_num-1]].typ , "CHARSY") != 0)
            error(cc - strlen(token) , 24);
    }*/

    table.table_elm[table.proc_index[table.proc_num-1]].parastyp.returnnum++;

	if(strcmp(sym , "RPARENT") == 0){
//        fillmidcode("RET" , "\0" , curobj , "\0" );
        strcpy(op , "RET");
        strcpy(v1 , "\0");
        strcpy(v2 , curobj);
        strcpy(v3 , "\0");
        fillmidcode();
		getsym();
		return 0;
	} else {
		error(cc - strlen(token) , 24);
	}

	return -1;
}

int Sentence(){
	printf("This is 语句\n");
	char tmptoken[AL];
	int adr;

	if (strcmp(sym , "IFSY") == 0) {
        IfSentence();
        return 0;
    } else if (strcmp(sym , "DOSY") == 0) {
        WhileSentence();
        return 0;
    } else if (strcmp(sym , "FORSY") == 0) {
        ForSentence();
        return 0;
    } else if(strcmp(sym , "IDENT") == 0) {
    	strcpy(tmptoken , token);
    	getsym();

    	if(strcmp(sym , "BECOMES") == 0 || strcmp(sym , "LBRACK") == 0){
    		BecomeSentence(tmptoken);
		} else if(strcmp(sym , "LPARENT") == 0){
		    adr = Position(tmptoken , 0);
			if(strcmp(table.table_elm[adr].typ , "VOIDSY") == 0 && strcmp(table.table_elm[adr].obj , "FUNCOBJ") == 0){
				vFuncCall(adr);
			} else if ((strcmp(table.table_elm[adr].typ , "INTSY") == 0 || strcmp(table.table_elm[adr].typ , "CHARSY") == 0) && strcmp(table.table_elm[adr].obj , "FUNCOBJ") == 0){
				rFuncCall(adr);
			} else {
				error(cc - strlen(token) , 24);
			}
		} else {
			error(cc - strlen(token) , 24);
		}
	} else if(strcmp(sym , "SCANFSY") == 0){
		ScanfSentence();
	} else if(strcmp(sym , "PRINTFSY") == 0){
		PrintfSentence();
	} else if(strcmp(sym , "RETURNSY") == 0){
		ReturnSentence();
	} else if (strcmp(sym , "LBRACE") == 0) {
        getsym();
        Sentences();
        if (strcmp(sym , "RBRACE") == 0) {
            getsym();
            return 0;
        } else {
         	error(cc - strlen(token) , 24);
		}
    }

    if(strcmp(sym , "SEMICOLON") == 0){
    	getsym();
		return 0;
	} else {
		error(cc - strlen(token) , 24);
	}

	return -1;
}

int Sentences(){
	printf("This is 语句列\n");
	while(strcmp(sym , "IFSY") == 0 || strcmp(sym , "DOSY") == 0 || strcmp(sym , "FORSY") == 0 || strcmp(sym , "LBRACE") == 0 || strcmp(sym , "IDENT") == 0 || strcmp(sym , "SCANFSY") == 0 || strcmp(sym , "PRINTFSY") == 0 || strcmp(sym ,"RETURNSY" ) == 0 || strcmp(sym ,"SEMICOLON" ) == 0){
		Sentence();
	}

	return 0;
}

int ComplexSentence(){
	printf("This is 复合语句\n");

	if(strcmp(sym , "CONSTSY") == 0){
		ConstSta();
	}

	if( strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0){
		VarSta();
	}

	if(strcmp(sym , "CONSTSY") == 0){
		error(cc-7 , 11);//TODO 跳读到int或char
	}

	Sentences();

	if(strcmp(sym , "RBRACE") != 0){
		getsym();
	}
	return 0;
}

int rFuncDef(){
	printf("This is 有返回值函数定义\n");

	char typ[AL];
	int tabadr = 0;
	char obj[] = "FUNCOBJ";
	value v;

	if(strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0){
		strcpy(typ , sym);
		getsym();
	} else {
		error(cc - strlen(token) , 24);
	}

	if(strcmp(sym , "IDENT") == 0){
		tabadr = enter(token, obj, typ, v, 0, 0, 0);//TODO 插入失败输出错误程序结束
        table.table_elm[tabadr].parastyp.returnnum = 0;

		getsym();
	} else {
		error(cc - strlen(token) , 20);
	}

	if(strcmp(sym , "LPARENT") == 0){
        //        fillmidcode(obj , typ , token , num2str(table.table_elm[tabadr].para));
        strcpy(op , obj);
        strcpy(v1 , typ);
        strcpy(v2 , table.table_elm[tabadr].name);
        strcpy(v3 , "\0");
        fillmidcode();

		getsym();
		Parameters(tabadr);
	} else {
		error(cc - strlen(token) , 25);
	}

	if(strcmp(sym , "RPARENT") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 16);
	}

	if(strcmp(sym , "LBRACE") == 0){
		getsym();
		ComplexSentence();
	} else {
		error(cc - strlen(token) , 15);
	}

	if(strcmp(sym , "RBRACE") == 0){

//        fillmidcode("END" , "\0" , table.table_elm[tabadr].name , "\0");

        if(table.table_elm[table.proc_index[table.proc_num-1]].parastyp.returnnum == 0){
            error(cc - strlen(token) , 15);
        }

        strcpy(op , "END");
        strcpy(v1 , "\0");
        strcpy(v2 , table.table_elm[tabadr].name);
        strcpy(v3 , "\0");
        fillmidcode();
        Flush();

		getsym();
		return 0;
	} else {
		error(cc - strlen(token) , 16);
	}

	return -1;
}

int vFuncDef(){
	printf("This is 无返回值函数定义\n");

	int tabadr = 0;
	char obj[] = "FUNCOBJ";
	char typ[] = "VOIDSY";
	value v;

	if(strcmp(sym , "IDENT") == 0){
		tabadr = enter(token, obj, typ, v, 0, 0, 0);//TODO 插入失败输出错误程序结束
		table.table_elm[tabadr].parastyp.returnnum = 0;

		getsym();
	} else {
		error(cc - strlen(token) , 20);
	}

	if(strcmp(sym , "LPARENT") == 0){
        //		fillmidcode(obj , typ , token , num2str(table.table_elm[tabadr].para));
        strcpy(op , obj);
        strcpy(v1 , typ);
        strcpy(v2 , table.table_elm[tabadr].name);
        strcpy(v3 , "\0");
        fillmidcode();

		getsym();
		Parameters(tabadr);
	} else {
		error(cc - strlen(token) , 25);
	}

	if(strcmp(sym , "RPARENT") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 16);
	}

	if(strcmp(sym , "LBRACE") == 0){
		getsym();
		ComplexSentence();
	} else {
		error(cc - strlen(token) , 15);
	}

	if(strcmp(sym , "RBRACE") == 0){

//        fillmidcode("END" , "\0" , table.table_elm[tabadr].name , "\0");
        if(table.table_elm[table.proc_index[table.proc_num-1]].parastyp.returnnum != 0){
            error(cc - strlen(token) , 15);
        }

        strcpy(op , "END");
        strcpy(v1 , "\0");
        strcpy(v2 , table.table_elm[tabadr].name);
        strcpy(v3 , "\0");
        fillmidcode();
        Flush();

		getsym();
		return 0;
	} else {
		error(cc - strlen(token) , 16);
	}

	return -1;
}

int Procedure(){
	printf("This is 程序\n");

	int tmpcc;
	int tmpln;

	getsym();

	if(strcmp(sym , "CONSTSY") == 0){
		ConstSta();
	}

	if( strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0){
		VarSta();
	}

	if(strcmp(sym , "CONSTSY") == 0){
		error(cc-7 , 11);//TODO 跳读到int或char
	}

	while(strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0 || strcmp(sym , "VOIDSY") == 0){
		tmpcc = cc - strlen(token) - 1;
		tmpln = ln;

		if(strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0){
			rFuncDef();
		} else {
			getsym();
			if (strcmp(sym , "IDENT") == 0) {
				vFuncDef();
			} else {
				skipback(tmpcc , tmpln);
				getsym();
				break;
			}
		}
	}

	if(strcmp(sym , "VOIDSY") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 12);
	}

	if(strcmp(sym , "MAINSY") == 0){

//        fillmidcode("FUNCOBJ" , "VOIDSY" , "main" , "\0");
        char name[10] , obj[10] , typ[10];
        value v;

        strcpy(name , "main");
        strcpy(obj , "FUNCOBJ");
        strcpy(typ , "VOIDSY");
        enter(name , obj , typ , v , 0 , 0 , 0);

        strcpy(op , "FUNCOBJ");
        strcpy(v1 , "VOIDSY");
        strcpy(v2 , "main");
        strcpy(v3 , "\0");
        fillmidcode();

		getsym();
	} else {
		error(cc - strlen(token) , 13);
	}

	if(strcmp(sym , "LPARENT") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 14);
	}

	if(strcmp(sym , "RPARENT") == 0){
		getsym();
	} else {
		error(cc - strlen(token) , 16);
	}

	if(strcmp(sym , "LBRACE") == 0){
		getsym();
		ComplexSentence();
	} else {
		error(cc - strlen(token) , 15);
	}

	if(strcmp(sym , "RBRACE") == 0){

//        fillmidcode("END" , "\0" , "\0" , "main");
        strcpy(op , "END");
        strcpy(v1 , "\0");
        strcpy(v2 , "\0");
        strcpy(v3 , "main");
        fillmidcode();
        Flush();
        free_tmpreg();
		return 0;
	} else {
		error(cc - strlen(token) , 16);
	}

	return -1;
}
//"CONSTSY","INTSY","CHARSY","VOIDSY","MAINSY","COMMA","SEMICOLON"
//"LBRACK","IDENT","BECOMES","LPARENT"
//TODO全局变量、常量都放在堆中

int interpret(){
    rsT<<"\t.data"<<endl;
    rsT<<"NewLine:"<<".asciiz\t"<<"\"\\n\""<<endl;
    rsT<<"\t.text"<<endl;
    rsT<<"\taddu\t$fp\t$sp\t$0"<< endl;
    rsT<<"\tori\t$t9\t$0\t"<< INITDATA << endl;

    while(strcmp(midcode[tpc].op , "CONSTOBJ") == 0){
        if(strcmp(midcode[tpc].v1 , "INTSY") == 0){
            insertadrtab(midcode[tpc].v2 , hp , midcode[tpc].v1 , 1);
            pushdata(1 , midcode[tpc].v3);
        } else if(strcmp(midcode[tpc].v1 , "CHARSY") == 0){
            insertadrtab(midcode[tpc].v2 , hp , midcode[tpc].v1 , 1);
            pushdata(1 , num2str((int)midcode[tpc].v3[0]));
        }
        tpc++;
    }

    while(strcmp(midcode[tpc].op , "VAROBJ") == 0 || strcmp(midcode[tpc].op , "AVAROBJ") == 0){
        char value[] = "\0";
        if(strcmp(midcode[tpc].op , "VAROBJ") == 0){
            insertadrtab(midcode[tpc].v2 , hp , midcode[tpc].v1 , 1);
            pushdata(1 , value);
        } else if (strcmp(midcode[tpc].op , "AVAROBJ") == 0){
            insertadrtab(midcode[tpc].v2 , hp , midcode[tpc].v1 , 1);
            pushdata(atoi(midcode[tpc].v3) , value);
        }
        tpc++;
    }

    while(strcmp(midcode[tpc].op , "FUNCOBJ") == 0){
        insertadrtab(midcode[tpc].v2 , -1 , midcode[tpc].v1 , -1);
        fx++;
        if(strcmp(midcode[tpc].v2 , "main") == 0){
            ismain = 1;
            rsT<<"_Start:"<<endl;
        } else {
            rsT<<"\tj\t_END_" <<fx<<endl;
            rsT<<midcode[tpc].v2<<":"<<endl;
        }
        tpc++;
        FUNCOBJasm();
    }
    return 0;
}

int pushstack(int length){
    sp -= ( 4 * length );
    rsT << "\tsubi\t$sp\t$sp\t" << 4 * length << endl;
    return 0;
}

int pushdata(int length , char* value){
    if(strcmp(value , "\0") != 0){
        rsT << "\tli\t$t0\t" << value << "\t#" << midcode[tpc].v2 << "=" << midcode[tpc].v3 << endl;
        rsT << "\tli\t$t1\t" << hp+INITDATA << endl;
        rsT << "\tsw\t$t0\t($t1)" << endl;
    }
    hp += ( 4 * length );
    return 0;
}

int insertadrtab(char* name , int adr , char* typ , int glob){
    strcpy(addrtable[ax].name , name);
    addrtable[ax].addr = adr;
    addrtable[ax].glob = glob;
    strcpy(addrtable[ax].typ , typ);
    ax++;
    return 0;
}

//查找汇编变量地址
int varaddr(char *name) {
    if(ax == 0) return -1;
    int t = ax - 1;
    isglob = 0;
    while ( t >= 0 ) {
        if ( strcmp(addrtable[ t ].name, name) == 0 ) {
            isglob = addrtable[ t ].glob;
            return addrtable[ t ].addr;
        }
        t--;
    }
    return -1;
}

int findaddr(char *name) {
    if(ax == 0) return -1;
    int t = ax - 1;
    while ( t >= 0 ) {
        if ( strcmp(addrtable[ t ].name, name) == 0 ) {
            return t;
        }
        t--;
    }
    return -1;
}

//   JMP ,  ,  ,
void JMPasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    rsT << "\tj\t" << midcode[ tpc ].v2 << endl;
    tpc++;
}

//    JEQ
void JEQasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    rsT << "\tbne\t$t0\t0\t" << midcode[ tpc ].v2 << endl;
    tpc++;
}

//    JNE
void JNEasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    rsT << "\tbne\t$t0\t1\t" << midcode[ tpc ].v2 << endl;
    tpc++;
}
//    LAB,  ,  ,
void LABasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    rsT << midcode[ tpc ].v2 << ":"<<endl;
    tpc++;
}


//    PLUS, a, b, c
void PLUSasm() {
    int addr1, addr2, addr3;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if (isNum(midcode[ tpc ].v1) || isChar(midcode[ tpc ].v1)) {
        rsT << "\tli\t$t0\t" << midcode[ tpc ].v1 << endl;
    } else {
        addr1 = varaddr(midcode[ tpc ].v1);
        if ( isglob )
            rsT << "\tlw\t$t0\t" << addr1 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t0\t" << addr1 << "($fp)" << endl;
    }
    if (isNum(midcode[ tpc ].v2) || isChar(midcode[ tpc ].v2)) {
        rsT << "\tli\t$t1\t" << midcode[ tpc ].v2 << endl;
    } else {
        addr2 = varaddr(midcode[ tpc ].v2);
        if ( isglob )
            rsT << "\tlw\t$t1\t" << addr2 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t1\t" << addr2 << "($fp)" << endl;
    }
    addr3 = varaddr(midcode[ tpc ].v3);
    rsT << "\tadd\t$t0\t$t0\t$t1" << endl;
    if ( isglob )
        rsT << "\tsw\t$t0\t" << addr1 << "($t9)" << endl;
    else
        rsT << "\tsw\t$t0\t" << addr3 << "($fp)" << endl;
    tpc++;
}

//    MINU, a, b, c
void MINUasm() {
    int addr1, addr2, addr3;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if (isNum(midcode[ tpc ].v1) || isChar(midcode[ tpc ].v1) ) {
        rsT << "\tli\t$t0\t" << midcode[ tpc ].v1 << endl;
    } else {
        addr1 = varaddr(midcode[ tpc ].v1);
        if ( isglob )
            rsT << "\tlw\t$t0\t" << addr1 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t0\t" << addr1 << "($fp)" << endl;
    }
    if (isNum(midcode[ tpc ].v2) || isChar(midcode[ tpc ].v2)) {
        rsT << "\tli\t$t1\t" << midcode[ tpc ].v2 << endl;
    } else {
        addr2 = varaddr(midcode[ tpc ].v2);
        if ( isglob )
            rsT << "\tlw\t$t1\t" << addr2 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t1\t" << addr2 << "($fp)" << endl;
    }
    addr3 = varaddr(midcode[ tpc ].v3);
    rsT << "\tsub\t$t0\t$t0\t$t1" << endl;
    if ( isglob )
        rsT << "\tsw\t$t0\t" << addr3 << "($t9)" << endl;
    else
        rsT << "\tsw\t$t0\t" << addr3 << "($fp)" << endl;
    tpc++;
}

//    MULT, a, b, c
void MULTasm() {
    int addr1, addr2, addr3;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if (isNum(midcode[ tpc ].v1) || isChar(midcode[ tpc ].v1)) {
        rsT << "\tli\t$t0\t" << midcode[ tpc ].v1 << endl;
    } else {
        addr1 = varaddr(midcode[ tpc ].v1);
        if ( isglob )
            rsT << "\tlw\t$t0\t" << addr1 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t0\t" << addr1 << "($fp)" << endl;
    }
    if (isNum(midcode[ tpc ].v2) || isChar(midcode[ tpc ].v2)) {
        rsT << "\tli\t$t1\t" << midcode[ tpc ].v2 << endl;
    } else {
        addr2 = varaddr(midcode[ tpc ].v2);
        if ( isglob )
            rsT << "\tlw\t$t1\t" << addr2 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t1\t" << addr2 << "($fp)" << endl;
    }
    addr3 = varaddr(midcode[ tpc ].v3);
    rsT << "\tmul\t$t0\t$t0\t$t1" << endl;
    if ( isglob )
        rsT << "\tsw\t$t0\t" << addr3 << "($t9)" << endl;
    else
        rsT << "\tsw\t$t0\t" << addr3 << "($fp)" << endl;
    tpc++;
}

//    DIV, a, b, c
void DIVasm() {
    int addr1, addr2, addr3;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if (isNum(midcode[ tpc ].v1) || isChar(midcode[ tpc ].v1)) {
        rsT << "\tli\t$t0\t" << midcode[ tpc ].v1 << endl;
    } else {
        addr1 = varaddr(midcode[ tpc ].v1);
        if ( isglob )
            rsT << "\tlw\t$t0\t" << addr1 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t0\t" << addr1 << "($fp)" << endl;
    }
    if (isNum(midcode[ tpc ].v2) || isChar(midcode[ tpc ].v2)) {
        rsT << "\tli\t$t1\t" << midcode[ tpc ].v2 << endl;
    } else {
        addr2 = varaddr(midcode[ tpc ].v2);
        if ( isglob )
            rsT << "\tlw\t$t1\t" << addr2 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t1\t" << addr2 << "($fp)" << endl;
    }
    addr3 = varaddr(midcode[ tpc ].v3);
    rsT << "\tdiv\t$t0\t$t0\t$t1" << endl;
    if ( isglob )
        rsT << "\tsw\t$t0\t" << addr3 << "($t9)" << endl;
    else
        rsT << "\tsw\t$t0\t" << addr3 << "($fp)" << endl;
    tpc++;
}

//   GTR , a, b, c
void GTRasm() {
    int addr1, addr2;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if (isNum(midcode[ tpc ].v1) || isChar(midcode[ tpc ].v1)) {
        rsT << "\tli\t$t0\t" << midcode[ tpc ].v1 << endl;
    } else {
        addr1 = varaddr(midcode[ tpc ].v1);
        if ( isglob )
            rsT << "\tlw\t$t0\t" << addr1 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t0\t" << addr1 << "($fp)" << endl;
    }
    if (isNum(midcode[ tpc ].v2) || isChar(midcode[ tpc ].v2)) {
        rsT << "\tli\t$t1\t" << midcode[ tpc ].v2 << endl;
    } else {
        addr2 = varaddr(midcode[ tpc ].v2);
        if ( isglob )
            rsT << "\tlw\t$t1\t" << addr2 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t1\t" << addr2 << "($fp)" << endl;
    }
    rsT << "\tslt\t$t0\t$t1\t$t0" << endl;
    tpc++;
}

//    GEQ
void GEQasm() {
    int addr1, addr2;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if (isNum(midcode[ tpc ].v1) || isChar(midcode[ tpc ].v1)) {
        rsT << "\tli\t$t0\t" << midcode[ tpc ].v1 << endl;
    } else {
        addr1 = varaddr(midcode[ tpc ].v1);
        if ( isglob )
            rsT << "\tlw\t$t0\t" << addr1 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t0\t" << addr1 << "($fp)" << endl;
    }
    if (isNum(midcode[ tpc ].v2) || isChar(midcode[ tpc ].v2)) {
        rsT << "\tli\t$t1\t" << midcode[ tpc ].v2 << endl;
    } else {
        addr2 = varaddr(midcode[ tpc ].v2);
        if ( isglob )
            rsT << "\tlw\t$t1\t" << addr2 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t1\t" << addr2 << "($fp)" << endl;
    }
    rsT << "\tslt\t$t0\t$t0\t$t1" << endl;
    rsT << "\tli\t$t1\t1" << endl;
    rsT << "\tsub\t$t0\t$t1\t$t0" << endl;
    tpc++;
}

//    LSS
void LSSasm() {
    int addr1, addr2;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if (isNum(midcode[ tpc ].v1) || isChar(midcode[ tpc ].v1)) {
        rsT << "\tli\t$t0\t" << midcode[ tpc ].v1 << endl;
    } else {
        addr1 = varaddr(midcode[ tpc ].v1);
        if ( isglob )
            rsT << "\tlw\t$t0\t" << addr1 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t0\t" << addr1 << "($fp)" << endl;
    }
    if (isNum(midcode[ tpc ].v2) || isChar(midcode[ tpc ].v2)) {
        rsT << "\tli\t$t1\t" << midcode[ tpc ].v2 << endl;
    } else {
        addr2 = varaddr(midcode[ tpc ].v2);
        if ( isglob )
            rsT << "\tlw\t$t1\t" << addr2 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t1\t" << addr2 << "($fp)" << endl;
    }
    rsT << "\tslt\t$t0\t$t0\t$t1" << endl;
    tpc++;
}

//    LEQ
void LEQasm() {
    int addr1, addr2;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if (isNum(midcode[ tpc ].v1) || isChar(midcode[ tpc ].v1)) {
        rsT << "\tli\t$t0\t" << midcode[ tpc ].v1 << endl;
    } else {
        addr1 = varaddr(midcode[ tpc ].v1);
        if ( isglob )
            rsT << "\tlw\t$t0\t" << addr1 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t0\t" << addr1 << "($fp)" << endl;
    }
    if (isNum(midcode[ tpc ].v2) || isChar(midcode[ tpc ].v2)) {
        rsT << "\tli\t$t1\t" << midcode[ tpc ].v2 << endl;
    } else {
        addr2 = varaddr(midcode[ tpc ].v2);
        if ( isglob )
            rsT << "\tlw\t$t1\t" << addr2 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t1\t" << addr2 << "($fp)" << endl;
    }
    rsT << "\tslt\t$t0\t$t1\t$t0" << endl;
    rsT << "\tli\t$t1\t1" << endl;
    rsT << "\tsub\t$t0\t$t1\t$t0" << endl;
    tpc++;
}

//    EQL
void EQLasm() {
    int addr1, addr2;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if (isNum(midcode[ tpc ].v1) || isChar(midcode[ tpc ].v1)) {
        rsT << "\tli\t$t0\t" << midcode[ tpc ].v1 << endl;
    } else {
        addr1 = varaddr(midcode[ tpc ].v1);
        if ( isglob )
            rsT << "\tlw\t$t0\t" << addr1 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t0\t" << addr1 << "($fp)" << endl;
    }
    if (isNum(midcode[ tpc ].v2) || isChar(midcode[ tpc ].v2)) {
        rsT << "\tli\t$t1\t" << midcode[ tpc ].v2 << endl;
    } else {
        addr2 = varaddr(midcode[ tpc ].v2);
        if ( isglob )
            rsT << "\tlw\t$t1\t" << addr2 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t1\t" << addr2 << "($fp)" << endl;
    }

    int t1 = tmplab++;
    int t2 = tmplab++;
    rsT << "\tbne\t$t0\t$t1\t__tLABEL" << t1 << endl;
    rsT << "\tli\t$t0\t1" << endl;
    rsT << "\tj\t__tLABEL" << t2 << endl;
    rsT << "__tLABEL" << t1 << ":" << endl;
    rsT << "\tli\t$t0\t0" << endl;
    rsT << "__tLABEL" << t2 << ":" << endl;
    tpc++;
}

//   NEQ
void NEQasm() {
    int addr1, addr2;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if (isNum(midcode[ tpc ].v1) || isChar(midcode[ tpc ].v1)) {
        rsT << "\tli\t$t0\t" << midcode[ tpc ].v1 << endl;
    } else {
        addr1 = varaddr(midcode[ tpc ].v1);
        if ( isglob )
            rsT << "\tlw\t$t0\t" << addr1 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t0\t" << addr1 << "($fp)" << endl;
    }
    if (isNum(midcode[ tpc ].v2) || isChar(midcode[ tpc ].v2)) {
        rsT << "\tli\t$t1\t" << midcode[ tpc ].v2 << endl;
    } else {
        addr2 = varaddr(midcode[ tpc ].v2);
        if ( isglob )
            rsT << "\tlw\t$t1\t" << addr2 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t1\t" << addr2 << "($fp)" << endl;
    }
    int t1 = tmplab++;
    int t2 = tmplab++;
    rsT << "\tbeq\t$t0\t$t1\t__tLABEL" << t1 << endl;
    rsT << "\tli\t$t0\t1" << endl;
    rsT << "\tj\t__tLABEL" << t2 << endl;
    rsT << "__tLABEL" << t1 << ":" << endl;
    rsT << "\tli\t$t0\t0" << endl;
    rsT << "__tLABEL" << t2 << ":" << endl;
    tpc++;
}

//    BECOM
void BECOMasm() {
    int addr1, addr2;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if (isNum(midcode[ tpc ].v1) || isChar(midcode[ tpc ].v1)) {
        rsT << "\tli\t$t0\t" << midcode[tpc].v1 << endl;
    } else {
        addr1 = varaddr(midcode[tpc].v1);
        if ( isglob )
            rsT << "\tlw\t$t0\t" << addr1 << "($t9)" << endl;
        else
            rsT << "\tlw\t$t0\t" << addr1 << "($fp)" << endl;
    }
    addr2 = varaddr(midcode[tpc].v3);
    if (isglob )
        rsT << "\tsw\t$t0\t" << addr2 << "($t9)" << endl;
    else
        rsT << "\tsw\t$t0\t" << addr2 << "($fp)" << endl;
    tpc++;
}

//    []= , a , i , t
void VARRAYasm() {
    int addr1, addr2, addrt;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    addr1 = varaddr(midcode[tpc].v3);
    int tisglob = isglob;
    if ( isNum(midcode[tpc].v2)) {//数组下标
        addr1 += (atoi(midcode[tpc].v2) * 4);
        if (isNum(midcode[ tpc ].v1) || isChar(midcode[ tpc ].v1)) {
            rsT << "\tli\t$t0\t" << midcode[tpc].v1 << endl;
        } else {
            addr2 = varaddr(midcode[tpc].v1);
            if ( isglob ) {
                rsT << "\tlw\t$t0\t" << addr2 << "($t9)" << endl;
            }else
                rsT << "\tlw\t$t0\t" << addr2 << "($fp)" << endl;
        }
        if (isglob)
            rsT << "\tsw\t$t0\t" << addr1 << "($t9)" << endl;
        else
            rsT << "\tsw\t$t0\t" << addr1 << "($fp)" << endl;
    } else {
        //求数组元素a[i]地址
        addrt = varaddr(midcode[tpc].v2);//addrt = &i
        if (isglob){
            rsT << "\tlw\t$t1\t" << addrt << "($t9)" << endl;    //t1 = i
        }else{
            rsT << "\tlw\t$t1\t" << addrt << "($fp)" << endl;    //t1 = i
        }
        rsT << "\tmul\t$t1\t$t1\t4\n";    //t1 = t1 * 4 (t1 = offset)
        if(tisglob){
            rsT << "\taddi\t$t1\t$t1\t" << addr1 << endl;    //t1 = &a[i] - $fp
            rsT << "\tadd\t$t1\t$t1\t$t9" << endl;
        }else{
            rsT << "\tsubi\t$t1\t$t1\t" << addr1 << endl;    //t1 = &a[i] - $fp
            rsT << "\tsub\t$t1\t$fp\t$t1" << endl;
        }
        if (isNum(midcode[ tpc ].v1) || isChar(midcode[ tpc ].v1)) {
            rsT << "\tli\t$t0\t" << midcode[tpc].v1 << endl;
        } else {
            addr2 = varaddr(midcode[tpc].v1);
            if ( isglob ) {
                rsT << "\tlw\t$t0\t" << addr2 << "($t9)" << endl;
            } else {
                rsT << "\tlw\t$t0\t" << addr2 << "($fp)" << endl;
            }

        }
        rsT << "\tsw\t$t0\t0($t1)" << endl;
    }
    tpc++;
}

//AVALUE, a, n, b
void AVALUEasm() {
    int addr1 = varaddr(midcode[tpc].v1);
    int tisglob = isglob;
    int addr2;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if (isNum(midcode[ tpc ].v2) || isChar(midcode[ tpc ].v2)) {
        addr1 += ( atoi(midcode[tpc].v2) * 4 );    //addr1 = &a[n]
        addr2 = varaddr(midcode[tpc].v3);        //addr2 = &b
        if ( tisglob ) {
            rsT << "\tlw\t$t0\t" << addr1 << "($t9)" << endl;
        }else
            rsT << "\tlw\t$t0\t" << addr1 << "($fp)" << endl;
        if ( isglob ) {
            rsT << "\tsw\t$t0\t" << addr2 << "($t9)" << endl;
        }else
            rsT << "\tsw\t$t0\t" << addr2 << "($fp)" << endl;
    } else {
        //求数组元素a[i]地址
        int addrt = varaddr(midcode[tpc].v2);//addrt = &i
        if ( isglob ) {
            rsT << "\tlw\t$t1\t" << addrt << "($t9)" << endl;    //t1 = i
        }else{
            rsT << "\tlw\t$t1\t" << addrt << "($fp)" << endl;    //t1 = i
        }
        rsT << "\tmul\t$t1\t$t1\t4\n";    //t1 = t1 * 4 (t1 = offset)
        if(tisglob){
            rsT << "\taddi\t$t1\t$t1\t" << addr1 << endl;    //t1 = &a[i] - $fp
            rsT << "\tadd\t$t1\t$t1\t$t9" << endl;
        }else{
            rsT << "\tsubi\t$t1\t$t1\t" << addr1 << endl;    //t1 = &a[i] - $fp
            rsT << "\tsub\t$t1\t$fp\t$t1" << endl;
        }
        rsT << "\tlw\t$t1\t0($t1)\n";    //t1 = a[i]
        addr2 = varaddr(midcode[tpc].v3);    //addr2 = &b
        if (isglob)
            rsT << "\tsw\t$t1\t" << addr2 << "($t9)" << endl;
        else
            rsT << "\tsw\t$t1\t" << addr2 << "($fp)" << endl;
    }
    tpc++;
}

//SCANF ,   ,   , a
void SCANFasm() {
    int addr;
    addr = varaddr(midcode[tpc].v2);
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if ( strcmp(addrtable[findaddr(midcode[tpc].v2)].typ , "INTSY") == 0 ) {
        rsT << "\tli\t$v0\t5" << endl;
        rsT << "\tsyscall" << endl;
        if (isglob)
            rsT << "\tsw\t$v0\t" << addr << "($t9)" << endl;
        else
            rsT << "\tsw\t$v0\t" << addr << "($fp)" << endl;
    } else {
        rsT << "\tli\t$v0\t12" << endl;
        rsT << "\tsyscall" << endl;
        if ( isglob )
            rsT << "\tsw\t$v0\t" << addr << "($t9)" << endl;
        else
            rsT << "\tsw\t$v0\t" << addr << "($fp)" << endl;
    }
    tpc++;
}

//PRINTF, a, b, symb
void PRINTFasm() {
    int addr;
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if ( midcode[tpc].v1[ 0 ] != '\0' ) {
        for ( int i = 0; i < (int)strlen(midcode[tpc].v1); i++ ) {
            rsT << "\tli\t$v0\t11" << endl;
            rsT << "\tli\t$a0\t" << int(midcode[tpc].v1[i]) << endl;
            rsT << "\tsyscall" << endl;
        }
/*        rsT <<"li $v0, 4 # system call code for print_str"<<endl;
        rsT <<"la $a0, NewLine"<<endl;
        rsT <<"syscall # print the string"<<endl;*/
    }
    if (midcode[tpc].v2[ 0 ] != '\0' ) {
        if (strcmp(midcode[tpc].v3, "CHARCON") == 0 ) {
            rsT << "\tli\t$v0\t11" << endl;
            rsT << "\tli\t$a0\t" << midcode[tpc].v2 << endl;
            rsT << "\tsyscall" << endl;
            tpc++;
            return;
        } else if (strcmp(midcode[tpc].v3, "INTCON") == 0 ) {
            rsT << "\tli\t$v0\t1" << endl;
            rsT << "\tli\t$a0\t" << midcode[tpc].v2 << endl;
            rsT << "\tsyscall" << endl;
            tpc++;
            return;
        }
        addr = varaddr(midcode[tpc].v2);
        if ( strcmp(addrtable[findaddr(midcode[tpc].v2)].typ , "INTSY") == 0 ){
            rsT << "\tli\t$v0\t1" << endl;
            if ( isglob ) {
                rsT << "\tlw\t$a0\t" << addr << "($t9)" << endl;
            }else
                rsT << "\tlw\t$a0\t" << addr << "($fp)" << endl;
            rsT << "\tsyscall" << endl;
        } else {
            rsT << "\tli\t$v0\t11" << endl;
            if ( isglob ){
                rsT << "\tlw\t$a0\t" << addr << "($t9)" << endl;
            }else
                rsT << "\tlw\t$a0\t" << addr << "($fp)" << endl;
            rsT << "\tsyscall" << endl;
        }
    }
    rsT <<"li $v0, 4 # system call code for print_str"<<endl;
    rsT <<"la $a0, NewLine"<<endl;
    rsT <<"syscall # print the string"<<endl;
    tpc++;
}

//VPARA,   ,   , a     ==> a is a function parameter
void VPARAasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if ( isNum(midcode[tpc].v1) || isChar(midcode[tpc].v1)) {
        rsT << "\tli\t$t"<< vtmpreg <<"\t" << midcode[tpc].v1 << endl;
    } else {
        rsT << "\tlw\t$t"<< vtmpreg <<"\t" << varaddr(midcode[tpc].v1); //li    $t0 item
        if(isglob){
            rsT << "($t9)" << endl;
        }else{
            rsT << "($fp)" << endl;
        }
    }
    vtmpreg++;
    if(vtmpreg-2 == addrtable[findaddr(midcode[tpc].v2)].paranum)
        vtmpreg = 2;
    tpc++;
}

//    CALL, f ,   , a
void CALLasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    rsT << "\tjal\t" << midcode[ tpc ].v1 << endl;
    rsT << "\tnop\n";
    if (midcode[ tpc ].v3[ 0 ] != '\0' ) {
        int addr2 = varaddr(midcode[ tpc ].v3);
        if ( isglob )
            rsT << "\tsw\t$v0\t" << addr2 << "($t9)" << endl;
        else
            rsT << "\tsw\t$v0\t" << addr2 << "($fp)" << endl;
    }
    tpc++;
}

//RET ,   ,   , (a)   ==> return a / return
void RETasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if ( midcode[tpc].v2[ 0 ] != '\0' ) {
        if (isNum(midcode[ tpc ].v2) || isChar(midcode[ tpc ].v2)) {
            rsT << "\tli\t$v0\t" << midcode[tpc].v2 << endl;
        } else {
            int addr2 = varaddr(midcode[tpc].v2);
            if ( isglob )
                rsT << "\tlw\t$v0\t" << addr2 << "($t9)" << endl;
            else
                rsT << "\tlw\t$v0\t" << addr2 << "($fp)" << endl;
        }
    }
    rsT << "\tj\t__FEND_LAB_" << fx << endl;//跳至结束
    tpc++;
}

//para, int, , a == > f(int a, ...)
void PARAasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    rsT << "\tsw\t$t"<< ptmpreg <<"\t($sp)"<<endl;
    ptmpreg++;
}

int FUNCOBJasm(){
    sp = 0;

    rsT << "__FSTART_LAB_" << fx << ":" << endl;
    rsT << "\tsw\t$fp\t($sp)" << endl;//存放上一个函数帧指针
    rsT << "\tadd\t$fp\t$sp\t$0" << endl;
    rsT << "\tsubi\t$sp\t$sp\t4" << endl;
    sp -= 4;
    rsT << "\tsw\t$ra\t($sp)" << endl;
    rsT << "\tsubi\t$sp\t$sp\t4" << endl;
    sp -= 4;

    int paranum = 0;
    int funcaddr = ax - 1;
    while(strcmp(midcode[tpc].op , "PARA") == 0){
        PARAasm();
        insertadrtab(midcode[tpc].v2 , sp , midcode[tpc].v1 , 0);
        pushstack(1);
        paranum++;
        tpc++;
    }
    addrtable[funcaddr].paranum = paranum;
    ptmpreg = 2;

    while(strcmp(midcode[tpc].op , "CONSTOBJ") == 0){
        if(strcmp(midcode[tpc].v1 , "INTSY") == 0){
            insertadrtab(midcode[tpc].v2 , hp , midcode[tpc].v1 , 1);
            pushdata(1 , midcode[tpc].v3);
        } else if(strcmp(midcode[tpc].v1 , "CHARSY") == 0){
            insertadrtab(midcode[tpc].v2 , hp , midcode[tpc].v1 , 1);
            pushdata(1 , num2str((int)midcode[tpc].v3[0]));
        }
        tpc++;
    }

    while(strcmp(midcode[tpc].op , "VAROBJ") == 0 || strcmp(midcode[tpc].op , "AVAROBJ") == 0){
        if(strcmp(midcode[tpc].op , "VAROBJ") == 0){
            insertadrtab(midcode[tpc].v2 , sp , midcode[tpc].v1 , 0);
            pushstack(1);
        } else if (strcmp(midcode[tpc].op , "AVAROBJ") == 0){
            insertadrtab(midcode[tpc].v2 , sp , midcode[tpc].v1 , 0);
            pushstack(atoi(midcode[tpc].v3));
        }
        tpc++;
    }
    //分配临时寄存器
    int tmptpc = tpc;
    while ( strcmp(midcode[ tmptpc ].op, "END") != 0 ) {
        if (midcode[ tmptpc ].v3[ 0 ] == '$') {
            rsT << "#" << midcode[ tmptpc ].op <<"\t"<<midcode[ tmptpc ].v1<<"\t"<<midcode[ tmptpc ].v2<<"\t"<<midcode[ tmptpc ].v3<<endl;

            if(strcmp(midcode[ tmptpc ].op, "CALL") == 0){
                insertadrtab(midcode[tmptpc].v3 , sp , addrtable[findaddr(midcode[tmptpc].v1)].typ , 0);
            } else if(strcmp(midcode[ tmptpc ].op, "AVALUE") == 0){
                insertadrtab(midcode[tmptpc].v3 , sp , addrtable[findaddr(midcode[tmptpc].v1)].typ , 0);
            } else {
                char typ[] = "INTSY";
                insertadrtab(midcode[tmptpc].v3 , sp , typ , 0);
            }

            pushstack(1);
        }
        tmptpc++;
    }

    while ( strcmp(midcode[tpc].op, "END") != 0 ) {
        if ( strcmp(midcode[tpc].op, "CALL") == 0 ) CALLasm();
        if ( strcmp(midcode[tpc].op, "VPARA") == 0 ) VPARAasm();
        if ( strcmp(midcode[tpc].op, "AVALUE") == 0 ) AVALUEasm();
        if ( strcmp(midcode[tpc].op, "VARRAY") == 0 ) VARRAYasm();
        if ( strcmp(midcode[tpc].op, "PLUS") == 0 ) PLUSasm();
        if ( strcmp(midcode[tpc].op, "MINU") == 0 ) MINUasm();
        if ( strcmp(midcode[tpc].op, "MULT") == 0 ) MULTasm();
        if ( strcmp(midcode[tpc].op, "DIV") == 0 ) DIVasm();
        if ( strcmp(midcode[tpc].op, "EQL") == 0 ) EQLasm();
        if ( strcmp(midcode[tpc].op, "NEQ") == 0 ) NEQasm();
        if ( strcmp(midcode[tpc].op, "GTR") == 0 ) GTRasm();
        if ( strcmp(midcode[tpc].op, "GEQ") == 0 ) GEQasm();
        if ( strcmp(midcode[tpc].op, "LSS") == 0 ) LSSasm();
        if ( strcmp(midcode[tpc].op, "LEQ") == 0 ) LEQasm();
        if ( strcmp(midcode[tpc].op, "LAB") == 0 ) LABasm();
        if ( strcmp(midcode[tpc].op, "JEQ") == 0 ) JEQasm();
        if ( strcmp(midcode[tpc].op, "JNE") == 0 ) JNEasm();
        if ( strcmp(midcode[tpc].op, "JMP") == 0 ) JMPasm();
        if ( strcmp(midcode[tpc].op, "BECOM") == 0 ) BECOMasm();
        if ( strcmp(midcode[tpc].op, "SCANF") == 0 ) SCANFasm();
        if ( strcmp(midcode[tpc].op, "PRINTF") == 0 ) PRINTFasm();
        if ( strcmp(midcode[tpc].op, "RET") == 0 ) RETasm();
    }
    rsT << "__FEND_LAB_" << fx << ":" << endl;
    rsT << "\tlw\t$ra\t-4($fp)" << endl;
    rsT << "\tadd\t$sp\t$fp\t$0" << endl;
    rsT << "\tlw\t$fp\t($fp)" << endl;
    if ( ismain ) {
        rsT << "\tli\t$v0\t10" << endl;
        rsT << "\tsyscall" << endl;//终止程序
    } else {
        rsT << "\tjr\t$ra\t" << endl;//返回
    }
    if(!ismain) rsT << "_END_" << fx << ":" << endl;
    tpc++;
    return 0;
}

int main(int argc, char** argv) {
//	CONSTSY,INTSY,CHARSY,VOIDSY,MAINSY,
//	IFSY,ELSESY,DOSY,WHILESY,FORSY,SCANFSY,PRINTFSY,RETURNSY

	strcpy(word[0],"const");
	strcpy(word[1],"int");
	strcpy(word[2],"char");
	strcpy(word[3],"void");
	strcpy(word[4],"main");
	strcpy(word[5],"if");
	strcpy(word[6],"else");
	strcpy(word[7],"do");
	strcpy(word[8],"while");
	strcpy(word[9],"for");
	strcpy(word[10],"scanf");
	strcpy(word[11],"printf");
	strcpy(word[12],"return");

	strcpy(wsym[0],"CONSTSY");
	strcpy(wsym[1],"INTSY");
	strcpy(wsym[2],"CHARSY");
	strcpy(wsym[3],"VOIDSY");
	strcpy(wsym[4],"MAINSY");
	strcpy(wsym[5],"IFSY");
	strcpy(wsym[6],"ELSESY");
	strcpy(wsym[7],"DOSY");
	strcpy(wsym[8],"WHILESY");
	strcpy(wsym[9],"FORSY");
	strcpy(wsym[10],"SCANFSY");
	strcpy(wsym[11],"PRINTFSY");
	strcpy(wsym[12],"RETURNSY");
/*
    char FN[50];

    cin >> FN;*/

	if((FP = fopen(FN,"r")) == NULL)
	{
		perror("fail to read");
		exit (1) ;
	}

    rsT.open(OUTFILE);

	ch = ' ';
	while(!feof(FP)){
		Procedure();
		interpret();
//		getsym();
//		while(ll != 0)
//			getsym();
	}
	fclose(FP);
	miD.open("mid_code.txt");
	int testmid = 0;
	for(testmid = 0;testmid < mpc ; testmid++){
        miD << midcode[testmid].op << "\t" << midcode[testmid].v1 << "\t" << midcode[testmid].v2 << "\t" << midcode[testmid].v3 << endl;
	}
    rsT.close();
    miD.close();
	return 0;
}
