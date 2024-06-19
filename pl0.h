                         /*PL/0 编译系统C版本头文件 pl0.h*/
                     
# define norw 19                 /*关键字个数*/
# define txmax 100               /*名字表容量*/
# define nmax  14                /*number的最大位数*/
# define al 10                   /*符号的最大长度*/
# define amax 2047               /*地址上界*/
# define levmax 3                /*最大允许过程嵌套声明层数[0，lexmax]*/
# define cxmax 200               /*最多的虚拟机代码数*/
/*符号*/
enum symbol{
	 nul,    ident/*标识符*/,     number,     plus,     minus,
	 times,   slash,   oddsym,   eql,     neq,
lss,     leq,      gtr,     geq,     lparen,
rparen,  comma,   semicolon,period,  becomes,
beginsym, endsym, ifsym,    thensym,  whilesym,
writesym, readsym,  dosym,  callsym,  constsym,
varsym,  procsym, 
//新增
elsesym,not1,plusbecomes,plusdouble,minusbecomes,forsym,tosym,downtosym,

// 选做
charsym,intsym
};
#define symnum 41 //符号总数目，和上面的symbol数量对应
/*-------------*/
enum object{
   constant,
   variable,
   procedur,
   charcon,//新增字符型
   realcon,//新增实数型
};
/*操作码（指令类型）--------------*/
enum fct{
lit, opr,  lod,  sto,  cal,  inte,  jmp,  jpc,
};
#define fctnum 8 //操作码（指令类型）的总数目，和上面的fct数量对应
/*f 为操作码，l 和 a 分别为层次差和地址或操作数--------------*/
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

bool tableswitch; //是否打印名字表
bool listswitch;  //是否打印虚拟机代码
char ch; //当前读取的字符
enum symbol sym;
enum symbol last_sym;
char id[al+1]; //存储标识符 ，存储词法分析阶段识别出的标识符，例如变量名、过程名等
int  num;
int cc; //当前字符在行缓冲区中的位置
int ll; //当前行长度
int cx; //当前指令地址
char line[81]; //行缓冲区，存储当前行的字符
char a[al+1]; //用于暂存在词法分析阶段识别出的当前字符或符号
struct instruction code[cxmax]; //存储生成的虚拟机代码
char word[norw][al]; //关键字 有序表
enum symbol wsym[norw]; // 关键字映射（映射到nul啊，ident什么的）
enum symbol ssym[256]; // 单字符符号映射
char mnemonic[fctnum][5]; //操作码数组

//根据index（即symnum定位）如declbegsys[1]=1则说明存在opr
bool declbegsys[symnum]; // 声明起始符号集合
bool statbegsys[symnum]; // 语句起始符号集合
bool facbegsys[symnum]; // 因子起始符号集合
/*------------------------------*/




struct tablestruct
{
   char name[al];                            /*名字*/
   enum object kind;                         /*类型：const，var，array or procedure*/
   int val;                                  /*数值，仅const使用*/
   int level;                                /*所处层，仅const不使用*/
   int adr;                                  /*地址，仅const不使用*/
   int size;                                 /*需要分配的数据区空间，仅procedure使用*/
};
struct tablestruct table[txmax];             /*名字（标识符）表，结构体数组*/
FILE* fin;
FILE* fout;
char fname[al];
int err;                                       /*错误计数器*/
/*当函数中会发生fatal error时，返回－1告知调用它的函数，最终退出程序*/
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
#define chardeclarationdo(a,b,c)              if(-1==chardeclaration(a,b,c))return -1  //新增 字符型
#define realdeclarationdo(a,b,c)               if(-1==realdeclaration(a,b,c))return -1

void error(int n);
int getsym(); // 词法分析，获取一个符号 
int getch();   // 
void init();   //初始化
int gen(enum fct x,int y,int z);
int test(bool*s1,bool*s2,int n); 
int inset(int e,bool*s);
int addset(bool*sr,bool*s1,bool*s2,int n);
int subset(bool*sr,bool*s1,bool*s2,int n);
int mulset(bool*sr,bool*s1,bool*s2,int n);
int block(int lev,int tx,bool* fsys);
void interpret1(); // 解释执行生成的目标代码
int factor(bool* fsys,int* ptx,int lev);
int term(bool*fsys,int*ptx,int lev);
int condition(bool*fsys,int*ptx,int lev);
int expression(bool*fsys,int*ptx,int lev);
int statement(bool*fsys,int*ptx,int lev);
void listcode(int cx0);
int vardeclaration(int* ptx,int lev, int* pdx);
int constdeclaration(int* ptx,int lev, int* pdx);
int chardeclaration(int * ptx,int lev,int * pdx); //新增字符型声明
int realdeclaration(int * ptx,int lev,int * pdx); //新增int型声明
int position(char* idt,int tx);
void enter(enum object k,int* ptx,int lev,int* pdx);
int base(int l,int* s,int b);

int reverse_condition(int a);

/*
输入源代码
   ↓
初始化
   ↓
获取下一个符号 (getsym)
   ↓
进入主程序块 (block)
   ↓
   ├─> 分析常量声明 (constdeclaration)
   │      ↓
   │      └─> 加入名字表 (enter)
   │
   ├─> 分析变量声明 (vardeclaration)
   │      ↓
   │      └─> 加入名字表 (enter)
   │
   └─> 分析语句 (statement)
          ↓
          ├─> 分析赋值语句 (statement)
          │      ↓
          │      ├─> 获取表达式 (expression)
          │      │      ↓
          │      │      ├─> 处理项 (term)
          │      │      │      ↓
          │      │      │      └─> 处理因子 (factor)
          │      │      │             ↓
          │      │      │             └─> 获取下一个符号 (getsym)
          │      │      │
          │      │      └─> 获取下一个符号 (getsym)
          │      │
          │      └─> 生成存储指令 (gen)
          │
          └─> 分析条件语句 (statement)
                 ↓
                 ├─> 分析条件 (condition)
                 │      ↓
                 │      ├─> 获取表达式 (expression)
                 │      │      ↓
                 │      │      ├─> 处理项 (term)
                 │      │      │      ↓
                 │      │      │      └─> 处理因子 (factor)
                 │      │      │             ↓
                 │      │      │             └─> 获取下一个符号 (getsym)
                 │      │      │
                 │      │      └─> 获取下一个符号 (getsym)
                 │      │
                 │      └─> 生成条件判断指令 (gen)
                 │
                 └─> 分析语句 (statement)
                        ↓
                        └─> 生成跳转指令 (gen)

生成目标代码
   ↓
解释执行 (interpret)
   ↓
输出结果
*/






