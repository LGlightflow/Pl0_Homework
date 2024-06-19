//A.2     C      ��   ��

/*��������л���:
*1Visual C++6.0,VisualC++.NET and Visual C++.NET 2003
*WinNT, Win 200, WinXP and  Win2003 
*2 gcc version 3.3.2  20031022(Red Hat Linux 3.3.2-1)
*Redhat Fedora core 1
*Intel 32 platform
*ʹ�÷���:
*���к�����PL/0 Դ�����ļ���
*�ش��Ƿ�������������
*�ش��Ƿ�������ֱ�
*fa.tmp ������������
*fa1.tmp  ���Դ�ļ�������ж�Ӧ���׵�ַ
*fa2.tmp  ������ 
*fas.tmp  ������ֱ�
*/
#include<stdio.h>
#include<stdlib.h>
#include"pl0.h"
#include"string.h"

/*����ִ��ʱʹ�õ�ջ*/
#define stacksize 500
int main()
{
	bool nxtlev[symnum];
	printf("Input pl/0 file ?");
	scanf("%s",fname);                                     /*�����ļ���*/
	fin=fopen(fname,"r");
	if(fin)
	{
		printf("List object code ?(Y/N)");                /*�Ƿ�������������*/
		scanf("%s",fname);
		listswitch=(fname[0]=='y'||fname[0]=='Y');
		printf("List symbol table ? (Y/N)");             /*�Ƿ�������ֱ�*/
		scanf("%s",fname);
		tableswitch=(fname[0]=='y'||fname[0]=='Y');
		fa1=fopen("fa1.tmp","w");
		fprintf(fa1,"Iput pl/0 file ?");
		fprintf(fa1,"%s\n", fname);
		init();                                          /*��ʼ��*/
		err=0;
		cc=cx=ll=0;
		ch=' ';
		if(-1!=getsym())
		{
			fa=fopen("fa.tmp","w");
			fas=fopen("fas.tmp","w");
			addset(nxtlev,declbegsys,statbegsys,symnum);
			nxtlev[period]=true;    
			if(-1==block(0,0,nxtlev))			/*���ñ������*/
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
*��ʼ��
*/
void init()
{
	int i;
	
	// ��ʼ���ַ�����
	for(i=0;i<=255;i++)
	{
		ssym[i]=nul;
	}

	//Ϊ�ض��ַ���+, -, *�����ö�Ӧ�ķ��ţ����Ž����ڴʷ������׶�
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
    ssym['!'] = not1;  //����

        //���ñ����ֺͷ��ţ����﷨�����׶�ʶ��ؼ���
	/*���ñ���������,������ĸ˳��,�����۰����*/
	strcpy(&(word[0][0]),"begin");
	strcpy(&(word[1][0]),"call");
	strcpy(&(word[2][0]),"char");//���� �ַ���char
	strcpy(&(word[3][0]),"const");
	strcpy(&(word[4][0]),"do");	//����do
	strcpy(&(word[5][0]),"downto"); //����downto
	strcpy(&(word[6][0]),"else"); //����
	strcpy(&(word[7][0]),"end");
	strcpy(&(word[8][0]),"for");
	strcpy(&(word[9][0]),"if");
	strcpy(&(word[10][0]),"int");
	strcpy(&(word[11][0]),"odd");
	strcpy(&(word[12][0]),"procedure");
	strcpy(&(word[13][0]),"read");
	strcpy(&(word[14][0]),"then");
	strcpy(&(word[15][0]),"to"); //����to
	strcpy(&(word[16][0]),"var");
	strcpy(&(word[17][0]),"while");
	strcpy(&(word[18][0]),"write");
	//��
	/*���ñ����ַ���*/
	wsym[0]=beginsym;
	wsym[1]=callsym;
	wsym[2]=charsym;  //���� �ַ���char
	wsym[3]=constsym;
	wsym[4]=dosym;
	wsym[5]=downtosym;
	wsym[6]=elsesym;	//����
	wsym[7]=endsym;
	wsym[8]=forsym;
	wsym[9]=ifsym;
	wsym[10]=intsym;	//����int����
	wsym[11]=oddsym;
	wsym[12]=procsym;
	wsym[13]=readsym;
	wsym[14]=thensym;
	wsym[15]=tosym;
	wsym[16]=varsym;
	wsym[17]=whilesym;
	wsym[18]=writesym;

	//Ϊ�����ָ��������ƣ���Щ���ƽ��������м�����Ŀ�����ʱʹ��
	/*����ָ������*/
	strcpy(&(mnemonic[lit][0]),"lit");
	strcpy(&(mnemonic[opr][0]),"opr");
	strcpy(&(mnemonic[lod][0]),"lod");
	strcpy(&(mnemonic[sto][0]),"sto");
	strcpy(&(mnemonic[cal][0]),"cal");
	strcpy(&(mnemonic[inte][0]),"int");
	strcpy(&(mnemonic[jmp][0]),"jmp");
	strcpy(&(mnemonic[jpc][0]),"jpc");

	//��ʼ���������������ӿ�ʼ���ż���ȷ�����з��ż�������ȷ��ʼ��Ϊfalse
	/*���÷��ż�*/
	for(i=0;i<symnum;i++)
	{
		declbegsys[i]=false;
		statbegsys[i]=false;
		facbegsys[i]=false;
	}

	// �����������������ӿ�ʼ���ż����Ա����﷨����ʱ��ȷʶ�����ṹ
	/*����������ʼ���ż�*/
	declbegsys[constsym]=true;
	declbegsys[varsym]=true;
	declbegsys[procsym]=true;
	declbegsys[charsym]=true;//���� �ַ��� charsym
	declbegsys[intsym]=true;//����int����
	/*������俪ʼ���ż�*/
	statbegsys[beginsym]=true;
	statbegsys[callsym]=true;
	statbegsys[ifsym]=true;
	statbegsys[whilesym]=true;
	/*�������ӿ�ʼ���ż�*/
	facbegsys[ident]=true;
	facbegsys[number]=true;
	facbegsys[lparen]=true;
	facbegsys[charsym]=true;//���� �ַ��� charsym
	facbegsys[intsym]=true;
}

 /*
  *������ʵ�ּ��ϵļ������㣺
  ���﷨�����׶Σ���������Ҫȷ����Щ���ſ��ԺϷ��س������ض�λ��
  		���������ţ��� const��var��procedure �ȣ�
		�����ţ��� begin��if��while��call �ȣ�
		���ӷ��ţ����ʶ�������֡������ŵȣ���
	ͨ��ʹ�ü������㣬���Է���ع���ͼ����Щ���š�
	���磬�ж�һ�������Ƿ���ĳ�������У�inset ���������ϲ�������ż��ϣ�addset �������������������ϵĽ�����mulset ������

	�������
		��Ҫ�������������ȷ�ԣ������ͼ�顢�������顢������������
			�������Ƿ��������ķ�Χ��ʹ�á�
			ȷ���������ṹ���� if ��䡢while ѭ�����ĺϷ��ԡ�
			�������������ȷ�������ͺ�������ȷ����������������ʹ�á�
  */

// �ж�Ԫ�� e �Ƿ��ڼ��� s ��
int inset(int e,bool* s)
{
    return s[e];
}
// ʵ�ּ��ϵĲ���������
int addset(bool* sr,bool* s1,bool* s2,int n)
{
    int i;
    for(i=0;i<n;i++)
    {
       sr[i]=s1[i]||s2[i];  
    }
    return 0;
}
//ʵ�ּ��ϵĲ�����
int subset(bool* sr,bool* s1,bool* s2,int n)
{
    int i;
    for(i=0;i<n;i++)
    {
       sr[i]=s1[i]&&(!s2[i]);
    }
    return 0; 
}
//ʵ�ּ��ϵĽ�����
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
 *��������ӡ����λ�úʹ������
 */
void error(int n)
 
{
	char space[81];
	memset(space,32,81); //32��Ӧ�ո��ַ� �� space �����ǰ 81 ���ֽ�ȫ������Ϊ�ո��ַ�
	printf("-------%c\n",ch);  // ��ӡ��ǰ�ַ���������λ����λ��
	space[cc-1]=0;//����ʱ��ǰ�����Ѿ����꣬����cc-1
	printf("****%s!%d\n",space,n);
	err++;
}
/*
 *  ©���ո񣬶�ȡһ���ַ�
 *
 *  ÿ�ζ�һ�У�����line��������line��getsymȡ�պ��ٶ�һ��
 *
 *  ������getsym����
 */
int getch()
{
	// ����Ѿ���ȡ����ǰ�е�ĩβ���������һ��
	if(cc==ll)
    {
		// ����ļ������������������������Ϣ
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
		while(ch!=10)  //�Ƿ�������з�
		{
			//fscanf(fin,"%c",&ch)
			// ��ȡһ���ַ��������ȡ���ļ�������������ѭ��
            if(EOF==fscanf(fin,"%c",&ch))
			{
				line[ll]=0; // ��ǰ�г�����0��������ǰ��
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
	 // ���л�������ȡһ���ַ�
	ch=line[cc];
	cc++;
	return 0;
}
 /*�ʷ���������ȡһ�����ţ����ʣ�
 */

int getsym()
{
	int i,j,k;
	 // �����ո񡢻��к��Ʊ��
	while( ch==' '||ch==10||ch==9|| ch==13)// ״̬1   //13��tab
	{	
		getchdo;
	} 
	// ��������ĸ��ͷ�ı�ʶ��
	if(ch>='a'&&ch<='z')
	{ 
		//״̬2
		k=0;
		do{
			if(k<al)
			{ 
				a[k]=ch;
				k++;
			}
			getchdo; 
		}while(ch>='a'&&ch<='z'||ch>='0'&&ch<='9'); // �����ֻ��ʶ��
		// ״̬3
		a[k]=0;
		strcpy(id,a);
		// ���ֲ��ң�ȷ����ʶ���Ƿ�Ϊ������
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
			sym=wsym[k]; //������
            //printf("ʶ���������\n");
        }
		else
		{
			sym=ident; //��ʶ��
            //printf("ʶ�����ʶ��\n");
		}
	}
	else// ��������
	{	
		if(ch>='0'&&ch<='9') 
		{//״̬4
			k=0;
			num=0;
			sym=number;
			/*����ʶ������12����һ����'1',num= 10*0+1;�ڶ���2��num=1*10+2 1*10�����λ
			* ��һ��ʶ�������Ĺ���
			*/
			do{
				num=10*num+ch-'0';
				k++;
				getchdo;
			}while(ch>='0'&&ch<='9'); /*��ȡ���ֵ�ֵ*/   //״̬5
			k--;
			if(k>nmax)
			{
				error(30); //���ֹ���
			}
		}
		else// ������������
		{ 
			if(ch==':')             /*��⸳ֵ����*/ 
			{//״̬6
				getchdo;
				if(ch=='=')					
				{//״̬7
					sym=becomes;
					getchdo;
				}
				else
				{
					sym=nul;            /*����ʶ��ķ���*/
				}
			}
			else if(ch=='<')         /*���С�ڻ�С�ڵ��ڷ���*/ //״̬8
			{
				getchdo;
				if(ch=='=')			
				{//״̬9
					sym=leq;
					getchdo;
				}else if(ch=='>')  //������<>������ �����
				{
					sym = neq;
					getchdo;
				}
				else				
				{ //״̬10
					sym=lss;
				}
			}
			else if(ch=='>')          /*�����ڻ���ڵ��ڷ���*/   
			{ //״̬11
				getchdo;
				if(ch=='=')					
				{//״̬12
					sym=geq;
					getchdo;
				}
				else					
				{//״̬13
					sym=gtr;
				}
			}
			else if(ch=='!')
			{
				//printf("ʶ�𵽡�!��");
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
					//printf("ʶ��++\n");
				}else if(ch=='=')
				{
					sym = plusbecomes;
					getchdo;
					//printf("ʶ��+=\n");
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
			else if(ch=='\'')//����char�ַ���
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
						error(39);  //���ʹ���                  
					}
				}
				else error(39); //���Ͳ�ƥ��
				getchdo;
			}
			else					
			{//״̬14
				sym=ssym[ch];/* �����Ų�������������ʱ��ȫ�����յ��ַ��Ŵ���*/
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
*�������������
*
*x:instruction.f; //������
*y:instruction.l; //��תָ����Ҫ�Ĳ�β�
*z:instruction.a; //��ַ�������
*/
int gen(enum fct x,int y,int z)
{
	if(cx>=cxmax)
	{
		printf("Program too long"); /*�������*/
		return -1;
	}
	code[cx].f=x;
	code[cx].l=y;
	code[cx].a=z;
	cx++;
	return 0;
}
/*
*���Ե�ǰ�����Ƿ�Ϸ����﷨�����׶Σ�
*
*��ĳһ���֣���һ����䣬һ�����ʽ����Ҫ����ʱʱ����ϣ����һ����������ĳ����
*���ò��ֵĺ�����ţ� test ���������⣬���Ҹ��𵱼�ⲻͨ��ʱ�Ĳ��ȴ�ʩ
*��������Ҫ���ʱָ����ǰ��Ҫ�ķ��ż��ϺͲ����õļ��ϣ���֮ǰδ��ɲ��ֵĺ��
*���ţ����Լ���ͨ��ʱ�Ĵ����
*
*S1��������Ҫ�ķ���
*s2:�������������Ҫ�ģ�����Ҫһ�������õļ���
*n:�����
*/
int test(bool* s1,bool* s2,int n)
{
    if(! inset(sym,s1))
    {
		error(n);
		/*����ⲻͨ��ʱ����ͣ��ȡ���ţ�ֱ����������Ҫ�ļ��ϻ򲹾ȵļ���*/
		while((! inset(sym,s1))&&(! inset(sym,s2)))
		{
			getsymdo; 
		}
    }
    return 0;
}
/*
 *�����������
 *
 *lev:��ǰ�ֳ������ڲ�
 *tx:���ֱ�ǰβָ��
 *fsys:��ǰģ�������ż���
 */
int block(int lev,int tx,bool* fsys)
{
    int i;
    int dx;                         /*���ַ��䵽����Ե�ַ*/
    int tx0;                        /*������ʼtx*/
    int cx0;                        /*������ʼcx*/
    bool nxtlev[symnum];            /*���¼����������õ�ǰ�����ĺ������Ĳ����У����ż��Ͼ�Ϊֵ�Σ�������ʹ������
                                      ʵ�֣����ݽ�������ָ�룬Ϊ��ֹ�¼������ı��ϼ�������
                                      ���ϣ������µĿռ䴫�ݸ��¼�����*/
    dx=3;							/* ��ʼƫ���������������ռ����̬��SL����̬��DL�ͷ��ص�ַRA */
    tx0=tx;                         /*��¼�������ֵĳ�ʼλ��*/
    table[tx].adr=cx;				 /* ��¼��ǰ���̵Ĵ�����ʼ��ַ */
    gendo(jmp,0,0);					/* ������תָ����ں���������̵���λ�� */
    if(lev > levmax)
    {
		error(32);                   //Ƕ��̫���򱨴�                                                                                                                      
    }
    do{
        if(sym==constsym)         /*�յ������������ţ���ʼ����������*/
        {
			getsymdo;
			do{
                constdeclarationdo(&tx,lev,&dx);   /*dx��ֵ�ᱻconstdeclaration�ı䣬ʹ��ָ��*/  //��������������

                while(sym==comma)  //���ŷָ��Ķ����������
                {
                   getsymdo;
                   constdeclarationdo(&tx,lev,&dx);  //���磺������������¼�����
                }
                if(sym==semicolon)
                {
                  getsymdo;
				}
				else
				{
					error(5); /*©���˶��Ż��߷ֺ�*/
				}
			}while(sym==ident); // ������һ����
		}
		if(sym==varsym)/*�յ������������ţ���ʼ�����������*/  //����ͬ����
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
		if(sym==intsym)/*�յ�int�������ţ���ʼ����int����*/  //����int
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
		if(sym==charsym)/*�յ��������������ţ���ʼ�������������*/
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
		while(sym==procsym)/*�յ������������ţ���ʼ�����������*/
		{
			getsymdo;
			if(sym==ident)
			{
				enter(procedur,&tx,lev,&dx);/*��¼��������*/
				getsymdo;
			}
			else
			{
				error(4);/*procedure��ӦΪ��ʶ��*/
			}
			if(sym==semicolon)
			{
				getsymdo;
			}
			else
			{
				error(5);/*©���˷ֺ�*/
			}

			 // �ݹ鴦���ӹ�����
			memcpy(nxtlev,fsys,sizeof(bool)*symnum);
			nxtlev[semicolon]=true;
			if(-1==block(lev+1,tx,nxtlev))
			{
				return -1;/*�ݹ���ã����������ʧ��*/
			}
            if(sym==semicolon)
            {
                getsymdo;
                memcpy(nxtlev,statbegsys,sizeof(bool)*symnum);
                nxtlev[ident]=true;
                nxtlev[procsym]=true;
                testdo(nxtlev,fsys,6); //�������ķ����Ƿ���ȷ
             }
             else
             {
                 error(5);                       /*©���˷ֺ�*/
             }
        }
		//�����������
        memcpy(nxtlev,statbegsys,sizeof(bool)*symnum);
        nxtlev[ident]=true;
        nxtlev[period]=true;
        testdo(nxtlev,declbegsys,7);		//��������ʼ��
    }while(inset(sym,declbegsys));                /*ֱ��û����������*/
    code[table[tx0].adr].a=cx;                    /*��ʼ���ɵ�ǰ���̴���*/
    table[tx0].adr=cx;                            /*��ǰ���̴����ַ*/
    table[tx0].size=dx;                           /*����������ÿ����һ�����������dx����1,���������Ѿ�����,dx���ǵ�ǰ�������ݵ�size*/
        
    cx0=cx;
    gendo(inte,0,dx);                             /*���ɷ����ڴ����*/
    if(tableswitch)                               /*������ֱ�*/
    {
        printf("TABLE:\n");
        if(tx0+1>tx)
        {
			printf("NULL\n");
        }
        for(i=tx0+1;i<=tx;i++)
        {
            switch(table[i].kind) // ���ֱ������   //todo ����Ҫ�Ļ����������char��int
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
	/*���������Ϊ�ֺŻ�end*/
	memcpy(nxtlev,fsys,sizeof(bool)*symnum);/*ÿ��������ż��Ͷ������ϲ������ż��ͣ��Ա㲹��*/
	nxtlev[semicolon]=true;
	nxtlev[endsym]=true;
	statementdo(nxtlev,&tx,lev);
	gendo(opr,0,0); /*ÿ�����̳��ڶ�Ҫʹ�õ��ͷ����ݶ�����*/
	memset(nxtlev,0,sizeof(bool)*symnum); /*�ֳ���û�в��ȼ���*/
	test(fsys,nxtlev,8);                  /*�����������ȷ��*/
	listcode(cx0);                        /*�������*/
	return 0;
}
/*
*�����ֱ���ʶ�����м���һ��
*
*k:��������const,var or procedure
*ptx:���ֱ�βָ���ָ�룬Ϊ�˿��Ըı����ֱ�βָ�����ֵ
*lev:�������ڵĲ�Σ��Ժ����е�lev��������
*pdx:Ϊ��ǰӦ����ı�������Ե�ַ�������Ҫ����1
*/
void enter (enum object k,int *ptx,int lev, int *pdx)
{
	(*ptx)++;
	strcpy(table[(*ptx)].name,id);       /*ȫ�ֱ���id���Ѵ��е�ǰ���ֵ�����*/
	table[(*ptx)].kind=k;
	switch(k)
	{
		case constant:                      /*��������*/
			if (num>amax)
			{
				error(31);
				num=0;
			}
			table[(*ptx)].val=num;
			break;
		case variable:                     /*��������*/
			table[(*ptx)].level=lev;
			table[(*ptx)].adr=(*pdx);
			(*pdx)++;
			break;                          /*��������*/
		case procedur:
			table[(*ptx)].level=lev;
			break;
		case charcon:                       /*���� �ַ�������*/
            table[(*ptx)].level=lev;
            table[(*ptx)].adr=(*pdx);
            (*pdx)++;
            break; 
		case realcon:                     /*����int ����*/
			table[(*ptx)].level=lev;
			table[(*ptx)].adr=(*pdx);
			(*pdx)++;
			break;    
	}

}
/*
 *�������֣���ʶ������λ��
 *�ҵ��򷵻������ֱ��е�λ�ã����򷵻�0
 *
 *idt: Ҫ���ҵ�����
 *tx:����ǰ���ֱ�βָ��
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
 *������������
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
				error(1);                     /*��=д�����ˣ�=*/
			}
			getsymdo;
			if(sym==number)
			{
				enter(constant,ptx,lev,pdx);
				getsymdo;
			}
			else
			{
				error(2);                  /*����˵��=��Ӧ������*/
			}         
		}
		else
		{
			error(3);                       /*����˵����ʶ��Ӧ��=*/
		}
	}
	else
	{
		error(4);                        /*const��Ӧ�Ǳ�ʶ*/
	}
	return 0;
}
/*
 *������������
 */
int vardeclaration(int * ptx,int lev,int * pdx)
{
	if(sym==ident)
 	{
 		enter(variable,ptx,lev,pdx);//��д���ֱ�
 		getsymdo;
 	}
 	else
 	{
 		error(4);
    }
    return 0;
}


/*
 * ���� �ַ���������
 */
int chardeclaration(int * ptx,int lev,int * pdx)
{
 if(sym==ident)
    {
        enter(charcon,ptx,lev,pdx);//��д���ֱ�
        getsymdo;
    }
    else
    {
        error(4);
    }
    return 0;
}

/*
 * ���� ʵ����������
 */
int realdeclaration(int * ptx,int lev,int * pdx)
{
	if(sym==ident)
 	{
 		enter(realcon,ptx,lev,pdx);//��д���ֱ�
 		getsymdo;
 	}
 	else
 	{
 		error(4);
    }
    return 0;
}

 /*
  *����Ŀ������嵥
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
*��䴦��

ptx��ָ��ǰ���ű��ָ��
lev����ǰ������ڵ�Ƕ�ײ㼶
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
   	 		if((table[i].kind!=variable)&&(table[i].kind!=charcon)&&(table[i].kind!=realcon)) // �ҵ��˱�ʶ��������Ϊ����variable
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
                getsymdo; //�ֺŴ������Է���plusbecomes�Ĳ���
            }
   	 		else
   	 		{
   	 			getsymdo;
   	 			if(sym==becomes) //��ȡ��һ�����ţ������becomes���������
   	 			{
   	 				getsymdo;
					memcpy(nxtlev,fsys,sizeof(bool)* symnum); //���ݺ�����ż���
					expressiondo(nxtlev,ptx,lev); //���ʽ����
					if(i!=0)
					{
					 	gendo(sto,lev-table[i].level,table[i].adr); //���ݱ�����ַ����sto�������ʽ�洢��������
					}
   	 			}else if(sym == plusbecomes)
				{
					getsymdo;
					/*�ҵ�������ַ������ֵ��ջ*/
					gendo(lod, lev - table[i].level, table[i].adr);
					if (sym == semicolon) // +=����ֺ�
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
					getsymdo;/*�ҵ�������ַ������ֵ��ջ*/
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
   	 			// memcpy(nxtlev,fsys,sizeof(bool)* symnum); //���ݺ�����ż���
   	 			// expressiondo(nxtlev,ptx,lev); //���ʽ����
   	 			// if(i!=0)
   	 			// {
   	 			//  	gendo(sto,lev-table[i].level,table[i].adr); //���ݱ�����ַ����sto�������ʽ�洢��������
   	 			// }
			}
		}
    }
    else if(sym==readsym)  // ��������
    {
		getsymdo;
		if(sym!=lparen)  //�����Ƿ�Ϊlparen������
		{
			error(34);
		}
		else
		{
			do{
				getsymdo;
				if(sym==ident) //����Ϊident
				{
					i=position(id, *ptx); //���ұ�ʶ��λ��
				}
				else
				{
					i=0;
				}
				if(i==0)
				{
					error(35);
				}else if(table[i].kind==charcon)//���� �ַ�������
				{
					gendo(opr,0,19);
					gendo(sto,lev-table[i].level,table[i].adr);
				}
				else
				{
					gendo(opr,0,16); //����opr������
					gendo(sto,lev-table[i].level,table[i].adr);	/* ���浽����*/
				}
				getsymdo;
			}while (sym==comma);	/*һ��read���ɶ�������� */ //�������������ѭ��
		}
		if(sym!=rparen) //������
		{
			error(33);			/* ��ʽ����Ӧ��������*/
			while(!inset(sym,fsys))/* �����ȣ�ֱ���յ��ϲ㺯���ĺ������*/
			{
				getsymdo;
			}
		}
		else
		{
			getsymdo;
		}
	}
	else if(sym==writesym)		//д������ʹ���	/* ׼������write��䴦����read����*/
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
						error(11);          /*����δ�ҵ�*/
					}
				}
				memcpy(nxtlev,fsys,sizeof(bool)*symnum);
				nxtlev[rparen]=true;
				nxtlev[comma]=true;		/* write�ĺ������Ϊ��or��*/
				expressiondo(nxtlev,ptx,lev);/* ���ñ��ʽ�����˴���read��ͬ��readΪ��������ֵ*/
				if(table[i].kind==charcon) //�ַ������ 
				{   
					gendo(opr,0,17);   
				}else
				{
					gendo(opr,0,14);/* �������ָ����ջ����ֵ*/
				}
				
			}while(sym==comma);
			if(sym!=rparen)
			{
				error(33);/* write()ӦΪ�������ʽ*/
			}
			else
			{
				getsymdo;
			}
		}
		gendo(opr,0,15);		/* �������*/
	}
	//����������ʹ���
	else if(sym==callsym)		/* ׼������call��䴦��*/
	{
		getsymdo;
		if(sym!=ident)
		{
			error(14);           /*call��ӦΪ��ʶ��*/
		}
		else
		{
			i=position(id,*ptx);
			if(i==0)
			{
				error(11);          /*����δ�ҵ�*/
			}
			else
			{
				if(table[i].kind==procedur)
				{
					gendo(cal,lev-table[i].level,table[i].adr);  /*����callָ��*/
				}
				else
				{
					error(15);      /*call���ʶ��ӦΪ����*/
				}
			}
			getsymdo;
		}
	}
	else if(sym==ifsym)     /*׼������if��䴦��*/
	{	
		/******** ����������ת��ת��if true ��ִ��else����䣩 */	
		//last_sym = ifsym;  
		/******** ����������ת��ת��if true ��ִ��else����䣩 */	
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[thensym]=true;
		nxtlev[dosym]=true;    /*�������Ϊthen��do*/
		nxtlev[elsesym] = true;
		conditiondo(nxtlev,ptx,lev);   /*�������������߼����㣩����*/
		cx_if_cdt = cx-1;// �ж�pcode������������
		if(sym==thensym)
		{
			getsymdo;   //thensym��writesym
		}
		else
		{
			error(16);          /*ȱ��then*/
		}
		cx1=cx;                /*���浱ǰָ���ַ*/  //cx1��jpc��ת�����������������ת
		gendo(jpc,0,0);        /*����������תָ���ת��ַ��д0*/
		statementdo(fsys,ptx,lev);   /*����then������*/   //�ⲽ��writesym��elsesym

		//code[cx1].a=cx;      /*��statement�����cxΪthen�����ִ�����λ�ã�������ǰ��δ������ת��ַ*/
		if (sym == elsesym) {
			cx2 = cx;   //cx2:elseǰ�� jmp ֱ����ת��������� 
			gendo(jmp, 0, 0); //�Ժ��ֱ����ת��else������
			getsymdo;
			code[cx1].a = cx; //����������תָ�����Ϊ��ʱ����else������ִ��
			statementdo(fsys, ptx, lev);/*����else������*/
			code[cx2].a = cx; //��ǰָ���ַcx��else�����������λ�ã�if���ִ�к�Ҫ��ת������

			// ��֮ǰ�����������ת
			//code[cx_if_cdt].a = reverse_condition(code[cx_if_cdt].a);

		}else {
			//����������תָ�����Ϊ�ٻ�ִ����then�����ִ��
			code[cx1].a = cx;
			/* ��statement�����cxΪthen�����ִ�����λ�ã�������ǰ��δ������ת��ַ */
		}
	}
	else if(sym==beginsym)   /*׼�����ո�����䴦��*/
	{
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[semicolon]=true;
		nxtlev[endsym]=true;/*�������Ϊ�ֺŻ�end*/
		/*ѭ��������䴦������ֱ����һ�����Ų�����俪ʼ���Ż��յ�end*/
		statementdo(nxtlev,ptx,lev);
		while(inset(sym,statbegsys)||sym==semicolon)
		{
			if(sym==semicolon)
			{
				getsymdo;
			}
			else
			{
				error(10);/*ȱ�ٷֺ�*/
			}
			statementdo(nxtlev,ptx,lev);
		}
		if(sym==endsym)
		{
			getsymdo;
		}
		else
		{
			error(17); /*ȱ��end��ֺ�*/
		}
	}
	else if(sym==whilesym)/*׼������while��䴦��*/
	{
		cx1=cx;        /*�����ж�����������λ��*/
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[dosym]=true;/*�������Ϊdo*/
		conditiondo(nxtlev,ptx,lev);  /*������������*/
		cx2=cx;       /*����ѭ����Ľ�������һ��λ��*/
		gendo(jpc,0,0);/*����������ת��������ѭ���ĵ�ַδ֪*/
		if(sym==dosym)
		{
			getsymdo;
		}
		else
		{
			error(18);      /*ȱ��do*/
		}
		statementdo(fsys,ptx,lev); /*ѭ����*/
		gendo(jmp,0,cx1);/*��ͷ�����ж�����*/
		code[cx2].a=cx;   /*��������ѭ���ĵ�ַ����if����*/
	}else if (sym == forsym)
	{
		getsymdo;
		if (sym == ident)   /* ׼�����ո�ֵ��䴦�� */
		{
			i = position(id, *ptx);
			if (i == 0)
			{
				error(11);  /* ����δ�ҵ� */
			}
			else
			{
				if (table[i].kind != variable &&table[i].kind != realcon) //����int�����ж�
				{
					error(12);  /* ��ֵ����ʽ����(��߱�ʶ�����Բ��Ǳ���) */
					//i = 0;
				}
				else
				{
					getsymdo;
					if (sym == becomes) {
						getsymdo;
					}
					else {
						error(13);  /* û�м�⵽��ֵ���� */
					}
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					nxtlev[tosym]=true;                     //�����to��downto
					nxtlev[downtosym]=true;
					expressiondo(nxtlev, ptx, lev); /* ����ֵ�����Ҳ���ʽ */
					gendo(sto,lev-table[i].level,table[i].adr);     //�����ֵ
                 	if(sym==tosym)
					{
                        getsymdo;
                        cx1=cx;       //����ѭ����ʼ��
                        //��ѭ���жϱ���ȡ���ŵ�ջ��
                        gendo(lod,lev-table[i].level,table[i].adr);                                        
                        memcpy(nxtlev,fsys,sizeof(bool)*symnum);    //������ʽ
                        nxtlev[dosym]=true;                         //�����do
                        expressiondo(nxtlev,ptx,lev);
                        /*�ж�ѭ����������*/     
                        gendo(opr,0,13);             //���ɱȽ�ָ�i�Ƿ�С�ڵ�������
						cx2=cx;   
                        //����������תָ�����ѭ���������ĵ�ַδ֪
                        gendo(jpc,0,0);                     
                        if(sym==dosym)               //����ѭ����
                        {
                            getsymdo;
                            statement(fsys,ptx,lev);  //ѭ���崦��
                            //����ѭ����������Ϊ
                            //��ѭ������ȡ������ջ��
                            gendo(lod,lev-table[i].level,table[i].adr);                                       
                            gendo(lit,0,1);                            //������ȡ��ջ��
                            gendo(opr,0,2);                            //ѭ�������Ӳ���
                            //��ջ����ֵ����ѭ������
                            gendo(sto,lev-table[i].level,table[i].adr); 
                            gendo(jmp,0,cx1);                 //��������ת��ѭ����ʼ��
                            /*����ѭ��������ĵ�ַ��cxΪelse�����ִ�����λ�ã�������ǰ��δ������ת��ַ*/
                            code[cx2].a=cx;                     
                         }
                         else
                         {
                            error(29);    //for���������do
                         }
                    }else if(sym == downtosym)
					{
						getsymdo;
                        cx1=cx;       //����ѭ����ʼ��
                        //��ѭ���жϱ���ȡ���ŵ�ջ��
                        gendo(lod,lev-table[i].level,table[i].adr);                                        
                        memcpy(nxtlev,fsys,sizeof(bool)*symnum);    //������ʽ
                        nxtlev[dosym]=true;                         //�����do
                        expressiondo(nxtlev,ptx,lev);
                        /*�ж�ѭ����������*/     
                        gendo(opr,0,11);             //���ɱȽ�ָ�i�Ƿ���ڵ�������
                        cx2=cx;                      //����ѭ��������
                        //����������תָ�����ѭ���������ĵ�ַδ֪
                        gendo(jpc,0,0);                     
                        if(sym==dosym)               //����ѭ����
                        {
                            getsymdo;
                            statement(fsys,ptx,lev);  //ѭ���崦��
                            //����ѭ����������Ϊ
                            //��ѭ������ȡ������ջ��
                            gendo(lod,lev-table[i].level,table[i].adr);                                       
                            gendo(lit,0,1);                            //������ȡ��ջ��
                            gendo(opr,0,3);                            //ѭ������������
                            //��ջ����ֵ����ѭ������
                            gendo(sto,lev-table[i].level,table[i].adr); 
                            gendo(jmp,0,cx1);                 //��������ת��ѭ����ʼ��
                            /*����ѭ��������ĵ�ַ��cxΪelse�����ִ�����λ�ã�������ǰ��δ������ת��ַ*/
                            code[cx2].a=cx;                     
                        }
                        else
                        {
                            error(29);    //for���������do
                        }
					}
				}
			}
		}
	}
	else
	{
		memset(nxtlev,0,sizeof(bool)*symnum);/*�������޲��ȼ���*/
		testdo(fsys,nxtlev,19);/*�������������ȷ��*/
	}

	return 0;
}
/*
*���ʽ����
*/
int expression(bool*fsys,int*ptx,int lev)
{
	enum symbol addop;                    /*���ڱ���������*/
	bool nxtlev[symnum];
	if(sym==plus||sym==minus)             /*��ͷ�������ţ���ʱ��ǰ���ʽ������һ�����Ļ򸺵���*/
	{
		addop=sym;                    /*���濪ͷ��������*/
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;
		termdo(nxtlev,ptx,lev);                /*������*/
		if(addop==minus)
		{
			gendo(opr,0,1);                   /*�����ͷΪ��������ȡ��ָ��*/
		}
	}
	else                             /*��ʱ���ʽ��������ļӼ�*/
	{
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;
		termdo(nxtlev,ptx,lev);            /*������*/
	}
	while(sym==plus||sym==minus)
	{
		addop=sym;
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;
		termdo(nxtlev,ptx,lev);              /*������*/
		if(addop==plus)
		{
			gendo(opr,0,2);                    /*���ɼӷ�ָ��*/
		}
		else
		{
			 gendo(opr,0,3);                /*���ɼ���ָ��*/
		}
	}
	return 0;
}
/*
*���
*/
int term(bool*fsys,int *ptx,int lev)
{
    enum symbol mulop;               /*���ڱ���˳�������*/
    bool nxtlev[symnum];
    memcpy(nxtlev,fsys,sizeof(bool)*symnum) ;
    nxtlev[times]=true;
    nxtlev[slash]=true;
    factordo(nxtlev,ptx,lev);       /*��������*/
    while(sym==times||sym==slash)
    {
		mulop=sym;
        getsymdo;
        factordo(nxtlev,ptx,lev);
        if(mulop==times)
        {
            gendo(opr,0,4);          /*���ɳ˷�ָ��*/
        }
        else
        {
            gendo(opr,0,5);           /*���ɳ���ָ��*/
        }
    }
     return 0;
}
/*
*���Ӵ���
*/
int factor(bool*fsys,int *ptx,int lev)
{
    int i;
    bool nxtlev[symnum];
    testdo(facbegsys,fsys,24);           /*������ӵĿ�ʼ���ú�*/
    while(inset(sym,facbegsys))          /*ѭ��ֱ���������ӿ�ʼ����*/
    {
        if(sym==ident)                   /*����Ϊ�������߱���*/
        {
            i=position(id,*ptx);        /*��������*/
            if(i==0)
            {
                error(11);               /*��ʶ��δ����*/
            }
            else
            {
				switch(table[i].kind)
				{
					case constant:                                      /*����Ϊ����*/
						gendo(lit,0,table[i].val);                       /*ֱ�Ӱѳ�����ֵ��ջ*/
						break;
					case variable:                                      /*����Ϊ����*/
						gendo(lod,lev-table[i].level,table[i].adr);      /*�ҵ�������ַ������ֵ��ջ*/
						break;
					case procedur:                                      /*����Ϊ����*/
						error(21);                                       /*����Ϊ����*/
						break;
					case charcon:                                      /*���� ����Ϊ�ַ���*/
                        gendo(lod,lev-table[i].level,table[i].adr);      /*�ҵ��ַ��͵�ַ������ֵ��ջ*/
                        break;
					case realcon:                                      /*���� ����Ϊint*/
						gendo(lod,lev-table[i].level,table[i].adr);      /*�ҵ�int��ַ������ֵ��ջ*/
						break;
				}
			}
			getsymdo;
		}
		else
		{
			if(sym==number)                                             /*����Ϊ��*/
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
				if(sym==lparen)                                           /*����Ϊ���ʽ*/
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
						error(22);                                       /*ȱ��������*/
					}
				}
				testdo(fsys,facbegsys,23);                        /*���Ӻ��зǷ�����*/
			}
		}
	}
	return 0;
}
/*
��������*/
int condition(bool* fsys,int* ptx,int lev)
{
    enum symbol relop;
    bool nxtlev[symnum];
    if(sym==oddsym)                        /*׼������odd���㴦��*/
	{
		getsymdo;
		expressiondo(fsys,ptx,lev);
		gendo(opr,0,6);                              /*����oddָ��*/
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
}                                                  /*���ͳ���*/

void interpret1()
{
	int p,b,t;             /*ָ��ָ�룬ָ���ַ��ջ��ָ��*/
	struct instruction i;  /*��ŵ�ǰָ��*/
	int s[stacksize];      /*ջ*/
	printf("start pl0\n");
	t=0;
	b=0;
	p=0;
	s[0]=s[1]=s[2]=0;
	do{
		i=code[p];         /*����ǰָ��*/
		p++;
		switch(i.f)
		{
			case lit:        /*��a��ֵȡ��ջ��*/
				s[t]=i.a; // ��i.aѹ��ջ��
				t++;
				break;
			case opr:        /*���֡��߼�����*/
				switch(i.a)
				{
					case 0:
						t=b;
						p=s[t+2];
						b=s[t+1];
						break;
					case 1: // ȡ��
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
						printf("%d",s[t-1]);  //�޸� �ַ��ͣ������������
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
					case 17:// ���ջ��ֵ
                        printf("(Char):");
                        printf("%c\n",s[t-1]);//���ջ��ֵ
                        fprintf(fa2,"%c\n",s[t-1]);//ͬʱ��ӡ���ļ�
                        t--;//ջ������
                        break;

                    case 19://����ջ��
                        printf("(char):");
                        fprintf(fa2,"(char):");
                        getchar();//���������enter
                        scanf("%c",&(s[t]));

                        fprintf(fa2,"%c\n",s[t]);//���û�����ֵ��ӡ���ļ�
                        t++;//ջ�����ƣ�����ռ�
                        break;
				}
				break;
			case lod:       /*ȡ��Ե�ǰ���̵����ݻ���ַΪ����ڴ��ֵ��ջ��*/
				s[t]=s[base(i.l,s,b)+i.a];
				t++;
				break;
			case sto:       /*ջ����ֵ�浽��Ե�ǰ���̵����ݻ���ַΪ����ڴ�*/
				t--;
	            s[base(i.l,s,b)+i.a]=s[t];
				break;
			case cal:              /*�����ӳ���*/
				s[t]=base(i.l,s,b); /*�������̻���ַ��ջ*/
				s[t+1]=b;           /*�������̻���ַ��ջ������������base����*/
				s[t+2]=p;           /*����ǰָ��ָ����ջ*/
				b=t;                /*�ı����ַָ��ֵΪ�¹��̵Ļ���ַ*/
				p=i.a;              /*��ת*/
				break;
			case inte:             /*�����ڴ�*/
				t+=i.a;
				break;
			case jmp:             /*ֱ����ת*/
				p=i.a;
				break;
			case jpc:              /*������ת*/
				t--;
				if(s[t]==0)
				{
					p=i.a;
				}
	    		break;
		}
	}while (p!=0);
}
/*ͨ�����̻�ַ����1����̵Ļ�ַ*/
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

// ����PCode
int exchangeCode(int cx1,int cx1_end,int cx2,int cx2_end){
	
     //TODO ��cx1��cx1_end��cx2��cx2_end���л�λ��else��then�������䴦��ͻ�Ե�
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