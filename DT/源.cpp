#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <map>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <set>
#include <functional>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <queue>


using namespace std;

const int ID3 = 0;
const int C45 = 1;
const int GINI = 2;

int SELECT = GINI;

const int MAX_ROW = 1000;
const int MAX_COLUMN = 10;

int train_row_data[MAX_ROW][MAX_COLUMN];
int val_row_data[MAX_ROW][MAX_COLUMN];
int test_row_data[MAX_ROW][MAX_COLUMN];
int Column = 0;
vector<int*> RAW_DATA;
//const int rol_value[9] = { 4,4,4,13,2,2,4,4,2 };//每一个属性的分叉数
const int rol_value[9] = { 4,4,4,3,3,2,2,4,4 };
const int attribute_0[5] = { 10,20,30,40,50 };

//将矩阵数据还换成向量保存，方便划分数据集
void convert(int data[MAX_ROW][MAX_COLUMN], vector<int*> &todata, int row)//要传值引用
{
	for (int i = 0; i < row; i++)
	{
		int* tmp = new int[row + 1];
		for (int j = 0; j < Column; j++)
		{
			tmp[j] = data[i][j];
		}
		todata.push_back(tmp);
	}
}
//读入数据集
pair<int, int> Text_Handle(string Path, int data[MAX_ROW][MAX_COLUMN])
{
	stringstream line;
	string temp_line;
	ifstream Semeval(Path, std::ios::in);
	char temp_notation;
	int row = 0, column = 0;

	if (Semeval)
	{
		while (getline(Semeval, temp_line))
		{
			line.str("");//!!!!!!!!
			line.clear();
			line << temp_line;
			column = 0;
			while (line >> data[row][column])
			{
				if (line)
					line >> temp_notation;
				else
					break;
				column++;
			}
			row++;
		}
	}
	else
	{
		cout << "No file!" << endl;
	}
	Semeval.close();

	return pair<int, int>(row, column);
}
//计算ID3、C45、GINI
int ID3_C45_GINI(int row,const vector<int*> &data, const set<int> col, int Select)
{
	int label = 0;
	int d_num = 0;
	int ID3_data[9][13][2];//维度分别对应哪一个属性、某一个属性中的哪个值、某一个值中的1/0数量
	memset(ID3_data, 0, sizeof(ID3_data));
	for (int i = 0; i<row; i++)
	{
		label = data[i][Column - 1] == 1 ? 1 : 0;
		d_num += label;
		//第一列属性由于分支过多，做离散化处理，简单地划分为区间10-20，20-30，30-40，40-50（视为课上年龄属性）
		if (data[i][0] <= 20)//第一个属性的分叉方法
			ID3_data[0][0][label]++;
		else if (data[i][0] <= 30)
			ID3_data[0][1][label]++;
		else if (data[i][0] <= 40)
			ID3_data[0][2][label]++;
		else
			ID3_data[0][3][label]++;

		for (int j = 1; j < Column; j++)//统计每一个属性下的分叉对应的标签
			ID3_data[j][data[i][j]][label]++;
	}
	//熵
	double avoid_nan_1 = 0.0, avoid_nan_2 = 0.0;//预判标签数是不是零，避免计算nan
	avoid_nan_1 = d_num == 0 ? 0 : (-(1.0 * d_num / row) * log2(1.0 * d_num / row));
	avoid_nan_2 = d_num == row ? 0 : (-(1.0 * (row - d_num) / row) * log2(1.0 * (row - d_num) / row));
	double hd = avoid_nan_1 + avoid_nan_2;

	if (hd == 0.0)
		return -1;

	//计算条件熵
	int tmp = 0;
	double min_hda = 10.0, hda = 0.0;//最小条件熵
	int index_hda = -1;//最小条件熵的下标，对应了属性列值
	avoid_nan_1 = 0.0, avoid_nan_2 = 0.0;
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	double C45 = 0.0;
	double max_c45 = 0.0;
	int index_c45 = -1;
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	double gini = 0.0;
	double min_gini = 100.0;
	int index_gini = -1;


	for (int i = 0; i < Column - 1; i++)
	{
		hda = 0.0;
		////////////////////////////////////////////////////////////////////////////////////////////////////
		C45 = 0.0;
		////////////////////////////////////////////////////////////////////////////////////////////////////
		gini = 0.0;

		if (col.find(i) == col.end())//如果某一列属性并不在输入的属性集合里面，说明该列已经被挑选了
			continue;
		for (int j = 0; j<rol_value[i]; j++)
		{
			tmp = ID3_data[i][j][0] + ID3_data[i][j][1];//某个属性之下某一个值的总数
			///////////////////////////////////////////////////////////////////////////////////////////////
			//预先判断计算公式的分母是否为零
			//信息增益率的计算公式
			avoid_nan_1 = tmp == 0 ? 0 : (-1.0*tmp / row*log(1.0*tmp / row));
			C45 += avoid_nan_1;
			///////////////////////////////////////////////////////////////////////////////////////////////
			//gini系数的计算公式
			gini += tmp != 0 ? 1.0*tmp / row*(1.0 - (1.0*ID3_data[i][j][0] / tmp)*(1.0*ID3_data[i][j][0] / tmp) - (1.0*ID3_data[i][j][1] / tmp)*(1.0*ID3_data[i][j][1] / tmp)) : 0;
			//信息增益中条件熵的计算公式
			avoid_nan_1 = ID3_data[i][j][0] == 0 ? 0 : (-1.0 * ID3_data[i][j][0] * log2(1.0 * ID3_data[i][j][0] / tmp));
			avoid_nan_2 = ID3_data[i][j][1] == 0 ? 0 : (-1.0 * ID3_data[i][j][1] * log2(1.0 * ID3_data[i][j][1] / tmp));
			//hda += 1.0 * tmp / row * (-1.0 * ID3_data[i][j][0] / tmp * log2(1.0 * ID3_data[i][j][0] / tmp) - 1.0 * ID3_data[i][j][1] / tmp * log2(1.0 * ID3_data[i][j][1] / tmp));
			hda += 1.0 / row * (avoid_nan_1 + avoid_nan_2);
		}
		//cout << hda << endl;
		//选择条件熵最小的，也就是信息增益最大的
		if (hda < min_hda)
		{
			min_hda = hda;
			index_hda = i;
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////
		C45 = C45 == 0 ? 0.001 : C45;
		double temp = 1.0*(hd - hda) / C45;
		if (max_c45 < temp)
		{
			max_c45 = temp;
			index_c45 = i;
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////
		if (min_gini > gini)
		{
			min_gini = gini;
			index_gini = i;
		}
		cout << hd - hda << " " << temp << " " << gini << endl;
	}
	cout << "Select:" << index_hda << " " << index_c45 << " " << index_gini << endl << endl;
	switch (Select)
	{
	case 0:return index_hda; break;
	case 1:return index_c45; break;
	case 2:return index_gini; break;
	default:return index_hda; break;
	}
}
//树节点
struct node
{
	bool if_end;
	int attribute;//属性
	int value_attribute;//属性值
	int result;//该节点可以用来判断的结果，就是以此为根，标签的众数，0/1
	int floor;//层数
	vector<node*> child;//子节点
	node()
	{
		if_end = false;
		attribute = 0;
		value_attribute = result = floor = -1;
	}
};

//判断是否到达递归边界，以数据集和剩余属性量判断
bool meet_bound(vector<int*> data, set<int> col)
{
	if (data.empty() || col.size() <= 1)
		return true;
	return false;
}
//根据属性值划分数据集
vector<vector<int*>> divide_set(vector<int*> data, int attri)
{
	vector<vector<int*>> Set;//需要返回多个数据集，用vector保存
	for (int j = 0; j < rol_value[attri]; j++)
	{
		vector<int*> tmp;
		tmp.clear();
		//按照属性列中不同属性值划分数据集
		for (int i = 0; i < data.size(); i++)
		{
			if (attri == 0)//第一列做离散化处理
			{
				if (attribute_0[j] < data[i][attri] && data[i][attri] <= attribute_0[j + 1])
					tmp.push_back(data[i]);
			}
			else
			{
				if (data[i][attri] == j)
					tmp.push_back(data[i]);
			}
		}
		Set.push_back(tmp);
	}
	return Set;
}
//建树
void build_tree(node* p, const vector<int*> &data, const set<int> col)
{
	if (meet_bound(data,col))
	{
		int attri[13][2];
		memset(attri, 0, sizeof(attri));
		int attribute = *(col.begin());//默认选择剩余属性列中的第一个
		int label = 0;
		//统计剩余数据集中的标签众数
		for (int i = 0; i < data.size(); i++)
		{
			label = data[i][Column - 1] == 1 ? 1 : 0;
			if (attribute == 0)
			{
				if (data[i][0] <= 20)//第一个属性的分叉方法
					attri[0][label]++;
				else if (data[i][0] <= 30)
					attri[1][label]++;
				else if (data[i][0] <= 40)
					attri[2][label]++;
				else
					attri[3][label]++;
			}
			else
				attri[data[i][attribute]][label]++;
		}
		//建立分叉节点作为叶子节点
		for (int i = 0; i < rol_value[attribute]; i++)
		{
			node* child = new node;
			child->floor = p->floor + 1;
			child->attribute = attribute;
			if (attribute == 0)
				child->value_attribute = attribute_0[i];
			else
				child->value_attribute = i;
			child->result = attri[i][1] >= attri[i][0] ? 1 : 0;
			child->if_end = true;
			p->child.push_back(child);
		}
		return;
	}
	//通过计算ID3/C4.5/Gini，选定属性列
	int select_attri = ID3_C45_GINI(data.size(), data, col, SELECT);

	//边界判定条件中的标签全部一致，此时返回选中列为-1
	if (select_attri == -1)
	{
		p->result = data[0][Column - 1] == 1 ? 1 : 0;
		return;
	}
	//统计属性列中的标签众数
	int Most_1 = 0;
	for (int i = 0; i < data.size(); i++)
	{
		Most_1 += data[i][Column - 1] == 1 ? 1 : 0;
	}
	if (Most_1 > (data.size() - Most_1))
		p->result = 1;
	else
		p->result = 0;

	vector<vector<int*>> data_set;
	set<int> new_col;
	new_col = col;
	new_col.erase(select_attri);//删除选中列
	data_set = divide_set(data, select_attri);//划分数据集
	//建立分支节点，递归建树
	for (int i = 0; i < rol_value[select_attri]; i++)
	{
		if (data_set[i].size() == 0)
			continue;
		node* child = new node;
		child->floor = p->floor + 1;
		child->attribute = select_attri;
		if (select_attri == 0)
			child->value_attribute = attribute_0[i + 1];
		else
			child->value_attribute = i;
		p->child.push_back(child);
		vector<int*> tmp = data_set[i];
		build_tree(child, data_set[i], new_col);
	}
}
//BFS遍历树，用来查看小数据集建树结果
void BFS(node* p)
{
	queue<node*> bfs;
	bfs.push(p);
	int T = 0, F = 0;//判断1，0的叶子数
	while (!bfs.empty())
	{
		node* tmp = bfs.front();
		bfs.pop();
		if (tmp->child.empty())
			if (tmp->result == 1)
				T++;
			else if (tmp->result == 0)
				F++;
		for (int i = 0; i < tmp->child.size(); i++)
		{
			/*cout << "Floor: " << tmp->child[i]->floor << " attribute: " << tmp->child[i]->attribute;
			cout << " Value attribute: " << tmp->child[i]->value_attribute;
			cout << " Result: " << tmp->child[i]->result << endl;*/
			
			bfs.push(tmp->child[i]);
		}
	}
	cout << "For 1 leaves and 0 leaves: ";
	cout << T << " " << F << endl;
}
//剪枝，针对那些只有一个分支的节点
void cut_only_one(node* p)
{
	if (p->child.size() == 1)
	{
		delete p->child[0];
		return;
	}
	for (int i = 0; i < p->child.size(); i++)
	{
		cut_only_one(p->child[0]);
	}
}
//预测数据时候的递归过程
int recursive(node* p, const int val[MAX_COLUMN])
{
	if (p->child.empty() || p->if_end)
	{
		//cout << p->result << endl;
		return p->result;
	}
	for (int i = 0; i < p->child.size(); i++)
	{
		if (p->child[i]->attribute != 0 && p->child[i]->value_attribute == val[p->child[i]->attribute])
			return recursive(p->child[i], val);
		else if(p->child[i]->attribute == 0 && (val[p->child[i]->attribute] <= p->child[i]->value_attribute && val[p->child[i]->attribute] > p->child[i]->value_attribute - 10))
		{
			return recursive(p->child[i], val);
		}
		//if(p->child[i]->value_attribute == val[p->child[i]->attribute])
		//	return recursive(p->child[i], val);
	}
	
	return p->result;
}
//预测数据
double Predict(node* p, const int val[MAX_ROW][MAX_COLUMN], int row, bool flag = false, bool output = false)
{
	int result = -1;
	int TP = 0, FP = 0, TN = 0, FN = 0, Nan = 0;
	fstream outfile("D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\result.txt", ios::out);
	for (int i = 0; i < row; i++)
	{
		result = recursive(p, val[i]);
		cout << (result == 1 ? 1 : -1) << endl;
		if (flag)
			outfile << (result == 1 ? 1 : -1) << endl;
		if (result == 1 && val[i][Column - 1] == 1)
			TP++;
		else if (result == 0 && val[i][Column - 1] == 1)
			FN++;
		else if (result == 0 && val[i][Column - 1] == -1)
			TN++;
		else if (result == 1 && val[i][Column - 1] == -1)
			FP++;
		else
			Nan++;
	}
	outfile.close();
	double accuracy = 0.0, recall = 0.0, precision = 0.0, f1 = 0.0;
	accuracy = 1.0*(TP + TN) / (TP + FP + TN + FN);
	recall = 1.0*TP / (TP + FN);
	precision = 1.0*TP / (TP + FP);
	f1 = 2.0*precision*recall / (precision + recall);
	if (output)
	{
		cout << accuracy << " " << recall << " ";
		cout << precision << " " << f1 << endl;
	}
	return accuracy;
}

//回收节点
void revoke(node* p)
{
	if (p->child.empty())
	{
		delete p;
		return;
	}
	for (int i = 0; i < p->child.size(); i++)
	{
		revoke(p->child[i]);
	}
	delete p;
}

//自底向上后剪枝
void hinder_cut(node* p,node* root, const int data[MAX_ROW][MAX_COLUMN], int row)
{
	if (p->child.empty())
		return;
	for (int i = 0; i < p->child.size(); i++)
	{
		hinder_cut(p->child[i], root, data, row);//先递归找到最底层节点
		double before_acc = Predict(root, data, row);//计算当前准确率
		p->child[i]->if_end = true;//假设当前节点为叶子节点
		double hinder_acc = Predict(root, data, row);//计算假设后的准确率
		if (before_acc - hinder_acc < 0.1)//阈值设置为准确率下降0.1
		{
			revoke(p->child[i]);//删除子树
			p->child.erase(p->child.begin() + i);
		}
		else
			p->if_end = false;
	}
}

//回收数据空间
void delete_data(vector<int*> &data)
{
	for (int i = data.size() - 1; i >= 0; i--)
		delete data[i];
}

int main()
{
	string small = "D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\small.csv";
	string train = "D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\train.csv";
	string test = "D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\test.csv";

	string two_two_train = "D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\二比二\\train.csv";
	string two_two_validation = "D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\二比二\\train.csv";

	string four_one_train = "D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\四比一\\train.csv";
	string four_one_val = "D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\四比一\\validation.csv";

	string yanshou_train = "D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\yanshou\\YStrain3.csv";
	string yanshou_test = "D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\yanshou\\YStest3.csv";

	SELECT = ID3;

	int row = 0;
	pair<int, int> temp;

	temp = Text_Handle(yanshou_train, train_row_data);
	row = temp.first;
	Column = temp.second;
	convert(train_row_data, RAW_DATA, row);

	//for (int i = 0; i < RAW_DATA.size(); i++)
	//{
	//	for (int j = 0; j < Column; j++)
	//		cout << RAW_DATA[i][j] << " ";
	//	cout << endl;
	//}

	set<int> col;
	for (int i = 1; i < Column - 1; i++)
		col.insert(i);

	node* root = new node;
	root->floor = 0;
	
	build_tree(root, RAW_DATA, col);

	//BFS(root);

	//cut_only_one(root);//剪枝

	//cout << "Before hinder cut: " << endl;
	//BFS(root);

	//temp = Text_Handle(four_one_val, val_row_data);
	//int val_row = 0;
	//val_row = temp.first;
	//cout << Predict(root, val_row_data, val_row, false, true) << endl;//预测

	//hinder_cut(root, root, val_row_data, val_row);

	//cout << "After hinder cut: " << endl;
	//BFS(root);
	//cout << Predict(root, val_row_data, val_row, false, true) << endl;//预测

	temp = Text_Handle(yanshou_test, test_row_data);
	int test_row = 0;
	test_row = temp.first;

	//for (int i = 0; i < test_row; i++)
	//{
	//	for (int j = 0; j < Column; j++)
	//		cout << test_row_data[i][j] << " ";
	//	cout << endl;
	//}

	cout << Predict(root, test_row_data, test_row) << endl;//预测


	revoke(root);//回收树节点所占内存
	delete_data(RAW_DATA);//回收数据集占有的内存
	system("pause");
}