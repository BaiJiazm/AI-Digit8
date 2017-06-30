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

//�궨����ֹ״̬
#define endState 0x07D63440

//mhd[9][9]��¼ÿ�����ֲ�ͬλ�õ������پ���
int mhd[9][9];

//���������پ���
void calcuMhd() {
	int pos, n;
	for (n = 0; n < 9; ++n)
		for (pos = 0; pos < 9; ++pos) {
			int d = abs(n - pos);
			mhd[n][pos] = d / 3 + d % 3;
		}
}

//���Է�ʽһ�������������ʼ����
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

//���Է�ʽ���������ʼ״̬
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

//����ʼĩ״̬������
int reOrderNum(unsigned int state,int flag=1) {
	int n = 0;
	unsigned int i, j, t1, pos0;
	for (pos0 = state >> 27, i = 0; i < 27; i += 3)
		if (pos0 != i / 3)
			for (t1 = state >> i & 7, j = 0; j < i; j += 3)
				if (t1 < (state >> j & 7))
					++n;
	if (flag)cout << dec << "��������" << n << endl;
	return n;
}

//�жϳ�ʼ״̬��Ŀ��״̬�Ƿ�ɽ�
bool canSolving(unsigned int initState) {
	return (reOrderNum(initState) & 1) == (reOrderNum(endState,0) & 1);
}

//����������Ĳ���
void findStep(vector<int>&step, unordered_map<int, int>&hashMap, int state) {
	int preState = hashMap[state];
	if (!preState) {
		step.push_back(state);
		return;
	}
	findStep(step, hashMap, preState);
	step.push_back(state);
}

//�����еĽڵ���Ϣ
struct queueNode {
	int state;		//״̬
	int f;			//����ֵfn
	int g;			//��ɢֵgn
	int h;			//����ֵhn
	int fa;			//��¼��չ����״̬�ĸ�״̬

	//С���ѵıȽϺ������Ѷ�Ϊfn��С
	bool operator < (const queueNode& thn)const {
		return f > thn.f;
	}
};

//A*�㷨ʵ��
void aStar(const int initState, vector<int>&step) {
	unordered_map<int, int>hashMap;		//��¼��ȷ��״̬
	priority_queue<queueNode>Q;			//���ȶ���С���ѱ������չ�ڵ�״̬
	int dir[4] = { -1,1,-3,3 };
	
	//��ʼ����ʼ�ڵ� 
	queueNode qn = { initState,0,0,0,0 };
	int i,initMhd,n1, cut, pos0, npos0, nextState;
	for (pos0 = initState >> 27, i = 0; i < 27; i += 3)
		if (i / 3 != pos0) {
			n1 = initState >> i & 7;
			qn.h += mhd[n1 + 1][i / 3];
		}
	initMhd=qn.f = qn.g + qn.h;
	Q.push(qn);
	
	//��ʼ���� 
	while (!Q.empty()) {
		qn = Q.top();	//ȡfnֵ��С��״̬�ڵ� 
		Q.pop();
		
		//�ýڵ�״̬δȷ��
		if (hashMap.count(qn.state) == 0) { 
			hashMap[qn.state] = qn.fa;		//��ϣ�����丸״̬ 
			if (qn.state == endState)		//������Ŀ��״̬ 
				break;
			
			//�ýڵ��ĸ����ܵķ��������չ
			for (pos0 = qn.state >> 27, i = 0; i < 4; ++i) {
				npos0 = pos0 + dir[i];
				
				//�ж��Ƿ������չ 
				if (npos0 >= 0 && npos0 <= 8 && (npos0 / 3 == pos0 / 3 || npos0 % 3 == pos0 % 3)) {
					n1 = qn.state >> npos0 * 3 & 7;
					nextState = (qn.state^n1 << npos0 * 3) | n1 << pos0 * 3;
					nextState ^= pos0 << 27;
					nextState |= npos0 << 27;
					
					if (hashMap.count(nextState))	//��չ����״̬δȷ��
						continue;
					cut = mhd[n1 + 1][npos0] - mhd[n1 + 1][pos0];
					Q.push({ nextState,qn.g + 1 + qn.h - cut,qn.g + 1,qn.h - cut,qn.state });
				}
			}
		}
	}
	cout << "�����پ��룺" << initMhd << endl;
	cout << "����״̬����" << dec << hashMap.size() << endl;
	findStep(step, hashMap, endState);
}

//���һ��״̬�����ڻ��ļ� 
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

//�����Ĳ��赽�ļ� 
void outPutFile(vector<int>&step) {
	ofstream fout("result.txt", ios::out);
	fout << "��Ĳ���Ϊ��" << step.size() - 1 << endl;
	for (int i = 0; i < (int)step.size(); ++i) {
		fout << "��" << i << "����" << endl;
		outState(fout, step[i]);
		fout << endl;
	}
}

bool digit8(vector<int>&step, unsigned int initState) {
	bool suc;
	cout << "��ʼ״̬��" << hex << initState << endl;
	outState(cout, initState);
	if (!(suc = canSolving(initState)))
		cout << "��ʼ״̬�޽�" << endl;
	else {
		aStar(initState, step);
		cout << "������" << step.size() - 1 << endl;
		outPutFile(step);
	}
	return suc;
}

//����display.cpp�е���ʾ���� 
void display(const vector<int>&);

int main() {
	freopen("out.txt", "w", stdout);
	vector<int>step;
	unsigned int initState = 0;
	calcuMhd();
	
	//���ַ�ʽ���������ʼ״̬,����ѡ��һ 
	inputState(initState);		//�ֶ����� 
//	generInitState(initState);	//������� 
								//��ֵ��ʽ 
//	initState=0x85DD981;		//2 0 7 5 6 4 8 3 1		�޽�
//	initState=0x205E88CE;		//7 2 4 5 0 6 8 3 1		����26����
//	initState = 0x08473947;		//8 0 6 5 4 7 2 3 1 	31����
	double start = GetTickCount();
	bool suc = digit8(step, initState);
	double end = GetTickCount();
	cout << "��ʱ��" << end - start << "ms" << endl;
	
	//�ɹ��н�����ʾ 
	if (suc) {
		cout << endl << "���������ʾ";
		getch();
		display(step);
	}
	return 0;
}
