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
#define NMAX 10//支持的最大整数长度
#define TMAX 200//符号表最大长度
#define FN "source_code.txt" //文件名
#define RESULTFILE "analysis.txt"
#define OUTFILE "target_code.txt"
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

FILE *FP , *rF;            //文件指针
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
    int returnnum = 0;
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

/////////////////////////////////////////////////////////
struct ENDsym{
    char ends[15][AL];
    int ep;
};

int errnum = 0;
/////////////////////////////////////////////////////////

int Expression(ENDsym* e);
int rFuncCall(ENDsym* e , int adr);
int Sentence(ENDsym* e);
int Sentences(ENDsym* e);

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

void error(int el , const char *ec){
/*    if(strcmp(sym , "CHARCON") == 0) el = el-2
    for(int i=0;i<el;i++){
        fprintf(rF , "_");
    }
    fprintf(rF , "^\t");*/
    errnum++;
	fprintf(rF , "error : in line %d\t%s\n" , ln , ec);
	printf("error : in line %d\t%s\n" , ln , ec);
	//printf("error : in line %d col %d\t%s\n" , ln , el , ec);
//	exit(0);
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
		 	printf("%s\n",line);
			fprintf(rF , "%s",line);
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
				error(cc-AL , "OVER_AL");
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
				error(cc-1,"NOT_DIGIT");
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
					error(cc-NMAX , "OVER_NUM");
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
			error(cc-strlen(token)+1 , "NOT_BECOM");
		}
	} else if(ch == '\"'){
		k = 0;
		token[k] = '\0';
		getch();
		while(ch == ' ' || ch == 32 || ch == 33 || (ch <= 126 && ch >= 35)){
            if(k >= 30){
                error(cc-strlen(token)+1 , "OVER_STR");
                getch();
                continue;
            }
			token[k++] = ch;
			getch();
		}
		if(ch == '\"'){
			strcpy(sym,"STRCON");
			getch();
		}
		else{
			error(cc-strlen(token)+1 , "NOT_DOUBLE");
		}
	}else if(ch == '\''){
		getch();
		if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || isDigit() || isLetter()){
			token[k++] = ch;
			getch();
		}else{
			error(cc-strlen(token)+1 , "NOT_CHAR");
		}
		if(ch == '\''){
			strcpy(sym,"CHARCON");
			getch();
		} else {
			error(cc-strlen(token)+1 , "NOT_SINGLE");
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

int enterENDS(ENDsym* e , const char* sym){
    if(e->ep == 15){
        error(cc - strlen(token)+1 , "OVER_ENDSYM");
        return -1;
    }

    strcpy(e->ends[e->ep] , sym);

    e->ep++;

    return 0;
}

int flushENDS(ENDsym* e){
    e->ep = 0;
    return 0;
}

int skip(ENDsym* e){
    int i;
    while(!feof(FP)){
        for(i = e->ep-1;i>=0;i--){
            if(strcmp(sym , e->ends[i]) == 0){
                //printf("%s",sym);
                return 0;
            }
        }
        getsym();
    }
    return -1;
}

int skipconst(ENDsym* e){
    while(strcmp(sym , "CONSTSY") == 0){
		error(cc-strlen(token)+1 , "CANT_CONSTSTA");
		enterENDS(e , "SEMICOLON");
		skip(e);
        flushENDS(e);
		getsym();
	}
	return 0;
}

int enter(char* name, char* obj, char* typ, value v , int addr , int para , int size){
    if(table.tx > TMAX){
        error(cc - strlen(token)+1 , "OVER_TABLE");//符号表溢出
    }

    if(strcmp(obj,"FUNCOBJ") == 0){
        int j;
        for(j = 0 ; j < table.proc_num ; j++){
            if(strcmp(table.table_elm[table.proc_index[j]].name , name) == 0){
                error(cc - strlen(token)+1 , "FUNC_RENAME");
                return -1;
            }
        }
        table.proc_index[table.proc_num++] = table.tx;
    } else {
        int j;
        if(table.proc_num == 0){
            j = 0;
        }else{
            j = table.proc_index[table.proc_num-1]+1;
        }
        for( ; j < table.tx ; j++){
            if(strcmp(table.table_elm[j].name , name) == 0){
                error(cc - strlen(token)+1 , "VORC_RENAME");
                return -1;
            }
        }
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
    } else if(tag == 2){//如果是变量
        i = table.proc_index[table.proc_num-1]+1;
        while(i < table.tx){
            if((strcmp(table.table_elm[i].name , name) == 0) && (strcmp(table.table_elm[i].obj , "VAROBJ") == 0)){
                return table.table_elm[i].addr;
            }
            i++;
        }
        for(i = 0 ; i < table.proc_index[0] ; i++){
            if((strcmp(table.table_elm[i].name , name) == 0) && (strcmp(table.table_elm[i].obj , "VAROBJ") == 0)){
                return table.table_elm[i].addr;
            }
        }
    }
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
    if(mpc > CMAX){
        error(cc - strlen(token)+1 , "OVER_MIDCODE");//符号表溢出
    }

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

int ConstDef(ENDsym* e , int tp){

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

	if(strcmp(sym , "IDENT") != 0){
		error(cc - strlen(token)-1 , "NOT_IDENT");
        enterENDS(e , "IDENT");
        enterENDS(e , "COMMA");
        enterENDS(e , "SEMICOLON");
        skip(e);
        flushENDS(e);

        if(strcmp(sym , "COMMA") == 0 || strcmp(sym , "SEMICOLON") == 0) return -1;
	}

    strcpy(tmptoken , token);
    getsym();

	if(strcmp(sym , "BECOMES") != 0) {
		error(cc - strlen(token)+1 , "NOT_BECOM");
        enterENDS(e , "BECOMES");
        enterENDS(e , "COMMA");
        enterENDS(e , "SEMICOLON");
        skip(e);
        flushENDS(e);

        if(strcmp(sym , "COMMA") == 0 || strcmp(sym , "SEMICOLON") == 0) return -1;
	}

    getsym();

	if(strcmp(typ , "INTSY") == 0){
		if(strcmp(sym , "PLUS") == 0){
			strcpy(tmpop , "PLUS");
			getsym();
		} else if(strcmp(sym , "MINU") == 0){
			strcpy(tmpop , "MINU");
			getsym();
		}

		if(strcmp(sym , "NUM") != 0 && strcmp(sym , "ZERO") != 0){
			error(cc - strlen(token)+1 , "NOT_INTCON");
            enterENDS(e , "NUM");
            enterENDS(e , "ZERO");
            enterENDS(e , "COMMA");
            enterENDS(e , "SEMICOLON");
            skip(e);
            flushENDS(e);
            if(strcmp(sym , "COMMA") == 0 || strcmp(sym , "SEMICOLON") == 0) return -1;
		}

        if(strcmp(tmpop , "PLUS") == 0){
            v.n = num;
        } else {
            v.n = 0 - num;
        }
        enter(tmptoken, obj, typ, v, 0, 0, 0);

        strcpy(op , obj);
        strcpy(v1 , typ);
        strcpy(v2 , tmptoken);
        strcpy(v3 , num2str(v.n));
        fillmidcode();

        getsym();

        return 0;

	} else {
		if(strcmp(sym , "CHARCON") != 0 ){
			error(cc - strlen(token)+1 , "NOT_CHARCON");
            enterENDS(e , "CHARCON");
            enterENDS(e , "COMMA");
            enterENDS(e , "SEMICOLON");
            skip(e);
            flushENDS(e);
            if(strcmp(sym , "COMMA") == 0 || strcmp(sym , "SEMICOLON") == 0) return -1;
		}

        strcpy(v.c , token);
        enter(tmptoken, obj, typ, v, 0, 0, 0);

        strcpy(op , obj);
        strcpy(v1 , typ);
        strcpy(v2 , tmptoken);
        strcpy(v3 , v.c);
        fillmidcode();

        getsym();

        return 0;

	}

	return -1;
}

int ConstSta(ENDsym* e){
    fprintf(rF , "This is ConstSta\n");
	if(strcmp(sym , "CONSTSY") != 0){
		error(cc - strlen(token)+1 , "NOT_CONSTSY");
        enterENDS(e , "CONSTSY");
		skip(e);
		flushENDS(e);
	}

	while(strcmp(sym , "CONSTSY") == 0){

		getsym();

        if(strcmp(sym , "INTSY") != 0 && strcmp(sym , "CHARSY") != 0) {
			error(cc - strlen(token)+1 , "NOT_TYPESY");
            enterENDS(e , "INTSY");
            enterENDS(e , "CHARSY");
            enterENDS(e , "SEMICOLON");
            skip(e);
            flushENDS(e);
            if(strcmp(sym , "SEMICOLON") == 0) return -1;
		}

		if(strcmp(sym , "INTSY") == 0){
			getsym();
			ConstDef(e,1);
			while(strcmp(sym , "COMMA") == 0){
				getsym();
				ConstDef(e,1);
			}
		} else if (strcmp(sym , "CHARSY") == 0) {
			getsym();
			ConstDef(e,2);
			while(strcmp(sym , "COMMA") == 0){
				getsym();
				ConstDef(e,2);
			}
		}

		if(strcmp(sym , "SEMICOLON") != 0) {
			error(cc - strlen(token)+1 , "NOT_SEMICOLON");
            enterENDS(e , "SEMICOLON");
            enterENDS(e , "INTSY");
            enterENDS(e , "CHARSY");
            enterENDS(e , "CONSTSY");
            skip(e);
            flushENDS(e);
		}
		if(strcmp(sym , "SEMICOLON") == 0) getsym();
	}
	return 0;
}

int VarDef(ENDsym* e , int tp){
    fprintf(rF , "This is VarDef\n");
	char tmptoken[AL];
	char obj[] = "VAROBJ";
	char typ[AL];
	value v;

	if(tp == 1){
		strcpy(typ , "INTSY");
	} else {
		strcpy(typ , "CHARSY");
	}

	if(strcmp(sym , "IDENT") != 0 ) {
		error(cc - strlen(token)+1 , "NOT_IDENT");
        enterENDS(e , "IDENT");
        enterENDS(e , "COMMA");
        enterENDS(e , "SEMICOLON");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "COMMA") == 0 || strcmp(sym , "SEMICOLON") == 0) return -1;
	}

    strcpy(tmptoken , token);
    getsym();
/*
    if(strcmp(sym , "COMMA") != 0 && strcmp(sym , "SEMICOLON") != 0 && strcmp(sym , "LBRACK") != 0) {
		error(cc - strlen(token)+1 , "NOT_VARDEF");
        enterENDS(e , "COMMA");
        enterENDS(e , "SEMICOLON");
        enterENDS(e , "LBRACK");
        skip(e);
        flushENDS(e);
	}*/

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

		if(strcmp(sym , "NUM") !=0 ) {
			error(cc - strlen(token)+1 , "NOT_NUM");
            enterENDS(e , "NUM");
            enterENDS(e , "COMMA");
            enterENDS(e , "SEMICOLON");
            skip(e);
            flushENDS(e);
            if(strcmp(sym , "COMMA") == 0 || strcmp(sym , "SEMICOLON") == 0) return -1;
		}

        enter(tmptoken, obj, typ, v, 0, 0, num);

        strcpy(op , "AVAROBJ");
        strcpy(v1 , typ);
        strcpy(v2 , tmptoken);
        strcpy(v3 ,  num2str(num));
        fillmidcode();

        getsym();

		if(strcmp(sym , "RBRACK") != 0) {
			error(cc - strlen(token)+1 , "NOT_RBRACK");
            enterENDS(e , "RBRACK");
            skip(e);
            flushENDS(e);
		}
        getsym();
        return 0;
	}

	return -1;
}

int VarSta(ENDsym* e){
    fprintf(rF , "This is VarSta\n");
	int tmpcc = 0;
	int tmpln = 0;

	while(strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0){
		tmpcc = cc - strlen(token) - 1;
		tmpln = ln;

		getsym();

		if(strcmp(sym , "IDENT") != 0){
			error(cc-strlen(token)+1 , "NOT_IDENT");
			enterENDS(e , "IDENT");
            skip(e);
            flushENDS(e);
		}

		getsym();

/*        if(strcmp(sym , "COMMA") != 0 && strcmp(sym , "SEMICOLON") != 0 && strcmp(sym , "LBRACK") != 0 && strcmp(sym , "LPARENT") != 0) {
			error(cc-strlen(token)+1 , "NOT_VARDEF");
			enterENDS(e , "COMMA");
			enterENDS(e , "SEMICOLON");
			enterENDS(e , "LBRACK");
			enterENDS(e , "LPARENT");
            skip(e);
            flushENDS(e);
		}*/

		if(strcmp(sym , "COMMA") == 0 || strcmp(sym , "SEMICOLON") == 0 || strcmp(sym , "LBRACK") == 0){
			skipback(tmpcc , tmpln);
			getsym();
		} else if (strcmp(sym , "LPARENT") == 0){
			skipback(tmpcc , tmpln);
			getsym();
			return 0;
		}

		if(strcmp(sym , "INTSY") == 0){
			getsym();
			VarDef(e,1);
			while(strcmp(sym , "COMMA") == 0){
				getsym();
				VarDef(e,1);
			}
		} else if (strcmp(sym , "CHARSY") == 0) {
			getsym();
			VarDef(e,2);
			while(strcmp(sym , "COMMA") == 0){
				getsym();
				VarDef(e,2);
			}
		}/* else {
			error(cc - strlen(token)+1 , "NOT_TYPESY");
		}*/

		if(strcmp(sym , "SEMICOLON") != 0) {
			error(cc - strlen(token)+1 , "NOT_SEMICOLON");
			enterENDS(e , "SEMICOLON");
			enterENDS(e , "INTSY");
            enterENDS(e , "CHARSY");
            skip(e);
            flushENDS(e);
		}
		if(strcmp(sym , "SEMICOLON") == 0) {getsym();skipconst(e);}
	}
	return 0;
}

int Parameters(ENDsym* e , int tabadr){
    fprintf(rF , "This is Parameters\n");
	char typ[AL];
	char obj[] = "VAROBJ";
	value v;
	int i = 0;

	if(strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0){
		strcpy(typ , sym);
		getsym();
	} else {
		return 0;
	}

	if(strcmp(sym , "IDENT") != 0){
		error(cc - strlen(token)+1 , "NOT_IDENT");
		enterENDS(e , "IDENT");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    enter(token, obj, typ, v, 0, 0, 0);//TODO 插入失败输出错误程序结束
    table.table_elm[tabadr].para++;

    strcpy(op , "PARA");
    strcpy(v1 , typ);
    strcpy(v2 , token);
    strcpy(v3 , num2str(table.table_elm[tabadr].para));
    fillmidcode();

    if(strcmp(typ , "INTSY") == 0)
        table.table_elm[tabadr].parastyp.paratyp[i] = 0;
    else
        table.table_elm[tabadr].parastyp.paratyp[i] = 1;
    i++;

    getsym();

	while(strcmp(sym , "COMMA") == 0){
        if(i >= 5){
            error(cc - strlen(token)+1 , "OVER_PARA");
            enterENDS(e , "RPARENT");
            skip(e);
            flushENDS(e);
            return 0;
        }
		getsym();
		if(strcmp(sym , "INTSY") != 0 && strcmp(sym , "CHARSY") != 0){
			error(cc - strlen(token)+1 , "NOT_TYPESY");
			enterENDS(e , "INTSY");
			enterENDS(e , "CHARSY");
			enterENDS(e , "RPARENT");
            skip(e);
            flushENDS(e);
            if(strcmp(sym , "RPARENT") == 0) return getsym();
		}

        strcpy(typ , sym);
        getsym();

		if(strcmp(sym , "IDENT") != 0){
			error(cc - strlen(token)+1 , "NOT_IDENT");
            enterENDS(e , "IDENT");
            enterENDS(e , "RPARENT");
            skip(e);
            flushENDS(e);
            if(strcmp(sym , "RPARENT") == 0) return getsym();
		}

        enter(token, obj, typ, v, 0, 0, 0);//TODO 插入失败输出错误程序结束
        table.table_elm[tabadr].para++;

        strcpy(op , "PARA");
        strcpy(v1 , typ);
        strcpy(v2 , token);
        strcpy(v3 , num2str(table.table_elm[tabadr].para));
        fillmidcode();

        if(strcmp(typ , "INTSY") == 0)
            table.table_elm[tabadr].parastyp.paratyp[i] = 0;
        else
            table.table_elm[tabadr].parastyp.paratyp[i] = 1;
        i++;

        getsym();

	}

	return 0;
}

int Factor(ENDsym* e){
    fprintf(rF , "This is Factor\n");
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
            if(strcmp(sym , "NUM")!= 0){
                error(cc-strlen(token)+1 , "NOT_INTCON");
                enterENDS(e , "NUM");
                skip(e);
                flushENDS(e);
            }

            strcpy(curobj , token);
            strcpy(curtyp , "INTCON");
            getsym();

        }else if(strcmp(sym , "MINU") == 0){
            getsym();
            if(strcmp(sym , "NUM") != 0){
                error(cc-strlen(token)+1 , "NOT_INTCON");
                enterENDS(e , "NUM");
                skip(e);
                flushENDS(e);
            }
            strcpy(curobj , num2str(-1*num));
            strcpy(curtyp , "INTCON");
            getsym();
        }
		return 0;
	} else if(strcmp(sym , "CHARCON") == 0){
		strcpy(curobj , num2str((int)token[0]));
		strcpy(curtyp , "CHARCON");
		getsym();
		return 0;
	} else if(strcmp(sym , "LPARENT") == 0){
		getsym();
		Expression(e);

		if(strcmp(sym , "RPARENT") != 0){
			error(cc - strlen(token)+1 , "NOT_RPARENT");
			enterENDS(e , "RPARENT");
            skip(e);
            flushENDS(e);
		}
		getsym();
        return 0;
	}/* else {
		error(cc - strlen(token)+1 , "NOT_FACTOR");
		return -1;
	}*/


	if(strcmp(sym , "LBRACK") == 0){
        adr = Position(tmptoken , 1);

        if(adr == -1){
            error(cc-strlen(token)+1 , "NOT_DEF");
            enterENDS(e , "SEMICOLON");
            enterENDS(e , "RPARENT");
            enterENDS(e , "RBRACK");
            enterENDS(e , "RBRACE");
            skip(e);
            flushENDS(e);
            return -1;
        }

		getsym();
		Expression(e);

		if(strcmp(curtyp , "INTSY") != 0 && strcmp(curtyp , "INTCON") != 0){
            error(cc-strlen(token)+1 , "NOT_ARRAYSUB");
            enterENDS(e , "INTSY");
            enterENDS(e , "INTCON");
            skip(e);
            flushENDS(e);
		}

		if(strcmp(sym , "RBRACK") != 0){
			error(cc - strlen(token)+1 , "NOT_RBRACK");
			enterENDS(e , "RBRACK");
            skip(e);
            flushENDS(e);
		}

		strcpy(op , "AVALUE");
        strcpy(v1 , tmptoken);
        strcpy(v2 , curobj);
        strcpy(v3 , nexttmp());
        fillmidcode();

        strcpy(curobj , v3);
        strcpy(curtyp , table.table_elm[adr].typ);

        getsym();

	} else if(strcmp(sym , "LPARENT") == 0){
	    adr = Position(tmptoken , 0);

	    if(adr == -1){
            error(cc-strlen(token)+1 , "NOT_DEF");
            enterENDS(e , "SEMICOLON");
            enterENDS(e , "RPARENT");//----------------------------------->Check
            skip(e);
            flushENDS(e);
            if(strcmp(sym , "RPARENT") == 0) return getsym(); else return -1;
        }

		if(strcmp(table.table_elm[adr].typ , "VOIDSY") == 0 && strcmp(table.table_elm[adr].obj , "FUNCOBJ") == 0){
			error(cc-strlen(token)+1 , "CANT_VOID");
			return -1;
		} else if ((strcmp(table.table_elm[adr].typ , "INTSY") == 0 || strcmp(table.table_elm[adr].typ , "CHARSY") == 0) && strcmp(table.table_elm[adr].obj , "FUNCOBJ") == 0){
			rFuncCall(e,adr);
		}
	} else {
        //标识符
        adr = Position(tmptoken , 1);
        if(adr != -1){
            strcpy(curobj , tmptoken);
            strcpy(curtyp , table.table_elm[adr].typ);
        } else {
            error(cc-strlen(token)+1 , "NOT_DEF");
            enterENDS(e , "SEMICOLON");
            skip(e);
            flushENDS(e);
            return -1;
        }
	}

	return 0;
}

int Item(ENDsym* e){
    fprintf(rF , "This is Item\n");
    char tmpop[10] , tmpv1[10];
	Factor(e);

	while(strcmp(sym , "MULT") == 0 || strcmp(sym , "DIV") == 0){
        strcpy(tmpop ,  sym);
        strcpy(tmpv1 , curobj);

		getsym();
		Factor(e);

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

int Expression(ENDsym* e){
	fprintf(rF , "This is Expression\n");
	char tmpop[10] , tmpv1[10];

	if(strcmp(sym , "PLUS") == 0 || strcmp(sym , "MINU") == 0){
		strcpy(tmpop , sym);
		getsym();
	}

	Item(e);

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
		Item(e);

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

int valuepara(ENDsym* e , int adr){
    fprintf(rF , "This is valuepara\n");
    int i = 0;//值参数个数
	if(strcmp(sym , "RPARENT") == 0){
		return 0;
	}

	Expression(e);

    if(table.table_elm[Position(curobj , 1)].size > 0){
        error(cc-strlen(token)+1 , "CANT_ARRAY");
        enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) {return i;}
    }

    if(i < table.table_elm[adr].para)
        i++;
    else{
        error(cc-strlen(token)+1 , "CANT_MATCH_PARANUM");
        enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) {return i;}
    }

    if(strcmp(curtyp , "INTSY") == 0 || strcmp(curtyp , "INTCON") == 0){
        if(table.table_elm[adr].parastyp.paratyp[i-1] != 0)
            error(cc-strlen(token)+1 , "CANT_MATCH_PARATYP");
    } else if(strcmp(curtyp , "CHARSY") == 0 || strcmp(curtyp , "CHARCON") == 0){
        if(table.table_elm[adr].parastyp.paratyp[i-1] != 1)
            error(cc-strlen(token)+1 , "CANT_MATCH_PARATYP");
    }

    strcpy(op , "VPARA");
    strcpy(v1 , curobj);
    strcpy(v2 , table.table_elm[adr].name);
    strcpy(v3 , "\0");
    fillmidcode();

	while(strcmp(sym , "COMMA") == 0){
		getsym();
		Expression(e);

		if(i < table.table_elm[adr].para)
            i++;
        else{
            error(cc-strlen(token)+1 , "CANT_MATCH_PARANUM");
            enterENDS(e , "RPARENT");
            skip(e);
            flushENDS(e);
            if(strcmp(sym , "RPARENT") == 0) {return i;}
        }

        if(strcmp(curtyp , "INTSY") == 0 || strcmp(curtyp , "INTCON") == 0){
            if(table.table_elm[adr].parastyp.paratyp[i-1] != 0)
                error(cc-strlen(token)+1 , "CANT_MATCH_PARATYP");
        } else if(strcmp(curtyp , "CHARSY") == 0 || strcmp(curtyp , "CHARCON") == 0){
            if(table.table_elm[adr].parastyp.paratyp[i-1] != 1)
                error(cc-strlen(token)+1 , "CANT_MATCH_PARATYP");
        }

        strcpy(op , "VPARA");
        strcpy(v1 , curobj);
        strcpy(v2 , table.table_elm[adr].name);
        strcpy(v3 , "\0");
        fillmidcode();
	}
	return i;
}

int vFuncCall(ENDsym* e , int adr){
    fprintf(rF , "This is vFuncCall\n");
	int paranum = 0;
	if(strcmp(sym , "LPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_LPARENT");
		enterENDS(e , "LPARENT");
        enterENDS(e , "PLUS");
		enterENDS(e , "MINU");
		enterENDS(e , "IDENT");
		enterENDS(e , "INTCON");
		enterENDS(e , "CHARCON");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    if(strcmp(sym , "LPARENT") == 0) getsym();

    paranum = valuepara(e , adr);

    if(paranum != table.table_elm[adr].para){
        error(cc-strlen(token)+1 , "CANT_MATCH_PARANUM");
        enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
    }

	if(strcmp(sym , "RPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_RPARENT");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
	}

    strcpy(op , "CALL");
    strcpy(v1 , table.table_elm[adr].name);
    strcpy(v2 , "\0");
    strcpy(v3 , "\0");
    fillmidcode();

    getsym();
    return 0;
}

int rFuncCall(ENDsym* e , int adr){
    fprintf(rF , "This is rFuncCall\n");
	int paranum = 0;

	if(strcmp(sym , "LPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_LPARENT");
		enterENDS(e , "LPARENT");
        enterENDS(e , "PLUS");
		enterENDS(e , "MINU");
		enterENDS(e , "IDENT");
		enterENDS(e , "INTCON");
		enterENDS(e , "CHARCON");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    if(strcmp(sym , "LPARENT") == 0) getsym();

    paranum = valuepara(e,adr);

    if(paranum != table.table_elm[adr].para){
        error(cc-strlen(token)+1 , "CANT_MATCH_PARANUM");
        enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
    }

	if(strcmp(sym , "RPARENT")!= 0){
		error(cc - strlen(token)+1 , "NOT_RPARENT");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
	}

    strcpy(op , "CALL");
    strcpy(v1 , table.table_elm[adr].name);
    strcpy(v2 , "\0");
    strcpy(v3 , nexttmp());
    fillmidcode();

    strcpy(curobj , v3);
    strcpy(curtyp , table.table_elm[adr].typ);

    getsym();
    return 0;
}

int Condition(ENDsym* e){
    fprintf(rF , "This is Condition\n");
    char tmpop[10] , tmpv1[10];
	Expression(e);

	if(strcmp(sym , "EQL") == 0){
		strcpy(tmpop , sym);
		strcpy(tmpv1 , curobj);

		getsym();
		Expression(e);

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
		Expression(e);

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
		Expression(e);

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
		Expression(e);

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
		Expression(e);

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
		Expression(e);

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

int IfSentence(ENDsym* e){
    fprintf(rF , "This is IfSentence\n");
	char lab1[10] , lab2[10];
	strcpy(lab1 , nextlab());
	strcpy(lab2 , nextlab());

    getsym();

	if(strcmp(sym , "LPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_LPARENT");
		enterENDS(e , "LPARENT");
        enterENDS(e , "PLUS");
		enterENDS(e , "MINU");
		enterENDS(e , "IDENT");
		enterENDS(e , "INTCON");
		enterENDS(e , "CHARCON");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
	}

    if(strcmp(sym , "LPARENT") == 0) getsym();

    if(strcmp(sym , "RPARENT") != 0){
        Condition(e);
        strcpy(op , "JNE");
        strcpy(v1 , curobj);
        strcpy(v2 , lab2);
        strcpy(v3 , "\0");
        fillmidcode();
    }

	if(strcmp(sym , "RPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_RPARENT");
		enterENDS(e , "RPARENT");
		enterENDS(e , "ELSESY");
        skip(e);
        flushENDS(e);
	}

    if(strcmp(sym , "RPARENT") == 0){
        getsym();
        Sentence(e);
        strcpy(op , "JMP");
        strcpy(v1 , "\0");
        strcpy(v2 , lab1);
        strcpy(v3 , "\0");
        fillmidcode();

        strcpy(op , "LAB");
        strcpy(v1 , "\0");
        strcpy(v2 , lab2);
        strcpy(v3 , "\0");
        fillmidcode();
    }

	if(strcmp(sym , "ELSESY") == 0){
		getsym();
		Sentence(e);
	}

    strcpy(op , "LAB");
    strcpy(v1 , "\0");
    strcpy(v2 , lab1);
    strcpy(v3 , "\0");
    fillmidcode();

	return 0;
}

int WhileSentence(ENDsym* e){
    fprintf(rF , "This is WhileSentence\n");
	char lab[10];
	strcpy(lab , nextlab());

    strcpy(op , "LAB");
    strcpy(v1 , "\0");
    strcpy(v2 , lab);
    strcpy(v3 , "\0");
    fillmidcode();

    getsym();
    Sentence(e);

	if(strcmp(sym , "WHILESY") != 0){
		error(cc - strlen(token)+1 , "NOT_WHILESY");
		enterENDS(e , "WHILESY");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    if(strcmp(sym , "WHILESY") == 0) getsym();

	if(strcmp(sym , "LPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_LPARENT");
		enterENDS(e , "LPARENT");
        enterENDS(e , "PLUS");
		enterENDS(e , "MINU");
		enterENDS(e , "IDENT");
		enterENDS(e , "INTCON");
		enterENDS(e , "CHARCON");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    if(strcmp(sym , "LPARENT") == 0) getsym();

    Condition(e);

	if(strcmp(sym , "RPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_RPARENT");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
	}

	strcpy(op , "JEQ");
    strcpy(v1 , curobj);
    strcpy(v2 , lab);
    strcpy(v3 , "\0");
    fillmidcode();

    getsym();
    return 0;
}

int ForSentence(ENDsym* e){
    fprintf(rF , "This is ForSentence\n");
	char tmptoken1[AL];
	char tmptoken2[AL];
	char tmptoken3[AL];
	char tmpop[AL];

	char lab1[10] , lab2[10] , lab3[10] , lab4[10];
	strcpy(lab1 , nextlab());
	strcpy(lab2 , nextlab());
	strcpy(lab3 , nextlab());
	strcpy(lab4 , nextlab());

    getsym();

	if(strcmp(sym , "LPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_LPARENT");
		enterENDS(e , "LPARENT");
		enterENDS(e , "IDENT");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    if(strcmp(sym , "LPARENT") == 0) getsym();

	if(strcmp(sym , "IDENT") != 0){
		error(cc - strlen(token)+1 , "NOT_IDENT");
		enterENDS(e , "IDENT");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    strcpy(tmptoken1 , token);

    if(Position(tmptoken1 , 1) >= 0){
       getsym();
    } else {
        error(cc-strlen(token)+1 , "NOT_DEF");
        enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        return getsym();
    }

	if(strcmp(sym , "BECOMES") != 0){
		error(cc - strlen(token)+1 , "NOT_BECOM");
		enterENDS(e , "BECOMES");
		enterENDS(e , "SEMICOLON");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    if(strcmp(sym , "BECOMES") == 0){
        getsym();
        Expression(e);

        strcpy(op , "BECOM");
        strcpy(v1 , curobj);
        strcpy(v2 , "\0");
        strcpy(v3 , tmptoken1);
        fillmidcode();
    }

	if(strcmp(sym , "SEMICOLON") != 0){
		error(cc - strlen(token)+1 , "NOT_SEMICOLON");
		enterENDS(e , "SEMICOLON");
		enterENDS(e , "PLUS");
		enterENDS(e , "MINU");
		enterENDS(e , "IDENT");
		enterENDS(e , "INTCON");
		enterENDS(e , "CHARCON");
		enterENDS(e , "LPARENT");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    if(strcmp(sym , "SEMICOLON") == 0) getsym();

    strcpy(op , "LAB");
    strcpy(v1 , "\0");
    strcpy(v2 , lab1);
    strcpy(v3 , "\0");
    fillmidcode();
    Condition(e);
    strcpy(op , "JNE");
    strcpy(v1 , curobj);
    strcpy(v2 , lab2);
    strcpy(v3 , "\0");
    fillmidcode();
    strcpy(op , "JMP");
    strcpy(v1 , "\0");
    strcpy(v2 , lab4);
    strcpy(v3 , "\0");
    fillmidcode();

	if(strcmp(sym , "SEMICOLON") != 0){
		error(cc - strlen(token)+1 , "NOT_SEMICOLON");
		enterENDS(e , "SEMICOLON");
		enterENDS(e , "IDENT");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    if(strcmp(sym , "SEMICOLON") == 0) getsym();

    strcpy(op , "LAB");
    strcpy(v1 , "\0");
    strcpy(v2 , lab3);
    strcpy(v3 , "\0");
    fillmidcode();

	if(strcmp(sym , "IDENT") != 0){
		error(cc - strlen(token)+1 , "NOT_IDENT");
		enterENDS(e , "IDENT");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    if(strcmp(sym , "IDENT") == 0){
        strcpy(tmptoken2 , token);
        getsym();
    }

	if(strcmp(sym , "BECOMES") != 0){
		error(cc - strlen(token)+1 , "NOT_BECOM");
		enterENDS(e , "BECOMES");
		enterENDS(e , "IDENT");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    if(strcmp(sym , "BECOMES") == 0) getsym();

	if(strcmp(sym , "IDENT") != 0){
		error(cc - strlen(token)+1 , "NOT_IDENT");
		enterENDS(e , "IDENT");
		enterENDS(e , "PLUS");
		enterENDS(e , "MINU");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    if(strcmp(sym , "IDENT") == 0){
        strcpy(tmptoken3 , token);
        getsym();
    }

	if(strcmp(sym , "PLUS") != 0 && strcmp(sym , "MINU") != 0){
		error(cc - strlen(token)+1 , "NOT_STEP");
		enterENDS(e , "PLUS");
		enterENDS(e , "MINU");
		enterENDS(e , "NUM");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    if(strcmp(sym , "PLUS") == 0 || strcmp(sym , "MINU") == 0){
        strcpy(tmpop , sym);
        getsym();
    }

	if(strcmp(sym , "NUM") != 0){
		error(cc - strlen(token)+1 , "NOT_NUM");
		enterENDS(e , "NUM");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "RPARENT") == 0) return getsym();
	}

    strcpy(v3 , nexttmp());
    if(strcmp(tmpop , "PLUS") == 0){
        strcpy(op , "PLUS");
        strcpy(v1 , tmptoken3);
        strcpy(v2 , token);
        fillmidcode();
        strcpy(curobj , v3);
    } else if (strcmp(tmpop , "MINU") == 0) {
        strcpy(op , "MINU");
        strcpy(v1 , tmptoken3);
        strcpy(v2 , token);
        fillmidcode();

        strcpy(curobj , v3);
    }

    strcpy(op , "BECOM");
    strcpy(v1 , curobj);
    strcpy(v2 , "\0");
    strcpy(v3 , tmptoken2);
    fillmidcode();
    getsym();

	if(strcmp(sym , "RPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_RPARENT");
		enterENDS(e , "RPARENT");
        skip(e);
        flushENDS(e);
	}

    strcpy(op , "JMP");
    strcpy(v1 , "\0");
    strcpy(v2 , lab1);
    strcpy(v3 , "\0");
    fillmidcode();
    getsym();

    strcpy(op , "LAB");
    strcpy(v1 , "\0");
    strcpy(v2 , lab4);
    strcpy(v3 , "\0");
    fillmidcode();
    Sentence(e);

    strcpy(op , "JMP");
    strcpy(v1 , "\0");
    strcpy(v2 , lab3);
    strcpy(v3 , "\0");
    fillmidcode();

    strcpy(op , "LAB");
    strcpy(v1 , "\0");
    strcpy(v2 , lab2);
    strcpy(v3 , "\0");
    fillmidcode();
    return 0;
}

int BecomeSentence(ENDsym* e , char *name){
    fprintf(rF , "This is BecomeSentence\n");
	int adr = Position(name , 1);
	char tmpv2[10];

    if(adr == -1){
        error(cc - strlen(token)+1 , "NOT_DEF");
        enterENDS(e , "SEMICOLON");
        skip(e);
        flushENDS(e);
        return -1;
    }

	if(strcmp(table.table_elm[adr].obj , "CONSTOBJ") == 0){
        error(cc - strlen(token)+1 , "CANT_VALUE_CONST");
        enterENDS(e , "SEMICOLON");
        skip(e);
        flushENDS(e);
        return -1;
	}

    if(strcmp(sym , "LBRACK") != 0 && strcmp(sym , "BECOMES") != 0){
		error(cc - strlen(token)+1 , "NOT_BECOME_SEN");
		enterENDS(e , "LBRACK");
		enterENDS(e , "BECOMES");
		enterENDS(e , "SEMICOLON");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "SEMICOLON") == 0) return -1;
	}

	if(strcmp(sym , "LBRACK") == 0){
		if(table.table_elm[adr].size == 0){
			error(cc - strlen(token)+1 , "CANT_SIZE_ZERO");
			enterENDS(e , "SEMICOLON");
            skip(e);
            flushENDS(e);
			return -1;
		}

		getsym();
		Expression(e);//如果不是整数

        strcpy(tmpv2 , curobj);

        if(isNum(tmpv2) == 1 && atoi(tmpv2) >= table.table_elm[adr].size){
            error(cc - strlen(token)+1 , "OVER_ARRAY");
			enterENDS(e , "SEMICOLON");
            skip(e);
            flushENDS(e);
            if(strcmp(sym , "SEMICOLON") == 0) return -1;
        }

		if(strcmp(sym , "RBRACK") != 0){
			error(cc - strlen(token)+1 , "NOT_RBRACK");
			enterENDS(e , "RBRACK");
			enterENDS(e , "BECOMES");
			enterENDS(e , "SEMICOLON");
            skip(e);
            flushENDS(e);
            if(strcmp(sym , "SEMICOLON") == 0) return -1;
		}

        if(strcmp(sym , "RBRACK") == 0) getsym();

		if(strcmp(sym , "BECOMES") != 0){
			error(cc - strlen(token)+1 , "NOT_BECOM");
			enterENDS(e , "BECOMES");
			enterENDS(e , "SEMICOLON");
            skip(e);
            flushENDS(e);
            if(strcmp(sym , "SEMICOLON") == 0) return -1;
		}

        getsym();
		Expression(e);

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
			error(cc - strlen(token)+1 , "CANT_SIZE_NZERO");
			enterENDS(e , "SEMICOLON");
            skip(e);
            flushENDS(e);
			if(strcmp(sym , "SEMICOLON") == 0) return -1;
		}

		getsym();
		Expression(e);

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
	}

	return -1;
}

int ScanfSentence(ENDsym* e){
    int addr;

    fprintf(rF , "This is ScanfSentence\n");
    getsym();
	if(strcmp(sym , "LPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_LPARENT");
		enterENDS(e , "LPARENT");
		enterENDS(e , "IDENT");
        enterENDS(e , "SEMICOLON");
        enterENDS(e , "COMMA");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "SEMICOLON") == 0) return -1;
	}

    if(strcmp(sym , "LPARENT") == 0 || strcmp(sym , "COMMA") == 0) getsym();

	if(strcmp(sym , "IDENT") != 0){
		error(cc - strlen(token)+1 , "NOT_IDENT");
		enterENDS(e , "IDENT");
        skip(e);
        flushENDS(e);
	}

    addr = Position(token , 2);

	if(addr == -1 || table.table_elm[addr].size > 0){
        error(cc - strlen(token)+1 , "CANT_SCANF");
        enterENDS(e , "COMMA");
        enterENDS(e , "RPARENT");
        enterENDS(e , "SEMICOLON");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "SEMICOLON") == 0) return -1;
    } else {
        strcpy(op , "SCANF");
        strcpy(v1 , "\0");
        strcpy(v2 , token);
        strcpy(v3 , "\0");
        fillmidcode();
        getsym();
    }

	while(strcmp(sym , "COMMA") == 0){
		getsym();

		if(strcmp(sym , "IDENT") != 0){
			error(cc - strlen(token)+1 , "NOT_IDENT");
			enterENDS(e , "IDENT");
            skip(e);
            flushENDS(e);
		}

        addr = Position(token , 2);

        if(addr == -1 || table.table_elm[addr].size > 0){
            error(cc - strlen(token)+1 , "CANT_SCANF");
			enterENDS(e , "COMMA");
			enterENDS(e , "RPARENT");
			enterENDS(e , "SEMICOLON");
            skip(e);
            flushENDS(e);
            continue;
        }

        strcpy(op , "SCANF");
        strcpy(v1 , "\0");
        strcpy(v2 , token);
        strcpy(v3 , "\0");
        fillmidcode();
        getsym();

	}

	if(strcmp(sym , "RPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_RPARENT");
		enterENDS(e , "RPARENT");
		enterENDS(e , "SEMICOLON");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "SEMICOLON") == 0) return -1;
	}

    getsym();
    return 0;
}

int PrintfSentence(ENDsym* e){
    fprintf(rF , "This is PrintfSentence\n");
	char tmpstr[AL];
	int flag = 0;

	if(strcmp(sym , "PRINTFSY") != 0){
		error(cc - strlen(token)+1 , "NOT_PRINTFSY");
		enterENDS(e , "PRINTFSY");
        skip(e);
        flushENDS(e);
	}

    getsym();

	if(strcmp(sym , "LPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_LPARENT");
		enterENDS(e , "LPARENT");
		enterENDS(e , "STRCON");
		enterENDS(e , "PLUS");
		enterENDS(e , "MINU");
		enterENDS(e , "IDENT");
		enterENDS(e , "CHARCON");
		enterENDS(e , "NUM");
		enterENDS(e , "ZERO");
		enterENDS(e , "PLUS");
		enterENDS(e , "SEMICOLON");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "SEMICOLON") == 0) return -1;
	}

    if(strcmp(sym , "LPARENT") == 0) getsym();

	if(strcmp(sym , "STRCON") == 0){
		strcpy(tmpstr , token);
		getsym();

		if(strcmp(sym , "COMMA") == 0){
            getsym();
			Expression(e);
			flag = 1;
		}
	} else if (strcmp(sym , "PLUS") == 0 || strcmp(sym , "MINU") == 0 || strcmp(sym , "IDENT") == 0 || strcmp(sym , "CHARCON") == 0 || strcmp(sym , "NUM") == 0 || strcmp(sym , "ZERO") == 0 || strcmp(sym , "LPARENT") == 0){
		Expression(e);
		flag = 2;
	} else {
		error(cc - strlen(token)+1 , "CANT_PRINTF");
		enterENDS(e , "SEMICOLON");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "SEMICOLON") == 0) return -1;
	}

	if(strcmp(sym , "RPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_RPARENT");
		enterENDS(e , "RPARENT");
		enterENDS(e , "SEMICOLON");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "SEMICOLON") == 0) return -1;
	}

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
}

int ReturnSentence(ENDsym* e){
    fprintf(rF , "This is ReturnSentence\n");
	if(strcmp(sym , "RETURNSY") != 0){
		error(cc - strlen(token)+1 , "NOT_RETURNSY");
		enterENDS(e , "RETURNSY");
        skip(e);
        flushENDS(e);
	}

    getsym();

	if(strcmp(sym , "LPARENT") == 0){
		getsym();
	} else if(strcmp(sym , "SEMICOLON") == 0){
	    strcpy(op , "RET");
        strcpy(v1 , "\0");
        strcpy(v2 , "\0");
        strcpy(v3 , "\0");
        fillmidcode();
		return 0;
	} else {
        error(cc-strlen(token)+1 , "NOT_LPARENT");
        enterENDS(e , "SEMICOLON");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "SEMICOLON") == 0) return -1;
	}

	Expression(e);

/*
    if(strcmp(curtyp , "INTSY") == 0 || strcmp(curtyp , "INTCON") == 0){
        if(strcmp(table.table_elm[table.proc_index[table.proc_num-1]].typ , "INTSY") != 0)
            error(cc - strlen(token) , 24);
    } else if(strcmp(curtyp , "CHARSY") == 0 || strcmp(curtyp , "CHARCON") == 0){
        if(strcmp(table.table_elm[table.proc_index[table.proc_num-1]].typ , "CHARSY") != 0)
            error(cc - strlen(token) , 24);
    }*/

    table.table_elm[table.proc_index[table.proc_num-1]].parastyp.returnnum++;

	if(strcmp(sym , "RPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_RPARENT");
		enterENDS(e , "RPARENT");
		enterENDS(e , "SEMICOLON");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "SEMICOLON") == 0) return -1;
	}

    strcpy(op , "RET");
    strcpy(v1 , "\0");
    strcpy(v2 , curobj);
    strcpy(v3 , "\0");
    fillmidcode();
    getsym();
    return 0;
}

int Sentence(ENDsym* e){
    fprintf(rF , "This is Sentence\n");
	char tmptoken[AL];
	int adr;

	if (strcmp(sym , "IFSY") == 0) {
        IfSentence(e);
        return 0;
    } else if (strcmp(sym , "DOSY") == 0) {
        WhileSentence(e);
        return 0;
    } else if (strcmp(sym , "FORSY") == 0) {
        ForSentence(e);
        return 0;
    } else if(strcmp(sym , "IDENT") == 0) {
    	strcpy(tmptoken , token);
    	getsym();

    	if(strcmp(sym , "BECOMES") == 0 || strcmp(sym , "LBRACK") == 0){
    		BecomeSentence(e,tmptoken);
		} else if(strcmp(sym , "LPARENT") == 0){
		    adr = Position(tmptoken , 0);
		    if(adr == -1) {
                error(cc - strlen(token)+1 , "NOT_DEF");
                enterENDS(e , "SEMICOLON");
                skip(e);
                flushENDS(e);
                if(strcmp(sym , "SEMICOLON") == 0) return getsym();
		    }
			if(strcmp(table.table_elm[adr].typ , "VOIDSY") == 0 && strcmp(table.table_elm[adr].obj , "FUNCOBJ") == 0){
				vFuncCall(e,adr);
			} else if ((strcmp(table.table_elm[adr].typ , "INTSY") == 0 || strcmp(table.table_elm[adr].typ , "CHARSY") == 0) && strcmp(table.table_elm[adr].obj , "FUNCOBJ") == 0){
				rFuncCall(e,adr);
			} else {
				error(cc - strlen(token)+1 , "CANT_CALL_VOID");
				enterENDS(e , "SEMICOLON");
                skip(e);
                flushENDS(e);
				if(strcmp(sym , "SEMICOLON") == 0) return getsym();
			}
		} else {
			error(cc - strlen(token)+1 , "NOT_SENTENCE");
			enterENDS(e , "SEMICOLON");
            skip(e);
            flushENDS(e);
			if(strcmp(sym , "SEMICOLON") == 0) return getsym();
		}
	} else if(strcmp(sym , "SCANFSY") == 0){
		ScanfSentence(e);
	} else if(strcmp(sym , "PRINTFSY") == 0){
		PrintfSentence(e);
	} else if(strcmp(sym , "RETURNSY") == 0){
		ReturnSentence(e);
	} else if (strcmp(sym , "LBRACE") == 0) {
        getsym();
        Sentences(e);
        if (strcmp(sym , "RBRACE") != 0){
         	error(cc - strlen(token)+1 , "NOT_RBRACE");
         	enterENDS(e , "RBRACE");
            skip(e);
            flushENDS(e);
		}
		getsym();
        return 0;
    }

    if(strcmp(sym , "SEMICOLON") != 0){
		error(cc - strlen(token)+1 , "NOT_SEMICOLON");
		enterENDS(e , "SEMICOLON");
		enterENDS(e , "IFSY");
		enterENDS(e , "DOSY");
		enterENDS(e , "FORSY");
		enterENDS(e , "LBRACE");
		enterENDS(e , "RBRACE");
		enterENDS(e , "IDENT");
		enterENDS(e , "SCANFSY");
		enterENDS(e , "PRINTFSY");
		enterENDS(e , "RETURNSY");
        skip(e);
        flushENDS(e);
        if(strcmp(sym , "SEMICOLON") == 0) return getsym();
	}
    getsym();
    return 0;
}

int Sentences(ENDsym* e){
    fprintf(rF , "This is Sentences\n");
	while(strcmp(sym , "IFSY") == 0 || strcmp(sym , "DOSY") == 0 || strcmp(sym , "FORSY") == 0 || strcmp(sym , "LBRACE") == 0 || strcmp(sym , "IDENT") == 0 || strcmp(sym , "SCANFSY") == 0 || strcmp(sym , "PRINTFSY") == 0 || strcmp(sym ,"RETURNSY" ) == 0 || strcmp(sym ,"SEMICOLON" ) == 0){
		Sentence(e);
	}

	return 0;
}

int ComplexSentence(ENDsym* e){
    fprintf(rF , "This is ComplexSentence\n");
	if(strcmp(sym , "CONSTSY") == 0){
		ConstSta(e);
	}

	if( strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0){
		VarSta(e);
	}

	while(strcmp(sym , "CONSTSY") == 0){
		error(cc-strlen(token)+1 , "CANT_CONSTSTA");//TODO 跳读到int或char
        enterENDS(e , "SEMICOLON");
		skip(e);
        flushENDS(e);
		if(strcmp(sym , "SEMICOLON") == 0) return getsym();
	}

	Sentences(e);
//CHECK!!!!!!!!!!!!!!!!!!!!!!!
	if(strcmp(sym , "RBRACE") != 0){
		getsym();
	}
	return 0;
}

int rFuncDef(ENDsym* e){
    fprintf(rF , "This is rFuncDef\n");
	char typ[AL];
	int tabadr = 0;
	char obj[] = "FUNCOBJ";
	value v;

	if(strcmp(sym , "INTSY") != 0 && strcmp(sym , "CHARSY") != 0){
		error(cc - strlen(token)+1 , "NOT_TYPESY");
		enterENDS(e , "INTSY");
		enterENDS(e , "CHARSY");
		skip(e);
        flushENDS(e);
	}

    if(strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0){
        strcpy(typ , sym);
        getsym();
    }

	if(strcmp(sym , "IDENT") != 0){
		error(cc - strlen(token)+1 , "NOT_IDENT");
		enterENDS(e , "IDENT");
		skip(e);
        flushENDS(e);
	}

    if(strcmp(sym , "IDENT") == 0){
        tabadr = enter(token, obj, typ, v, 0, 0, 0);//TODO 插入失败输出错误程序结束
        table.table_elm[tabadr].parastyp.returnnum = 0;
        getsym();
    }

	if(strcmp(sym , "LPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_LPARENT");
		enterENDS(e , "LPARENT");
		skip(e);
        flushENDS(e);
	}

    if(strcmp(sym , "LPARENT") == 0){
        int tmpc = mpc;
        strcpy(op , obj);
        strcpy(v1 , typ);
        strcpy(v2 , table.table_elm[tabadr].name);
        strcpy(v3 , "\0");
        fillmidcode();

        getsym();
        Parameters(e,tabadr);
        strcpy(midcode[tmpc].v3,num2str(table.table_elm[tabadr].para));
    }

	if(strcmp(sym , "RPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_RPARENT");
		enterENDS(e , "RPARENT");
		skip(e);
        flushENDS(e);
	}

    if(strcmp(sym , "RPARENT") == 0) getsym();

	if(strcmp(sym , "LBRACE") != 0){
		error(cc - strlen(token)+1 , "NOT_LBRACE");
		enterENDS(e , "LBRACE");
		skip(e);
        flushENDS(e);
	}

    if(strcmp(sym , "LBRACE") == 0){
        getsym();
        ComplexSentence(e);
    }

	if(strcmp(sym , "RBRACE") != 0){
		error(cc - strlen(token)+1 , "NOT_RBRACE");
		enterENDS(e , "RBRACE");
		skip(e);
        flushENDS(e);
	}

    if(table.table_elm[table.proc_index[table.proc_num-1]].parastyp.returnnum == 0){
        error(cc - strlen(token)+1 , "CANT_RETNUM_ZERO");
        Flush();
        getsym();
        return -1;
    }

    strcpy(op , "END");
    strcpy(v1 , "\0");
    strcpy(v2 , table.table_elm[tabadr].name);
    strcpy(v3 , "\0");
    fillmidcode();
    Flush();

    getsym();
    return 0;
}

int vFuncDef(ENDsym* e){
    fprintf(rF , "This is vFuncDef\n");
	int tabadr = 0;
	char obj[] = "FUNCOBJ";
	char typ[] = "VOIDSY";
	value v;

	if(strcmp(sym , "IDENT") != 0){
		error(cc - strlen(token)+1 , "NOT_IDENT");
		enterENDS(e , "IDENT");
		skip(e);
        flushENDS(e);
	}

    if(strcmp(sym , "IDENT") == 0){
        tabadr = enter(token, obj, typ, v, 0, 0, 0);//TODO 插入失败输出错误程序结束
        table.table_elm[tabadr].parastyp.returnnum = 0;
        getsym();
    }

	if(strcmp(sym , "LPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_LPARENT");
		enterENDS(e , "LPARENT");
		skip(e);
        flushENDS(e);
	}

    if(strcmp(sym , "LPARENT") == 0){
        int tmpc = mpc;
        strcpy(op , obj);
        strcpy(v1 , typ);
        strcpy(v2 , table.table_elm[tabadr].name);
        strcpy(v3 , "\0");
        fillmidcode();

        getsym();
        Parameters(e,tabadr);
        strcpy(midcode[tmpc].v3,num2str(table.table_elm[tabadr].para));
    }

	if(strcmp(sym , "RPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_RPARENT");
		enterENDS(e , "RPARENT");
		skip(e);
        flushENDS(e);
	}

    if(strcmp(sym , "RPARENT") == 0) getsym();

	if(strcmp(sym , "LBRACE") != 0){
		error(cc - strlen(token)+1 , "NOT_LBRACE");
		enterENDS(e , "LBRACE");
		skip(e);
        flushENDS(e);
	}

    if(strcmp(sym , "LBRACE") == 0){
        getsym();
        ComplexSentence(e);
    }

	if(strcmp(sym , "RBRACE") != 0) {
		error(cc - strlen(token)+1 , "NOT_RBRACE");
		enterENDS(e , "RBRACE");
		skip(e);
        flushENDS(e);
	}

    if(table.table_elm[table.proc_index[table.proc_num-1]].parastyp.returnnum != 0){
        error(cc - strlen(token)+1 , "CANT_RETNUM_NZERO");
        Flush();
        getsym();
        return -1;
    }

    strcpy(op , "END");
    strcpy(v1 , "\0");
    strcpy(v2 , table.table_elm[tabadr].name);
    strcpy(v3 , "\0");
    fillmidcode();
    Flush();

    getsym();
    return 0;
}

int Procedure(ENDsym* e){
    fprintf(rF , "This is Procedure\n");
	int tmpcc;
	int tmpln;

	getsym();

	if(strcmp(sym , "CONSTSY") == 0){
		ConstSta(e);
	}

	if( strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0){
		VarSta(e);
	}

	while(strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0 || strcmp(sym , "VOIDSY") == 0){
		tmpcc = cc - strlen(token) - 1;
		tmpln = ln;

		if(strcmp(sym , "INTSY") == 0 || strcmp(sym , "CHARSY") == 0){
			rFuncDef(e);
		} else {
			getsym();
			if (strcmp(sym , "IDENT") == 0) {
				vFuncDef(e);
			} else {
				skipback(tmpcc , tmpln);
				getsym();
				break;
			}
		}
		skipconst(e);
	}

	if(strcmp(sym , "VOIDSY") != 0){
		error(cc - strlen(token)+1 , "NOT_VOIDSY");

		enterENDS(e , "VOIDSY");

		skip(e);

		flushENDS(e);
	}

    getsym();

	if(strcmp(sym , "MAINSY") != 0){
		error(cc - strlen(token)+1 , "NOT_MAINSY");

		enterENDS(e , "MAINSY");

		skip(e);

		flushENDS(e);
	}

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

	if(strcmp(sym , "LPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_LPARENT");

        enterENDS(e , "LPARENT");

		skip(e);

		flushENDS(e);
	}

    getsym();

	if(strcmp(sym , "RPARENT") != 0){
		error(cc - strlen(token)+1 , "NOT_RPARENT");

        enterENDS(e , "RPARENT");

		skip(e);

		flushENDS(e);
	}

    getsym();

	if(strcmp(sym , "LBRACE") != 0){
		error(cc - strlen(token)+1 , "NOT_LBRACE");

        enterENDS(e , "LBRACE");

		skip(e);

		flushENDS(e);
	}

    getsym();
    ComplexSentence(e);

    if(table.table_elm[table.proc_index[table.proc_num-1]].parastyp.returnnum != 0){
        error(cc - strlen(token)+1 , "CANT_RETNUM_NZERO");
    }

	if(strcmp(sym , "RBRACE") != 0){
		error(cc - strlen(token)+1 , "NOT_RBRACE");

        enterENDS(e , "RBRACE");

		skip(e);

		flushENDS(e);
	}

    strcpy(op , "END");
    strcpy(v1 , "\0");
    strcpy(v2 , "\0");
    strcpy(v3 , "main");
    fillmidcode();
    Flush();
    free_tmpreg();

    getsym();
    return 0;
}

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

int pushstack(char* value){
    rsT << "\tli\t$t0\t" << value << "\t#" << midcode[tpc].v2 << "=" << midcode[tpc].v3 << endl;
    rsT << "\tsw\t$t0\t($sp)" << endl;
    rsT << "\tsubi\t$sp\t$sp\t" << 4 << endl;
    sp -= 4;
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

void JMPasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    rsT << "\tj\t" << midcode[ tpc ].v2 << endl;
    tpc++;
}

void JEQasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    rsT << "\tbne\t$t0\t0\t" << midcode[ tpc ].v2 << endl;
    tpc++;
}

void JNEasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    rsT << "\tbne\t$t0\t1\t" << midcode[ tpc ].v2 << endl;
    tpc++;
}

void LABasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    rsT << midcode[ tpc ].v2 << ":"<<endl;
    tpc++;
}

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
        rsT << "\tsw\t$t0\t" << addr3 << "($t9)" << endl;
    else
        rsT << "\tsw\t$t0\t" << addr3 << "($fp)" << endl;
    tpc++;
}

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

void VPARAasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    if ( isNum(midcode[tpc].v1) || isChar(midcode[tpc].v1)) {
        rsT << "\tli\t$t0\t" << midcode[tpc].v1 << endl;
    } else {
        rsT << "\tlw\t$t0\t" << varaddr(midcode[tpc].v1); //li    $t0 item
        if(isglob){
            rsT << "($t9)" << endl;
        }else{
            rsT << "($fp)" << endl;
        }
    }
    rsT << "\tsw\t$t0\t($sp)" << endl;      //sw    $t0 $sp
    sp -= 4;
    rsT << "\tsubi\t$sp\t$sp\t4" << endl;
    tpc++;
}

void CALLasm() {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    rsT << "\tjal\t" << midcode[ tpc ].v1 << endl;
//    rsT << "\tnop\n";
    if (midcode[ tpc ].v3[ 0 ] != '\0' ) {
        int addr2 = varaddr(midcode[ tpc ].v3);
        if ( isglob )
            rsT << "\tsw\t$v0\t" << addr2 << "($t9)" << endl;
        else
            rsT << "\tsw\t$v0\t" << addr2 << "($fp)" << endl;
    }
    tpc++;
}

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

void PARAasm(int fparanum) {
    rsT << "#" << midcode[ tpc ].op <<"\t"<<midcode[ tpc ].v1<<"\t"<<midcode[ tpc ].v2<<"\t"<<midcode[ tpc ].v3<<endl;
    rsT << "\taddi\t$t0\t$fp\t" << (fparanum-atoi(midcode[ tpc ].v3)+1)*4 << endl;
    rsT << "\tlw\t$t1\t($t0)" << endl;
    rsT << "\tsw\t$t1\t($sp)" << endl;
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

    int paranum = atoi(midcode[tpc-1].v3);
    int funcaddr = ax - 1;
    addrtable[funcaddr].paranum = paranum;
    while(strcmp(midcode[tpc].op , "PARA") == 0){
        PARAasm(paranum);
        insertadrtab(midcode[tpc].v2 , sp , midcode[tpc].v1 , 0);
        pushstack(1);
        tpc++;
    }

    ptmpreg = 2;

    while(strcmp(midcode[tpc].op , "CONSTOBJ") == 0){
        if(strcmp(midcode[tpc].v1 , "INTSY") == 0){
            insertadrtab(midcode[tpc].v2 , sp , midcode[tpc].v1 , 0);
            pushstack(midcode[tpc].v3);
        } else if(strcmp(midcode[tpc].v1 , "CHARSY") == 0){
            insertadrtab(midcode[tpc].v2 , sp , midcode[tpc].v1 , 0);
            pushstack(num2str((int)midcode[tpc].v3[0]));
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

    rsT << "\taddi\t$sp\t$sp\t" << paranum*4 << endl;

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

//    char FN[50];

//    cin >> FN;

    table.tx = 0;
    table.proc_num = 0;

	if((FP = fopen(FN,"r")) == NULL)
	{
		perror("fail to open");
		exit (1) ;
	}

    if((rF = fopen(RESULTFILE,"w")) == NULL)
	{
		perror("fail to open");
		exit (1) ;
	}

    rsT.open(OUTFILE);

	ch = ' ';
	ENDsym* endsymbol = (ENDsym*)malloc(sizeof(ENDsym));

    endsymbol->ep = 0;

    Procedure(endsymbol);
//    if(strcmp(sym , "NUL") != 0) {error(cc-strlen(token)+1 , "CANT_NOT_OVER");}
    if(errnum == 0) interpret();

	fclose(FP);
	fclose(rF);

    free(endsymbol);

	miD.open("mid_code.txt");

	int testmid = 0;
	for(testmid = 0;testmid < mpc ; testmid++){
        miD << midcode[testmid].op << "\t" << midcode[testmid].v1 << "\t" << midcode[testmid].v2 << "\t" << midcode[testmid].v3 << endl;
	}

    rsT.close();
    miD.close();
	return 0;
}
