                         /*PL/0 ����ϵͳC�汾ͷ�ļ� pl0.h*/
                     
# define norw 19                 /*�ؼ��ָ���*/
# define txmax 100               /*���ֱ�����*/
# define nmax  14                /*number�����λ��*/
# define al 10                   /*���ŵ���󳤶�*/
# define amax 2047               /*��ַ�Ͻ�*/
# define levmax 3                /*�����������Ƕ����������[0��lexmax]*/
# define cxmax 200               /*���������������*/
/*����*/
enum symbol{
	 nul,    ident/*��ʶ��*/,     number,     plus,     minus,
	 times,   slash,   oddsym,   eql,     neq,
lss,     leq,      gtr,     geq,     lparen,
rparen,  comma,   semicolon,period,  becomes,
beginsym, endsym, ifsym,    thensym,  whilesym,
writesym, readsym,  dosym,  callsym,  constsym,
varsym,  procsym, 
//����
elsesym,not1,plusbecomes,plusdouble,minusbecomes,forsym,tosym,downtosym,

// ѡ��
charsym,intsym
};
#define symnum 41 //��������Ŀ���������symbol������Ӧ
/*-------------*/
enum object{
   constant,
   variable,
   procedur,
   charcon,//�����ַ���
   realcon,//����ʵ����
};
/*�����루ָ�����ͣ�--------------*/
enum fct{
lit, opr,  lod,  sto,  cal,  inte,  jmp,  jpc,
};
#define fctnum 8 //�����루ָ�����ͣ�������Ŀ���������fct������Ӧ
/*f Ϊ�����룬l �� a �ֱ�Ϊ��β�͵�ַ�������--------------*/
struct instruction
{
  enum fct f;
  int l;
  int a;
};

FILE * fas;
FILE * fa;
FILE * fa1;
FILE * fa2;

bool tableswitch; //�Ƿ��ӡ���ֱ�
bool listswitch;  //�Ƿ��ӡ���������
char ch; //��ǰ��ȡ���ַ�
enum symbol sym;
enum symbol last_sym;
char id[al+1]; //�洢��ʶ�� ���洢�ʷ������׶�ʶ����ı�ʶ�����������������������
int  num;
int cc; //��ǰ�ַ����л������е�λ��
int ll; //��ǰ�г���
int cx; //��ǰָ���ַ
char line[81]; //�л��������洢��ǰ�е��ַ�
char a[al+1]; //�����ݴ��ڴʷ������׶�ʶ����ĵ�ǰ�ַ������
struct instruction code[cxmax]; //�洢���ɵ����������
char word[norw][al]; //�ؼ��� �����
enum symbol wsym[norw]; // �ؼ���ӳ�䣨ӳ�䵽nul����identʲô�ģ�
enum symbol ssym[256]; // ���ַ�����ӳ��
char mnemonic[fctnum][5]; //����������

//����index����symnum��λ����declbegsys[1]=1��˵������opr
bool declbegsys[symnum]; // ������ʼ���ż���
bool statbegsys[symnum]; // �����ʼ���ż���
bool facbegsys[symnum]; // ������ʼ���ż���
/*------------------------------*/




struct tablestruct
{
   char name[al];                            /*����*/
   enum object kind;                         /*���ͣ�const��var��array or procedure*/
   int val;                                  /*��ֵ����constʹ��*/
   int level;                                /*�����㣬��const��ʹ��*/
   int adr;                                  /*��ַ����const��ʹ��*/
   int size;                                 /*��Ҫ������������ռ䣬��procedureʹ��*/
};
struct tablestruct table[txmax];             /*���֣���ʶ���������ṹ������*/
FILE* fin;
FILE* fout;
char fname[al];
int err;                                       /*���������*/
/*�������лᷢ��fatal errorʱ�����أ�1��֪�������ĺ����������˳�����*/
#define getsymdo                              if(-1==getsym())return -1
#define getchdo                               if(-1==getch())return -1
#define testdo(a,b,c)                         if(-1==test(a,b,c))return -1
#define gendo(a,b,c)                          if(-1==gen(a,b,c))return -1
#define expressiondo(a,b,c)                   if(-1==expression(a,b,c))return -1
#define factordo(a,b,c)                       if(-1==factor(a,b,c))return -1
#define termdo(a,b,c)                         if(-1==term(a,b,c))return -1
#define conditiondo(a,b,c)                    if(-1==condition(a,b,c))return -1
#define statementdo(a,b,c)                    if(-1==statement(a,b,c))return -1
#define constdeclarationdo(a,b,c)             if(-1==constdeclaration(a,b,c))return -1
#define vardeclarationdo(a,b,c)               if(-1==vardeclaration(a,b,c))return -1
#define chardeclarationdo(a,b,c)              if(-1==chardeclaration(a,b,c))return -1  //���� �ַ���
#define realdeclarationdo(a,b,c)               if(-1==realdeclaration(a,b,c))return -1

void error(int n);
int getsym(); // �ʷ���������ȡһ������ 
int getch();   // 
void init();   //��ʼ��
int gen(enum fct x,int y,int z);
int test(bool*s1,bool*s2,int n); 
int inset(int e,bool*s);
int addset(bool*sr,bool*s1,bool*s2,int n);
int subset(bool*sr,bool*s1,bool*s2,int n);
int mulset(bool*sr,bool*s1,bool*s2,int n);
int block(int lev,int tx,bool* fsys);
void interpret1(); // ����ִ�����ɵ�Ŀ�����
int factor(bool* fsys,int* ptx,int lev);
int term(bool*fsys,int*ptx,int lev);
int condition(bool*fsys,int*ptx,int lev);
int expression(bool*fsys,int*ptx,int lev);
int statement(bool*fsys,int*ptx,int lev);
void listcode(int cx0);
int vardeclaration(int* ptx,int lev, int* pdx);
int constdeclaration(int* ptx,int lev, int* pdx);
int chardeclaration(int * ptx,int lev,int * pdx); //�����ַ�������
int realdeclaration(int * ptx,int lev,int * pdx); //����int������
int position(char* idt,int tx);
void enter(enum object k,int* ptx,int lev,int* pdx);
int base(int l,int* s,int b);

int reverse_condition(int a);

/*
����Դ����
   ��
��ʼ��
   ��
��ȡ��һ������ (getsym)
   ��
����������� (block)
   ��
   ����> ������������ (constdeclaration)
   ��      ��
   ��      ����> �������ֱ� (enter)
   ��
   ����> ������������ (vardeclaration)
   ��      ��
   ��      ����> �������ֱ� (enter)
   ��
   ����> ������� (statement)
          ��
          ����> ������ֵ��� (statement)
          ��      ��
          ��      ����> ��ȡ����ʽ (expression)
          ��      ��      ��
          ��      ��      ����> ������ (term)
          ��      ��      ��      ��
          ��      ��      ��      ����> �������� (factor)
          ��      ��      ��             ��
          ��      ��      ��             ����> ��ȡ��һ������ (getsym)
          ��      ��      ��
          ��      ��      ����> ��ȡ��һ������ (getsym)
          ��      ��
          ��      ����> ���ɴ洢ָ�� (gen)
          ��
          ����> ����������� (statement)
                 ��
                 ����> �������� (condition)
                 ��      ��
                 ��      ����> ��ȡ����ʽ (expression)
                 ��      ��      ��
                 ��      ��      ����> ������ (term)
                 ��      ��      ��      ��
                 ��      ��      ��      ����> �������� (factor)
                 ��      ��      ��             ��
                 ��      ��      ��             ����> ��ȡ��һ������ (getsym)
                 ��      ��      ��
                 ��      ��      ����> ��ȡ��һ������ (getsym)
                 ��      ��
                 ��      ����> ���������ж�ָ�� (gen)
                 ��
                 ����> ������� (statement)
                        ��
                        ����> ������תָ�� (gen)

����Ŀ�����
   ��
����ִ�� (interpret)
   ��
������
*/





