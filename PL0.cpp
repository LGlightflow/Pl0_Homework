//A.2     C      版   本

/*编译和运行环境:
*1Visual C++6.0,VisualC++.NET and Visual C++.NET 2003
*WinNT, Win 200, WinXP and  Win2003 
*2 gcc version 3.3.2  20031022(Red Hat Linux 3.3.2-1)
*Redhat Fedora core 1
*Intel 32 platform
*使用方法:
*运行后输入PL/0 源程序文件名
*回答是否输出虚拟机代码
*回答是否输出名字表
*fa.tmp 输出虚拟机代码
*fa1.tmp  输出源文件及其各行对应的首地址
*fa2.tmp  输出结果 
*fas.tmp  输出名字表
*/
#include<stdio.h>
#include<stdlib.h>
#include"pl0.h"
#include"string.h"

/*解释执行时使用的栈*/
#define stacksize 500
int main()
{
	bool nxtlev[symnum];
	printf("Input pl/0 file ?");
	scanf("%s",fname);                                     /*输入文件名*/
	fin=fopen(fname,"r");
	if(fin)
	{
		printf("List object code ?(Y/N)");                /*是否输出虚拟机代码*/
		scanf("%s",fname);
		listswitch=(fname[0]=='y'||fname[0]=='Y');
		printf("List symbol table ? (Y/N)");             /*是否输出名字表*/
		scanf("%s",fname);
		tableswitch=(fname[0]=='y'||fname[0]=='Y');
		fa1=fopen("fa1.tmp","w");
		fprintf(fa1,"Iput pl/0 file ?");
		fprintf(fa1,"%s\n", fname);
		init();                                          /*初始化*/
		err=0;
		cc=cx=ll=0;
		ch=' ';
		if(-1!=getsym())
		{
			fa=fopen("fa.tmp","w");
			fas=fopen("fas.tmp","w");
			addset(nxtlev,declbegsys,statbegsys,symnum);
			nxtlev[period]=true;    
			if(-1==block(0,0,nxtlev))			/*调用编译程序*/
			{
				fclose(fa);
				fclose(fa1);
				fclose(fas);
				fclose(fin);
				printf("\n");
				return 0;
			}
			fclose(fa);
			fclose(fa1);
			fclose(fas);
			if(sym!=period)
			{
				error(9);
			}
			if(err==0)
			{
				fa2=fopen("fa2.tmp", "w");
				interpret1();
				fclose(fa2);
			}
			else
			{
				printf("Errors in pl/0 program");
			}
		}
		fclose(fin);
	}
	else
	{
		printf("Can't open file! \n");
	}
	printf("\n");
    system ("pause");
	return 0;
}
/*
*初始化
*/
void init()
{
	int i;
	
	// 初始化字符符号
	for(i=0;i<=255;i++)
	{
		ssym[i]=nul;
	}

	//为特定字符如+, -, *等设置对应的符号，符号将用于词法分析阶段
	ssym['+']=plus;
	ssym['-']=minus;
	ssym['*']=times;
	ssym['/']=slash;
	ssym['(']=lparen;
	ssym[')']=rparen;
	ssym['=']=eql;
	ssym[',']=comma;
	ssym['.']=period;
	//ssym['#']=neq;
	ssym[';']=semicolon;
    ssym['!'] = not1;  //新增

        //设置保留字和符号，在语法分析阶段识别关键字
	/*设置保留字名字,按照字母顺序,便于折半查找*/
	strcpy(&(word[0][0]),"begin");
	strcpy(&(word[1][0]),"call");
	strcpy(&(word[2][0]),"char");//新增 字符型char
	strcpy(&(word[3][0]),"const");
	strcpy(&(word[4][0]),"do");	//新增do
	strcpy(&(word[5][0]),"downto"); //新增downto
	strcpy(&(word[6][0]),"else"); //新增
	strcpy(&(word[7][0]),"end");
	strcpy(&(word[8][0]),"for");
	strcpy(&(word[9][0]),"if");
	strcpy(&(word[10][0]),"int");
	strcpy(&(word[11][0]),"odd");
	strcpy(&(word[12][0]),"procedure");
	strcpy(&(word[13][0]),"read");
	strcpy(&(word[14][0]),"then");
	strcpy(&(word[15][0]),"to"); //新增to
	strcpy(&(word[16][0]),"var");
	strcpy(&(word[17][0]),"while");
	strcpy(&(word[18][0]),"write");
	//改
	/*设置保留字符号*/
	wsym[0]=beginsym;
	wsym[1]=callsym;
	wsym[2]=charsym;  //新增 字符型char
	wsym[3]=constsym;
	wsym[4]=dosym;
	wsym[5]=downtosym;
	wsym[6]=elsesym;	//新增
	wsym[7]=endsym;
	wsym[8]=forsym;
	wsym[9]=ifsym;
	wsym[10]=intsym;	//新增int类型
	wsym[11]=oddsym;
	wsym[12]=procsym;
	wsym[13]=readsym;
	wsym[14]=thensym;
	wsym[15]=tosym;
	wsym[16]=varsym;
	wsym[17]=whilesym;
	wsym[18]=writesym;

	//为虚拟机指令集设置名称，这些名称将在生成中间代码或目标代码时使用
	/*设置指令名称*/
	strcpy(&(mnemonic[lit][0]),"lit");
	strcpy(&(mnemonic[opr][0]),"opr");
	strcpy(&(mnemonic[lod][0]),"lod");
	strcpy(&(mnemonic[sto][0]),"sto");
	strcpy(&(mnemonic[cal][0]),"cal");
	strcpy(&(mnemonic[inte][0]),"int");
	strcpy(&(mnemonic[jmp][0]),"jmp");
	strcpy(&(mnemonic[jpc][0]),"jpc");

	//初始化声明、语句和因子开始符号集，确保所有符号集都被正确初始化为false
	/*设置符号集*/
	for(i=0;i<symnum;i++)
	{
		declbegsys[i]=false;
		statbegsys[i]=false;
		facbegsys[i]=false;
	}

	// 设置声明、语句和因子开始符号集，以便在语法分析时正确识别程序结构
	/*设置声明开始符号集*/
	declbegsys[constsym]=true;
	declbegsys[varsym]=true;
	declbegsys[procsym]=true;
	declbegsys[charsym]=true;//新增 字符型 charsym
	declbegsys[intsym]=true;//新增int类型
	/*设置语句开始符号集*/
	statbegsys[beginsym]=true;
	statbegsys[callsym]=true;
	statbegsys[ifsym]=true;
	statbegsys[whilesym]=true;
	/*设置因子开始符号集*/
	facbegsys[ident]=true;
	facbegsys[number]=true;
	facbegsys[lparen]=true;
	facbegsys[charsym]=true;//新增 字符型 charsym
	facbegsys[intsym]=true;
}

 /*
  *用数组实现集合的集合运算：
  在语法分析阶段，编译器需要确定哪些符号可以合法地出现在特定位置
  		（声明符号（如 const、var、procedure 等）
		语句符号（如 begin、if、while、call 等）
		因子符号（如标识符、数字、左括号等））
	通过使用集合运算，可以方便地管理和检查这些符号。
	例如，判断一个符号是否在某个集合中（inset 函数），合并多个符号集合（addset 函数），或求两个集合的交集（mulset 函数）

	语义分析
		需要检查程序的语义正确性，如类型检查、作用域检查、控制流分析等
			检查变量是否在声明的范围内使用。
			确定控制流结构（如 if 语句、while 循环）的合法性。
			处理作用域规则，确保变量和函数在正确的作用域内声明和使用。
  */

// 判断元素 e 是否在集合 s 中
int inset(int e,bool* s)
{
    return s[e];
}
// 实现集合的并（或）运算
int addset(bool* sr,bool* s1,bool* s2,int n)
{
    int i;
    for(i=0;i<n;i++)
    {
       sr[i]=s1[i]||s2[i];  
    }
    return 0;
}
//实现集合的差运算
int subset(bool* sr,bool* s1,bool* s2,int n)
{
    int i;
    for(i=0;i<n;i++)
    {
       sr[i]=s1[i]&&(!s2[i]);
    }
    return 0; 
}
//实现集合的交运算
int mulset(bool* sr,bool* s1,bool* s2,int n)
{
    int i;
    for(i=0;i<n;i++)
    {
       sr[i]=s1[i]&&s2[i];  
    } 
    return 0;
}
/*
 *出错处理，打印出错位置和错误编码
 */
void error(int n)
 
{
	char space[81];
	memset(space,32,81); //32对应空格字符 将 space 数组的前 81 个字节全部设置为空格字符
	printf("-------%c\n",ch);  // 打印当前字符，帮助定位错误位置
	space[cc-1]=0;//出错时当前符号已经读完，所以cc-1
	printf("****%s!%d\n",space,n);
	err++;
}
/*
 *  漏掉空格，读取一个字符
 *
 *  每次读一行，存入line缓冲区，line被getsym取空后再读一行
 *
 *  被函数getsym调用
 */
int getch()
{
	// 如果已经读取到当前行的末尾，则读入下一行
	if(cc==ll)
    {
		// 如果文件结束，则输出程序不完整的信息
		if(feof(fin))
		{
			printf("program incomplete");
			return -1;
		}

		ll=0;
		cc=0;

		printf("%d ",cx );
		fprintf(fa1,"%d ",cx);
		ch=' ';
		while(ch!=10)  //是否读到换行符
		{
			//fscanf(fin,"%c",&ch)
			// 读取一个字符，如果读取到文件结束符则跳出循环
            if(EOF==fscanf(fin,"%c",&ch))
			{
				line[ll]=0; // 当前行长度置0，结束当前行
				break;
			}
			printf("%c",ch);
			fprintf(fa1,"%c",ch);
			line[ll]=ch;
			ll++;
		}
		printf("\n");
		fprintf(fa1,"\n");
	}
	 // 从行缓冲区读取一个字符
	ch=line[cc];
	cc++;
	return 0;
}
 /*词法分析，获取一个符号（单词）
 */

int getsym()
{
	int i,j,k;
	 // 跳过空格、换行和制表符
	while( ch==' '||ch==10||ch==9|| ch==13)// 状态1   //13是tab
	{	
		getchdo;
	} 
	// 处理以字母开头的标识符
	if(ch>='a'&&ch<='z')
	{ 
		//状态2
		k=0;
		do{
			if(k<al)
			{ 
				a[k]=ch;
				k++;
			}
			getchdo; 
		}while(ch>='a'&&ch<='z'||ch>='0'&&ch<='9'); // 保留字或标识符
		// 状态3
		a[k]=0;
		strcpy(id,a);
		// 二分查找，确定标识符是否为保留字
		i=0;
		j=norw-1;
		do{
			k=(i+j)/2;
			if(strcmp(id,word[k])<=0)
			{
				j=k-1;
			}
			if(strcmp(id,word[k])>=0)
			{
				i=k+1;
			}
		}while(i<=j);
		if(i-1>j)
		{
			sym=wsym[k]; //保留字
            //printf("识别出保留字\n");
        }
		else
		{
			sym=ident; //标识符
            //printf("识别出标识符\n");
		}
	}
	else// 处理数字
	{	
		if(ch>='0'&&ch<='9') 
		{//状态4
			k=0;
			num=0;
			sym=number;
			/*假如识别数字12，第一个读'1',num= 10*0+1;第二读2，num=1*10+2 1*10代表进位
			* 即一个识别整数的过程
			*/
			do{
				num=10*num+ch-'0';
				k++;
				getchdo;
			}while(ch>='0'&&ch<='9'); /*获取数字的值*/   //状态5
			k--;
			if(k>nmax)
			{
				error(30); //数字过长
			}
		}
		else// 处理其他符号
		{ 
			if(ch==':')             /*检测赋值符号*/ 
			{//状态6
				getchdo;
				if(ch=='=')					
				{//状态7
					sym=becomes;
					getchdo;
				}
				else
				{
					sym=nul;            /*不能识别的符号*/
				}
			}
			else if(ch=='<')         /*检测小于或小于等于符号*/ //状态8
			{
				getchdo;
				if(ch=='=')			
				{//状态9
					sym=leq;
					getchdo;
				}else if(ch=='>')  //新增：<>不等于 运算符
				{
					sym = neq;
					getchdo;
				}
				else				
				{ //状态10
					sym=lss;
				}
			}
			else if(ch=='>')          /*检测大于或大于等于符号*/   
			{ //状态11
				getchdo;
				if(ch=='=')					
				{//状态12
					sym=geq;
					getchdo;
				}
				else					
				{//状态13
					sym=gtr;
				}
			}
			else if(ch=='!')
			{
				//printf("识别到”!“");
				getchdo;
				if(ch=='=')
				{
					//todo
					
					//getchdo;
				}
				else
				{
					sym=not1;
				}
			}
			else if(ch == '+')
			{
				getchdo;
				if(ch=='+')
				{
					sym = plusdouble;
					getchdo;
					//printf("识别++\n");
				}else if(ch=='=')
				{
					sym = plusbecomes;
					getchdo;
					//printf("识别+=\n");
				}else
				{
					sym = plus;
					
				}
			}
			else if(ch == '-')
			{
				getchdo;
				if(ch == '=')
				{
					sym = minusbecomes;
					getchdo;
				}else{
					sym = minus;
				}
			}
			else if(ch=='\'')//新增char字符型
			{
				getchdo;
				if((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z'))
				{
					num=(int)ch;
					getchdo;
					if(ch=='\'') 
						sym=charsym;
					else
					{
						num=0;
						sym=nul;
						error(39);  //类型错误                  
					}
				}
				else error(39); //类型不匹配
				getchdo;
			}
			else					
			{//状态14
				sym=ssym[ch];/* 当符号不满足上述条件时，全部按照单字符号处理*/
				//getchdo;
				//richard
				if(sym!=period)
				{
					getchdo;
				}
				//end richard
			}					
		}
	}
	return 0;
}
/*
*生成虚拟机代码
*
*x:instruction.f; //操作码
*y:instruction.l; //跳转指令需要的层次差
*z:instruction.a; //地址或操作数
*/
int gen(enum fct x,int y,int z)
{
	if(cx>=cxmax)
	{
		printf("Program too long"); /*程序过长*/
		return -1;
	}
	code[cx].f=x;
	code[cx].l=y;
	code[cx].a=z;
	cx++;
	return 0;
}
/*
*测试当前符号是否合法（语法分析阶段）
*
*在某一部分（如一条语句，一个表达式）将要结束时时我们希望下一个符号属于某集合
*（该部分的后跟符号） test 负责这项检测，并且负责当检测不通过时的补救措施
*程序在需要检测时指定当前需要的符号集合和补救用的集合（如之前未完成部分的后跟
*符号），以及不通过时的错误号
*
*S1：我们需要的符号
*s2:如果不是我们需要的，则需要一个补救用的集合
*n:错误号
*/
int test(bool* s1,bool* s2,int n)
{
    if(! inset(sym,s1))
    {
		error(n);
		/*当检测不通过时，不停获取符号，直到它属于需要的集合或补救的集合*/
		while((! inset(sym,s1))&&(! inset(sym,s2)))
		{
			getsymdo; 
		}
    }
    return 0;
}
/*
 *编译程序主体
 *
 *lev:当前分程序所在层
 *tx:名字表当前尾指针
 *fsys:当前模块后跟符号集合
 */
int block(int lev,int tx,bool* fsys)
{
    int i;
    int dx;                         /*名字分配到的相对地址*/
    int tx0;                        /*保留初始tx*/
    int cx0;                        /*保留初始cx*/
    bool nxtlev[symnum];            /*在下级函数（调用当前函数的函数）的参数中，符号集合均为值参，但由于使用数组
                                      实现，传递进来的是指针，为防止下级函数改变上级函数的
                                      集合，开辟新的空间传递给下级函数*/
    dx=3;							/* 初始偏移量，留出三个空间给静态链SL、动态链DL和返回地址RA */
    tx0=tx;                         /*记录本层名字的初始位置*/
    table[tx].adr=cx;				 /* 记录当前过程的代码起始地址 */
    gendo(jmp,0,0);					/* 生成跳转指令，用于后续回填过程调用位置 */
    if(lev > levmax)
    {
		error(32);                   //嵌套太深则报错                                                                                                                      
    }
    do{
        if(sym==constsym)         /*收到常量声明符号，开始处理常量声明*/
        {
			getsymdo;
			do{
                constdeclarationdo(&tx,lev,&dx);   /*dx的值会被constdeclaration改变，使用指针*/  //处理单个常量声明

                while(sym==comma)  //逗号分隔的多个常量声明
                {
                   getsymdo;
                   constdeclarationdo(&tx,lev,&dx);  //例如：这个函数就是下级函数
                }
                if(sym==semicolon)
                {
                  getsymdo;
				}
				else
				{
					error(5); /*漏掉了逗号或者分号*/
				}
			}while(sym==ident); // 处理下一常量
		}
		if(sym==varsym)/*收到变量声名符号，开始处理变量声名*/  //基本同上了
		{
			getsymdo;
			do{
				vardeclarationdo(&tx,lev,&dx);
				while(sym==comma)
				{
					getsymdo;
					vardeclarationdo(&tx,lev,&dx);
				}
				if(sym==semicolon)
				{
					getsymdo;
				}
				else
				{
					error(5);
				}
			}while(sym==ident);
		}
		if(sym==intsym)/*收到int声明符号，开始处理int声名*/  //新增int
		{
			getsymdo;
			do{
				realdeclarationdo(&tx,lev,&dx);
				while(sym==comma)
				{
					getsymdo;
					realdeclarationdo(&tx,lev,&dx);
				}
				if(sym==semicolon)
				{
					getsymdo;
				}
				else
				{
					error(5);
				}
			}while(sym==ident);
		}
		if(sym==charsym)/*收到符号型声名符号，开始处理符号型声名*/
        {
            getsymdo;
            do{
                chardeclarationdo(&tx,lev,&dx);
                while(sym==comma)
                {
                    getsymdo;
                    chardeclarationdo(&tx,lev,&dx);
                }
                if(sym==semicolon)
                {
                    getsymdo;
                }
                else
                {
                    error(5);
                }
            }while(sym==ident);
        }
		while(sym==procsym)/*收到过程声名符号，开始处理过程声名*/
		{
			getsymdo;
			if(sym==ident)
			{
				enter(procedur,&tx,lev,&dx);/*记录过程名字*/
				getsymdo;
			}
			else
			{
				error(4);/*procedure后应为标识符*/
			}
			if(sym==semicolon)
			{
				getsymdo;
			}
			else
			{
				error(5);/*漏掉了分号*/
			}

			 // 递归处理子过程体
			memcpy(nxtlev,fsys,sizeof(bool)*symnum);
			nxtlev[semicolon]=true;
			if(-1==block(lev+1,tx,nxtlev))
			{
				return -1;/*递归调用，处理过程体失败*/
			}
            if(sym==semicolon)
            {
                getsymdo;
                memcpy(nxtlev,statbegsys,sizeof(bool)*symnum);
                nxtlev[ident]=true;
                nxtlev[procsym]=true;
                testdo(nxtlev,fsys,6); //检查语句后的符号是否正确
             }
             else
             {
                 error(5);                       /*漏掉了分号*/
             }
        }
		//处理语句序列
        memcpy(nxtlev,statbegsys,sizeof(bool)*symnum);
        nxtlev[ident]=true;
        nxtlev[period]=true;
        testdo(nxtlev,declbegsys,7);		//检查语句起始符
    }while(inset(sym,declbegsys));                /*直到没有声明符号*/
    code[table[tx0].adr].a=cx;                    /*开始生成当前过程代码*/
    table[tx0].adr=cx;                            /*当前过程代码地址*/
    table[tx0].size=dx;                           /*声明部分中每增加一条声明都会给dx增加1,声明部分已经结束,dx就是当前过程数据的size*/
        
    cx0=cx;
    gendo(inte,0,dx);                             /*生成分配内存代码*/
    if(tableswitch)                               /*输出名字表*/
    {
        printf("TABLE:\n");
        if(tx0+1>tx)
        {
			printf("NULL\n");
        }
        for(i=tx0+1;i<=tx;i++)
        {
            switch(table[i].kind) // 名字表输出，   //todo 有需要的话在这里加上char和int
            {
                case constant:
					printf("%d const %s",i,table[i].name);
					printf("val=%d\n",table[i].val);
					fprintf(fas,"%d const %s",i,table[i].name);
					fprintf(fas,"val=%d\n",table[i].val);
                    break;
                case variable: //
                    printf("%d var%s",i,table[i].name);
                    printf("lev=%d addr=%d\n",table[i].level,table[i].adr);
                    fprintf(fas,"%d var %s",i,table[i].name);
                    fprintf(fas,"lev=%d addr=%d\n",table[i].level,table[i].adr);
                    break;
                case procedur:
                    printf("%d proc%s",i,table[i].name);
                    printf("lev=%d addr=%d size=%d\n",table[i].level,table[i].adr,table[i].size);
					fprintf(fas,"%d proc%s",i,table[i].name);
					fprintf(fas,"lev=%d adr=%d size=%d \n",table[i].level,table[i].adr,table[i].size);
					break;
			}
		}
		printf("\n");
	}
	/*语句后跟符号为分号或end*/
	memcpy(nxtlev,fsys,sizeof(bool)*symnum);/*每个后跟符号集和都包含上层后跟符号集和，以便补救*/
	nxtlev[semicolon]=true;
	nxtlev[endsym]=true;
	statementdo(nxtlev,&tx,lev);
	gendo(opr,0,0); /*每个过程出口都要使用的释放数据段命令*/
	memset(nxtlev,0,sizeof(bool)*symnum); /*分程序没有补救集合*/
	test(fsys,nxtlev,8);                  /*检测后跟符号正确性*/
	listcode(cx0);                        /*输出代码*/
	return 0;
}
/*
*在名字表（标识符）中加入一项
*
*k:名字种类const,var or procedure
*ptx:名字表尾指针的指针，为了可以改变名字表尾指针的数值
*lev:名字所在的层次，以后所有的lev都是这样
*pdx:为当前应分配的变量的相对地址，分配后要增加1
*/
void enter (enum object k,int *ptx,int lev, int *pdx)
{
	(*ptx)++;
	strcpy(table[(*ptx)].name,id);       /*全局变量id中已存有当前名字的名字*/
	table[(*ptx)].kind=k;
	switch(k)
	{
		case constant:                      /*常量名字*/
			if (num>amax)
			{
				error(31);
				num=0;
			}
			table[(*ptx)].val=num;
			break;
		case variable:                     /*变量名字*/
			table[(*ptx)].level=lev;
			table[(*ptx)].adr=(*pdx);
			(*pdx)++;
			break;                          /*过程名字*/
		case procedur:
			table[(*ptx)].level=lev;
			break;
		case charcon:                       /*新增 字符型名字*/
            table[(*ptx)].level=lev;
            table[(*ptx)].adr=(*pdx);
            (*pdx)++;
            break; 
		case realcon:                     /*新增int 名字*/
			table[(*ptx)].level=lev;
			table[(*ptx)].adr=(*pdx);
			(*pdx)++;
			break;    
	}

}
/*
 *查找名字（标识符）的位置
 *找到则返回在名字表中的位置，否则返回0
 *
 *idt: 要查找的名字
 *tx:：当前名字表尾指针
 */
int position(char *  idt,int  tx)
{
	int i;
	strcpy(table[0].name,idt);
	i=tx;
	while(strcmp(table[i].name,idt)!=0)
	{
		i--;
	}
	return i;
}
/*
 *常量声明处理
 */
int constdeclaration(int *  ptx,int lev,int *  pdx)
{
	if(sym==ident)
	{
		getsymdo;
		if(sym==eql ||sym==becomes)
		{
			if(sym==becomes)
			{
				error(1);                     /*把=写出成了：=*/
			}
			getsymdo;
			if(sym==number)
			{
				enter(constant,ptx,lev,pdx);
				getsymdo;
			}
			else
			{
				error(2);                  /*常量说明=后应是数字*/
			}         
		}
		else
		{
			error(3);                       /*常量说明标识后应是=*/
		}
	}
	else
	{
		error(4);                        /*const后应是标识*/
	}
	return 0;
}
/*
 *变量声明处理
 */
int vardeclaration(int * ptx,int lev,int * pdx)
{
	if(sym==ident)
 	{
 		enter(variable,ptx,lev,pdx);//填写名字表
 		getsymdo;
 	}
 	else
 	{
 		error(4);
    }
    return 0;
}


/*
 * 新增 字符声明处理
 */
int chardeclaration(int * ptx,int lev,int * pdx)
{
 if(sym==ident)
    {
        enter(charcon,ptx,lev,pdx);//填写名字表
        getsymdo;
    }
    else
    {
        error(4);
    }
    return 0;
}

/*
 * 新增 实数声明处理
 */
int realdeclaration(int * ptx,int lev,int * pdx)
{
	if(sym==ident)
 	{
 		enter(realcon,ptx,lev,pdx);//填写名字表
 		getsymdo;
 	}
 	else
 	{
 		error(4);
    }
    return 0;
}

 /*
  *输入目标代码清单
  */
void listcode(int cx0)
{
	int i;
   	if (listswitch)
   	{
   		for(i=cx0;i<cx;i++)
   	 	{
   	 		printf("%d %s %d %d\n",i,mnemonic[code[i].f],code[i].l,code[i].a);
   	 		fprintf(fa,"%d %s %d %d\n",i,mnemonic[code[i].f],code[i].l,code[i].a);
   	    }
   	 }
}
/*
*语句处理

ptx：指向当前符号表的指针
lev：当前语句所在的嵌套层级
*/
int statement(bool* fsys,int * ptx,int lev)
{
	int i,cx1,cx2,cx_if_cdt;
   	bool nxtlev[symnum];
   	if(sym==ident)
   	{
		i=position(id,*ptx);
   	 	if(i==0)
   	 	{
   	 		error(11);
   	 	}
   	 	else
   	 	{
   	 		if((table[i].kind!=variable)&&(table[i].kind!=charcon)&&(table[i].kind!=realcon)) // 找到了标识符且类型为变量variable
   	 		{
   	 		 	error(12);
   	 		 	i=0;
   	 		}
			else if(table[i].kind==charcon)
            {
                getsymdo;
                if(sym!=becomes) error(13);
                else
                {
                    getsymdo;
                    if(sym!=charsym) error(32);
                    else
                    {
                        gendo(lit,0,num);
                        gendo(sto,lev-table[i].level,table[i].adr);
                    }
                }
                getsymdo; //分号处理，可以仿照plusbecomes的操作
            }
   	 		else
   	 		{
   	 			getsymdo;
   	 			if(sym==becomes) //获取下一个符号，如果是becomes则继续处理
   	 			{
   	 				getsymdo;
					memcpy(nxtlev,fsys,sizeof(bool)* symnum); //备份后跟符号集合
					expressiondo(nxtlev,ptx,lev); //表达式处理
					if(i!=0)
					{
					 	gendo(sto,lev-table[i].level,table[i].adr); //根据变量地址生成sto，将表达式存储到变量中
					}
   	 			}else if(sym == plusbecomes)
				{
					getsymdo;
					/*找到变量地址并将其值入栈*/
					gendo(lod, lev - table[i].level, table[i].adr);
					if (sym == semicolon) // +=后跟分号
					{
						getsymdo;
					}
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					expressiondo(nxtlev, ptx, lev);
					gendo(opr, 0, 2);
					if (i != 0)
					{
						gendo(sto, lev - table[i].level, table[i].adr);
					}
				}else if (sym == minusbecomes)
				{
					getsymdo;/*找到变量地址并将其值入栈*/
					gendo(lod, lev - table[i].level, table[i].adr);
					if (sym == semicolon)
					{
						getsymdo;
					}
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					expressiondo(nxtlev, ptx, lev);
					gendo(opr, 0, 3);
					if (i != 0)
					{
						gendo(sto, lev - table[i].level, table[i].adr);
					}
				}else
   	 			{
   	 			 	error(13);
   	 			}
   	 			// memcpy(nxtlev,fsys,sizeof(bool)* symnum); //备份后跟符号集合
   	 			// expressiondo(nxtlev,ptx,lev); //表达式处理
   	 			// if(i!=0)
   	 			// {
   	 			//  	gendo(sto,lev-table[i].level,table[i].adr); //根据变量地址生成sto，将表达式存储到变量中
   	 			// }
			}
		}
    }
    else if(sym==readsym)  // 处理读语句
    {
		getsymdo;
		if(sym!=lparen)  //符号是否为lparen左括号
		{
			error(34);
		}
		else
		{
			do{
				getsymdo;
				if(sym==ident) //符号为ident
				{
					i=position(id, *ptx); //查找标识符位置
				}
				else
				{
					i=0;
				}
				if(i==0)
				{
					error(35);
				}else if(table[i].kind==charcon)//新增 字符型输入
				{
					gendo(opr,0,19);
					gendo(sto,lev-table[i].level,table[i].adr);
				}
				else
				{
					gendo(opr,0,16); //生成opr操作码
					gendo(sto,lev-table[i].level,table[i].adr);	/* 储存到变量*/
				}
				getsymdo;
			}while (sym==comma);	/*一条read语句可读多个变量 */ //遇到逗号则继续循环
		}
		if(sym!=rparen) //右括号
		{
			error(33);			/* 格式错误，应是右括号*/
			while(!inset(sym,fsys))/* 出错补救，直到收到上层函数的后跟符号*/
			{
				getsymdo;
			}
		}
		else
		{
			getsymdo;
		}
	}
	else if(sym==writesym)		//写语句类型处理	/* 准备按照write语句处理，与read类似*/
	{
		getsymdo;
		if(sym==lparen)
		{
			do{
				getsymdo;
				//
				if(sym==ident)
				{
					i=position(id,*ptx);
					if(i==0)
					{
						error(11);          /*过程未找到*/
					}
				}
				memcpy(nxtlev,fsys,sizeof(bool)*symnum);
				nxtlev[rparen]=true;
				nxtlev[comma]=true;		/* write的后跟符号为）or，*/
				expressiondo(nxtlev,ptx,lev);/* 调用表达式处理，此处与read不同，read为给变量赋值*/
				if(table[i].kind==charcon) //字符型输出 
				{   
					gendo(opr,0,17);   
				}else
				{
					gendo(opr,0,14);/* 生成输出指令，输出栈顶的值*/
				}
				
			}while(sym==comma);
			if(sym!=rparen)
			{
				error(33);/* write()应为完整表达式*/
			}
			else
			{
				getsymdo;
			}
		}
		gendo(opr,0,15);		/* 输出换行*/
	}
	//其他语句类型处理
	else if(sym==callsym)		/* 准备按照call语句处理*/
	{
		getsymdo;
		if(sym!=ident)
		{
			error(14);           /*call后应为标识符*/
		}
		else
		{
			i=position(id,*ptx);
			if(i==0)
			{
				error(11);          /*过程未找到*/
			}
			else
			{
				if(table[i].kind==procedur)
				{
					gendo(cal,lev-table[i].level,table[i].adr);  /*生成call指令*/
				}
				else
				{
					error(15);      /*call后标识符应为过程*/
				}
			}
			getsymdo;
		}
	}
	else if(sym==ifsym)     /*准备按照if语句处理*/
	{	
		/******** 用于条件跳转翻转（if true 则执行else后语句） */	
		//last_sym = ifsym;  
		/******** 用于条件跳转翻转（if true 则执行else后语句） */	
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[thensym]=true;
		nxtlev[dosym]=true;    /*后跟符号为then或do*/
		nxtlev[elsesym] = true;
		conditiondo(nxtlev,ptx,lev);   /*调用条件处理（逻辑运算）函数*/
		cx_if_cdt = cx-1;// 判断pcode代码所在行数
		if(sym==thensym)
		{
			getsymdo;   //thensym到writesym
		}
		else
		{
			error(16);          /*缺少then*/
		}
		cx1=cx;                /*保存当前指令地址*/  //cx1：jpc跳转，如果符合条件则跳转
		gendo(jpc,0,0);        /*生成条件跳转指令，跳转地址暂写0*/
		statementdo(fsys,ptx,lev);   /*处理then后的语句*/   //这步由writesym到elsesym

		//code[cx1].a=cx;      /*经statement处理后，cx为then后语句执行完的位置，它正是前面未定的跳转地址*/
		if (sym == elsesym) {
			cx2 = cx;   //cx2:else前面 jmp 直接跳转到程序结束 
			gendo(jmp, 0, 0); //以后会直接跳转到else语句后面
			getsymdo;
			code[cx1].a = cx; //回填条件跳转指令，条件为假时跳到else的语句块执行
			statementdo(fsys, ptx, lev);/*处理else后的语句*/
			code[cx2].a = cx; //当前指令地址cx是else后面的语句结束位置，if语句执行后要跳转到这里

			// 将之前保存的条件翻转
			//code[cx_if_cdt].a = reverse_condition(code[cx_if_cdt].a);

		}else {
			//回填条件跳转指令，条件为假或执行完then后继续执行
			code[cx1].a = cx;
			/* 经statement处理后，cx为then后语句执行完的位置，它正是前面未定的跳转地址 */
		}
	}
	else if(sym==beginsym)   /*准备按照复合语句处理*/
	{
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[semicolon]=true;
		nxtlev[endsym]=true;/*后跟符号为分号或end*/
		/*循环调用语句处理函数，直到下一个符号不是语句开始符号或收到end*/
		statementdo(nxtlev,ptx,lev);
		while(inset(sym,statbegsys)||sym==semicolon)
		{
			if(sym==semicolon)
			{
				getsymdo;
			}
			else
			{
				error(10);/*缺少分号*/
			}
			statementdo(nxtlev,ptx,lev);
		}
		if(sym==endsym)
		{
			getsymdo;
		}
		else
		{
			error(17); /*缺少end或分号*/
		}
	}
	else if(sym==whilesym)/*准备按照while语句处理*/
	{
		cx1=cx;        /*保存判断条件操作的位置*/
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[dosym]=true;/*后跟符号为do*/
		conditiondo(nxtlev,ptx,lev);  /*调用条件处理*/
		cx2=cx;       /*保存循环体的结束的下一个位置*/
		gendo(jpc,0,0);/*生成条件跳转，但跳出循环的地址未知*/
		if(sym==dosym)
		{
			getsymdo;
		}
		else
		{
			error(18);      /*缺少do*/
		}
		statementdo(fsys,ptx,lev); /*循环体*/
		gendo(jmp,0,cx1);/*回头重新判断条件*/
		code[cx2].a=cx;   /*反填跳出循环的地址，与if类似*/
	}else if (sym == forsym)
	{
		getsymdo;
		if (sym == ident)   /* 准备按照赋值语句处理 */
		{
			i = position(id, *ptx);
			if (i == 0)
			{
				error(11);  /* 变量未找到 */
			}
			else
			{
				if (table[i].kind != variable &&table[i].kind != realcon) //新增int类型判断
				{
					error(12);  /* 赋值语句格式错误(左边标识符属性不是变量) */
					//i = 0;
				}
				else
				{
					getsymdo;
					if (sym == becomes) {
						getsymdo;
					}
					else {
						error(13);  /* 没有检测到赋值符号 */
					}
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					nxtlev[tosym]=true;                     //后跟符to和downto
					nxtlev[downtosym]=true;
					expressiondo(nxtlev, ptx, lev); /* 处理赋值符号右侧表达式 */
					gendo(sto,lev-table[i].level,table[i].adr);     //保存初值
                 	if(sym==tosym)
					{
                        getsymdo;
                        cx1=cx;       //保存循环开始点
                        //将循环判断变量取出放到栈顶
                        gendo(lod,lev-table[i].level,table[i].adr);                                        
                        memcpy(nxtlev,fsys,sizeof(bool)*symnum);    //处理表达式
                        nxtlev[dosym]=true;                         //后跟符do
                        expressiondo(nxtlev,ptx,lev);
                        /*判断循环变量条件*/     
                        gendo(opr,0,13);             //生成比较指令，i是否小于等于条件
						cx2=cx;   
                        //生成条件跳转指令，跳出循环，跳出的地址未知
                        gendo(jpc,0,0);                     
                        if(sym==dosym)               //处理循环体
                        {
                            getsymdo;
                            statement(fsys,ptx,lev);  //循环体处理
                            //增加循环变量步长为
                            //将循环变量取出放在栈顶
                            gendo(lod,lev-table[i].level,table[i].adr);                                       
                            gendo(lit,0,1);                            //将步长取到栈顶
                            gendo(opr,0,2);                            //循环变量加步长
                            //将栈顶的值存入循环变量
                            gendo(sto,lev-table[i].level,table[i].adr); 
                            gendo(jmp,0,cx1);                 //无条件跳转到循环开始点
                            /*回填循环结束点的地址，cx为else后语句执行完的位置，它正是前面未定的跳转地址*/
                            code[cx2].a=cx;                     
                         }
                         else
                         {
                            error(29);    //for语句中少了do
                         }
                    }else if(sym == downtosym)
					{
						getsymdo;
                        cx1=cx;       //保存循环开始点
                        //将循环判断变量取出放到栈顶
                        gendo(lod,lev-table[i].level,table[i].adr);                                        
                        memcpy(nxtlev,fsys,sizeof(bool)*symnum);    //处理表达式
                        nxtlev[dosym]=true;                         //后跟符do
                        expressiondo(nxtlev,ptx,lev);
                        /*判断循环变量条件*/     
                        gendo(opr,0,11);             //生成比较指令，i是否大于等于条件
                        cx2=cx;                      //保存循环结束点
                        //生成条件跳转指令，跳出循环，跳出的地址未知
                        gendo(jpc,0,0);                     
                        if(sym==dosym)               //处理循环体
                        {
                            getsymdo;
                            statement(fsys,ptx,lev);  //循环体处理
                            //增加循环变量步长为
                            //将循环变量取出放在栈顶
                            gendo(lod,lev-table[i].level,table[i].adr);                                       
                            gendo(lit,0,1);                            //将步长取到栈顶
                            gendo(opr,0,3);                            //循环变量减步长
                            //将栈顶的值存入循环变量
                            gendo(sto,lev-table[i].level,table[i].adr); 
                            gendo(jmp,0,cx1);                 //无条件跳转到循环开始点
                            /*回填循环结束点的地址，cx为else后语句执行完的位置，它正是前面未定的跳转地址*/
                            code[cx2].a=cx;                     
                        }
                        else
                        {
                            error(29);    //for语句中少了do
                        }
					}
				}
			}
		}
	}
	else
	{
		memset(nxtlev,0,sizeof(bool)*symnum);/*语句结束无补救集合*/
		testdo(fsys,nxtlev,19);/*检测语句结束的正确性*/
	}

	return 0;
}
/*
*表达式处理
*/
int expression(bool*fsys,int*ptx,int lev)
{
	enum symbol addop;                    /*用于保存正负号*/
	bool nxtlev[symnum];
	if(sym==plus||sym==minus)             /*开头的正负号，此时当前表达式被看作一个正的或负的项*/
	{
		addop=sym;                    /*保存开头的正负号*/
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;
		termdo(nxtlev,ptx,lev);                /*处理项*/
		if(addop==minus)
		{
			gendo(opr,0,1);                   /*如果开头为负号生成取负指令*/
		}
	}
	else                             /*此时表达式被看作项的加减*/
	{
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;
		termdo(nxtlev,ptx,lev);            /*处理项*/
	}
	while(sym==plus||sym==minus)
	{
		addop=sym;
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;
		termdo(nxtlev,ptx,lev);              /*处理项*/
		if(addop==plus)
		{
			gendo(opr,0,2);                    /*生成加法指令*/
		}
		else
		{
			 gendo(opr,0,3);                /*生成减法指令*/
		}
	}
	return 0;
}
/*
*项处理
*/
int term(bool*fsys,int *ptx,int lev)
{
    enum symbol mulop;               /*用于保存乘除法符号*/
    bool nxtlev[symnum];
    memcpy(nxtlev,fsys,sizeof(bool)*symnum) ;
    nxtlev[times]=true;
    nxtlev[slash]=true;
    factordo(nxtlev,ptx,lev);       /*处理因子*/
    while(sym==times||sym==slash)
    {
		mulop=sym;
        getsymdo;
        factordo(nxtlev,ptx,lev);
        if(mulop==times)
        {
            gendo(opr,0,4);          /*生成乘法指令*/
        }
        else
        {
            gendo(opr,0,5);           /*生成除法指令*/
        }
    }
     return 0;
}
/*
*因子处理
*/
int factor(bool*fsys,int *ptx,int lev)
{
    int i;
    bool nxtlev[symnum];
    testdo(facbegsys,fsys,24);           /*检测因子的开始符好号*/
    while(inset(sym,facbegsys))          /*循环直到不是因子开始符号*/
    {
        if(sym==ident)                   /*因子为常量或者变量*/
        {
            i=position(id,*ptx);        /*查找名字*/
            if(i==0)
            {
                error(11);               /*标识符未声明*/
            }
            else
            {
				switch(table[i].kind)
				{
					case constant:                                      /*名字为常量*/
						gendo(lit,0,table[i].val);                       /*直接把常量的值入栈*/
						break;
					case variable:                                      /*名字为变量*/
						gendo(lod,lev-table[i].level,table[i].adr);      /*找到变量地址并将其值入栈*/
						break;
					case procedur:                                      /*名字为过程*/
						error(21);                                       /*不能为过程*/
						break;
					case charcon:                                      /*新增 名字为字符型*/
                        gendo(lod,lev-table[i].level,table[i].adr);      /*找到字符型地址并将其值入栈*/
                        break;
					case realcon:                                      /*新增 名字为int*/
						gendo(lod,lev-table[i].level,table[i].adr);      /*找到int地址并将其值入栈*/
						break;
				}
			}
			getsymdo;
		}
		else
		{
			if(sym==number)                                             /*因子为数*/
			{
				if(num>amax)
				{
					error(31);
					num=0;
				}
				gendo(lit,0,num);
				getsymdo;
			}
			else
			{
				if(sym==lparen)                                           /*因子为表达式*/
				{
					getsymdo;
					memcpy(nxtlev,fsys,sizeof(bool)*symnum);
					nxtlev[rparen]=true;
					expressiondo(nxtlev,ptx,lev);
					if(sym==rparen)
					{
						getsymdo;
					}
					else
					{
						error(22);                                       /*缺少右括号*/
					}
				}
				testdo(fsys,facbegsys,23);                        /*因子后有非法符号*/
			}
		}
	}
	return 0;
}
/*
条件处理*/
int condition(bool* fsys,int* ptx,int lev)
{
    enum symbol relop;
    bool nxtlev[symnum];
    if(sym==oddsym)                        /*准备按照odd运算处理*/
	{
		getsymdo;
		expressiondo(fsys,ptx,lev);
		gendo(opr,0,6);                              /*生成odd指令*/
    }
    else
    {
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[eql]=true;
		nxtlev[neq]=true;
		nxtlev[lss]=true;
		nxtlev[leq]=true;
		nxtlev[gtr]=true;
		nxtlev[geq]=true;
		expressiondo(nxtlev,ptx,lev);


		if(sym!=eql&&sym!=neq&&sym!=lss&&sym!=leq&&sym!=gtr&&sym!=geq)
		{
			error(20);
		}
		else
		{
			relop=sym;
			getsymdo;
			expressiondo(fsys,ptx,lev);
			switch(relop)
			{
				case eql:
					gendo(opr,0,8);
					break;
				case neq:
					gendo(opr,0,9);
					break;
				case lss:
					gendo(opr,0,10);
					break;
				case geq:
					gendo(opr,0,11);
					break;
				case gtr:
					gendo(opr,0,12);
					break;
				case leq:
					gendo(opr,0,13);
					break;
			}

		}
    }
    return 0;
}                                                  /*解释程序*/

void interpret1()
{
	int p,b,t;             /*指令指针，指令基址，栈顶指针*/
	struct instruction i;  /*存放当前指令*/
	int s[stacksize];      /*栈*/
	printf("start pl0\n");
	t=0;
	b=0;
	p=0;
	s[0]=s[1]=s[2]=0;
	do{
		i=code[p];         /*读当前指令*/
		p++;
		switch(i.f)
		{
			case lit:        /*将a的值取到栈顶*/
				s[t]=i.a; // 将i.a压入栈顶
				t++;
				break;
			case opr:        /*数字、逻辑运算*/
				switch(i.a)
				{
					case 0:
						t=b;
						p=s[t+2];
						b=s[t+1];
						break;
					case 1: // 取反
						s[t-1]=-s[t-1];
						break;
					case 2:
						t--;
						s[t-1]=s[t-1]+s[t];
						break;
					case 3:
						t--;
						s[t-1]=s[t-1]-s[t];
						break;
					case 4:
						t--;
						s[t-1]=s[t-1]*s[t];
						break;
					case 5:
						t--;
						s[t-1]=s[t-1]/s[t];
             			break;
					case 6:
						s[t-1]=s[t-1]%2;
						break;
					case 8:
						t--;
						s[t-1]=(s[t-1]==s[t]);
 						break;
					case 9:
						t--;
						s[t-1]=(s[t-1]!=s[t]);
 						break;
					case 10:
						t--;
						s[t-1]=(s[t-1]<s[t]);
 						break;
					case 11:
						t--;
						s[t-1]=(s[t-1]>=s[t]);
 						break;
					case 12:
						t--;
						s[t-1]=(s[t-1]>s[t]);
 						break;
					case 13:
						t--;
						s[t-1]=(s[t-1]<=s[t]);
 						break;
					case 14:    
						printf("%d",s[t-1]);  //修改 字符型，这里输出数字
						fprintf(fa2,"%d",s[t-1]);
						t--;
						break;
					case 15:
						printf("\n");
						fprintf(fa2,"\n");
						break;
					case 16:
						printf("?");
						fprintf(fa2,"?");
						scanf("%d",&(s[t]));
						fprintf(fa2,"%d\n",s[t]);
						t++;
						break;
					case 17:// 输出栈顶值
                        printf("(Char):");
                        printf("%c\n",s[t-1]);//输出栈顶值
                        fprintf(fa2,"%c\n",s[t-1]);//同时打印到文件
                        t--;//栈顶下移
                        break;

                    case 19://输入栈顶
                        printf("(char):");
                        fprintf(fa2,"(char):");
                        getchar();//消除输入的enter
                        scanf("%c",&(s[t]));

                        fprintf(fa2,"%c\n",s[t]);//把用户输入值打印到文件
                        t++;//栈顶上移，分配空间
                        break;
				}
				break;
			case lod:       /*取相对当前过程的数据基地址为ａ的内存的值到栈顶*/
				s[t]=s[base(i.l,s,b)+i.a];
				t++;
				break;
			case sto:       /*栈顶的值存到相对当前过程的数据基地址为ａ的内存*/
				t--;
	            s[base(i.l,s,b)+i.a]=s[t];
				break;
			case cal:              /*调用子程序*/
				s[t]=base(i.l,s,b); /*将父过程基地址入栈*/
				s[t+1]=b;           /*将本过程基地址入栈，此两项用于base函数*/
				s[t+2]=p;           /*将当前指令指针入栈*/
				b=t;                /*改变基地址指针值为新过程的基地址*/
				p=i.a;              /*跳转*/
				break;
			case inte:             /*分配内存*/
				t+=i.a;
				break;
			case jmp:             /*直接跳转*/
				p=i.a;
				break;
			case jpc:              /*条件跳转*/
				t--;
				if(s[t]==0)
				{
					p=i.a;
				}
	    		break;
		}
	}while (p!=0);
}
/*通过过程基址求上1层过程的基址*/
int base(int l,int * s,int b)
{
	int b1;
	b1=b;
	 while(l>0)
	 {
		 b1=s[b1];
     	 l--;
	 }
	 return b1;
}

// 交换PCode
int exchangeCode(int cx1,int cx1_end,int cx2,int cx2_end){
	
     //TODO 将cx1到cx1_end和cx2到cx2_end进行换位，else和then后面的语句处理就会对调
    return -1;
}

int reverse_condition(int a){
	switch(a)
			{
				case eql:
					return 9;
				case neq:
					return 8;
				case lss:
					return 11;
				case geq:
					return 10;
				case gtr:
					return 13;
				case leq:
					return 12;
			}
	return -1;
}