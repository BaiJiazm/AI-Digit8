#define _CRT_SECURE_NO_WARNINGS
#include<cmath>
#include<ctime>
#include<cstdlib>
#include<iostream>
#include<fstream>
#include<vector>
#include<queue>
#include<unordered_map>
#include<conio.h>
#include<Windows.h>
using namespace std;

//宏定义终止状态
#define endState 0x07D63440

//mhd[9][9]记录每个数字不同位置的曼哈顿距离
int mhd[9][9];

//计算曼哈顿距离
void calcuMhd() {
	int pos, n;
	for (n = 0; n < 9; ++n)
		for (pos = 0; pos < 9; ++pos) {
			int d = abs(n - pos);
			mhd[n][pos] = d / 3 + d % 3;
		}
}

//测试方式一：随机化产生初始序列
void generInitState(unsigned int &initState) {
	srand((unsigned)time(NULL));
	initState = 0;
	unsigned int use = 0, pos0 = 0;
	int i;
	for (initState = 0, i = 0; i < 9;) {
		unsigned int r = (unsigned)rand() % 9;
		if (!(use & 1 << r)) {
			++i;
			use |= 1 << r;
			if (r == 0)
				pos0 = 9 - i;
			else
				--r;
			initState = initState << 3 | r;
		}
	}
	initState |= pos0 << 27;
}

//测试方式二：输入初始状态
void inputState(unsigned int &initState) {
	int i, n,pos0;
	initState = 0;
	for (i = 0; i <=8; ++i) {
		cin >> n;
		if (n)
			--n;
		else
			pos0 = i;
		initState |= n << 3 * i;
	}
	initState |= pos0 << 27;
}

//计算始末状态逆序数
int reOrderNum(unsigned int state,int flag=1) {
	int n = 0;
	unsigned int i, j, t1, pos0;
	for (pos0 = state >> 27, i = 0; i < 27; i += 3)
		if (pos0 != i / 3)
			for (t1 = state >> i & 7, j = 0; j < i; j += 3)
				if (t1 < (state >> j & 7))
					++n;
	if (flag)cout << dec << "逆序数：" << n << endl;
	return n;
}

//判断初始状态到目标状态是否可解
bool canSolving(unsigned int initState) {
	return (reOrderNum(initState) & 1) == (reOrderNum(endState,0) & 1);
}

//回溯搜索解的步骤
void findStep(vector<int>&step, unordered_map<int, int>&hashMap, int state) {
	int preState = hashMap[state];
	if (!preState) {
		step.push_back(state);
		return;
	}
	findStep(step, hashMap, preState);
	step.push_back(state);
}

//搜索中的节点信息
struct queueNode {
	int state;		//状态
	int f;			//评估值fn
	int g;			//耗散值gn
	int h;			//启发值hn
	int fa;			//记录扩展到该状态的父状态

	//小顶堆的比较函数，堆顶为fn最小
	bool operator < (const queueNode& thn)const {
		return f > thn.f;
	}
};

//A*算法实现
void aStar(const int initState, vector<int>&step) {
	unordered_map<int, int>hashMap;		//记录已确定状态
	priority_queue<queueNode>Q;			//优先队列小顶堆保存待扩展节点状态
	int dir[4] = { -1,1,-3,3 };
	
	//初始化初始节点 
	queueNode qn = { initState,0,0,0,0 };
	int i,initMhd,n1, cut, pos0, npos0, nextState;
	for (pos0 = initState >> 27, i = 0; i < 27; i += 3)
		if (i / 3 != pos0) {
			n1 = initState >> i & 7;
			qn.h += mhd[n1 + 1][i / 3];
		}
	initMhd=qn.f = qn.g + qn.h;
	Q.push(qn);
	
	//开始搜索 
	while (!Q.empty()) {
		qn = Q.top();	//取fn值最小的状态节点 
		Q.pop();
		
		//该节点状态未确定
		if (hashMap.count(qn.state) == 0) { 
			hashMap[qn.state] = qn.fa;		//哈希表保存其父状态 
			if (qn.state == endState)		//搜索到目标状态 
				break;
			
			//该节点四个可能的方向进行扩展
			for (pos0 = qn.state >> 27, i = 0; i < 4; ++i) {
				npos0 = pos0 + dir[i];
				
				//判断是否可以扩展 
				if (npos0 >= 0 && npos0 <= 8 && (npos0 / 3 == pos0 / 3 || npos0 % 3 == pos0 % 3)) {
					n1 = qn.state >> npos0 * 3 & 7;
					nextState = (qn.state^n1 << npos0 * 3) | n1 << pos0 * 3;
					nextState ^= pos0 << 27;
					nextState |= npos0 << 27;
					
					if (hashMap.count(nextState))	//扩展的新状态未确定
						continue;
					cut = mhd[n1 + 1][npos0] - mhd[n1 + 1][pos0];
					Q.push({ nextState,qn.g + 1 + qn.h - cut,qn.g + 1,qn.h - cut,qn.state });
				}
			}
		}
	}
	cout << "曼哈顿距离：" << initMhd << endl;
	cout << "搜索状态数：" << dec << hashMap.size() << endl;
	findStep(step, hashMap, endState);
}

//输出一个状态到窗口或文件 
void outState(ostream &fout, int s) {
	int pos0, n1, j;
	for (pos0 = s >> 27, j = 0; j < 9; ++j) {
		n1 = s >> j * 3 & 7;
		if (j == pos0)
			fout << "  ";
		else
			fout << n1 + 1 << " ";
		if (j % 3 == 2)
			fout << endl;
	}
	fout << endl;
}

//输出解的步骤到文件 
void outPutFile(vector<int>&step) {
	ofstream fout("result.txt", ios::out);
	fout << "解的步数为：" << step.size() - 1 << endl;
	for (int i = 0; i < (int)step.size(); ++i) {
		fout << "第" << i << "步：" << endl;
		outState(fout, step[i]);
		fout << endl;
	}
}

bool digit8(vector<int>&step, unsigned int initState) {
	bool suc;
	cout << "初始状态：" << hex << initState << endl;
	outState(cout, initState);
	if (!(suc = canSolving(initState)))
		cout << "初始状态无解" << endl;
	else {
		aStar(initState, step);
		cout << "步数：" << step.size() - 1 << endl;
		outPutFile(step);
	}
	return suc;
}

//调用display.cpp中的演示函数 
void display(const vector<int>&);

int main() {
	freopen("out.txt", "w", stdout);
	vector<int>step;
	unsigned int initState = 0;
	calcuMhd();
	
	//三种方式进行输入初始状态,可任选其一 
	inputState(initState);		//手动输入 
//	generInitState(initState);	//随机产生 
								//赋值方式 
//	initState=0x85DD981;		//2 0 7 5 6 4 8 3 1		无解
//	initState=0x205E88CE;		//7 2 4 5 0 6 8 3 1		书上26步解
//	initState = 0x08473947;		//8 0 6 5 4 7 2 3 1 	31步解
	double start = GetTickCount();
	bool suc = digit8(step, initState);
	double end = GetTickCount();
	cout << "用时：" << end - start << "ms" << endl;
	
	//成功有解则演示 
	if (suc) {
		cout << endl << "按任意键演示";
		getch();
		display(step);
	}
	return 0;
}
