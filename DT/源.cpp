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
//const int rol_value[9] = { 4,4,4,13,2,2,4,4,2 };//ÿһ�����Եķֲ���
const int rol_value[9] = { 4,4,4,3,3,2,2,4,4 };
const int attribute_0[5] = { 10,20,30,40,50 };

//���������ݻ������������棬���㻮�����ݼ�
void convert(int data[MAX_ROW][MAX_COLUMN], vector<int*> &todata, int row)//Ҫ��ֵ����
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
//�������ݼ�
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
//����ID3��C45��GINI
int ID3_C45_GINI(int row,const vector<int*> &data, const set<int> col, int Select)
{
	int label = 0;
	int d_num = 0;
	int ID3_data[9][13][2];//ά�ȷֱ��Ӧ��һ�����ԡ�ĳһ�������е��ĸ�ֵ��ĳһ��ֵ�е�1/0����
	memset(ID3_data, 0, sizeof(ID3_data));
	for (int i = 0; i<row; i++)
	{
		label = data[i][Column - 1] == 1 ? 1 : 0;
		d_num += label;
		//��һ���������ڷ�֧���࣬����ɢ�������򵥵ػ���Ϊ����10-20��20-30��30-40��40-50����Ϊ�����������ԣ�
		if (data[i][0] <= 20)//��һ�����Եķֲ淽��
			ID3_data[0][0][label]++;
		else if (data[i][0] <= 30)
			ID3_data[0][1][label]++;
		else if (data[i][0] <= 40)
			ID3_data[0][2][label]++;
		else
			ID3_data[0][3][label]++;

		for (int j = 1; j < Column; j++)//ͳ��ÿһ�������µķֲ��Ӧ�ı�ǩ
			ID3_data[j][data[i][j]][label]++;
	}
	//��
	double avoid_nan_1 = 0.0, avoid_nan_2 = 0.0;//Ԥ�б�ǩ���ǲ����㣬�������nan
	avoid_nan_1 = d_num == 0 ? 0 : (-(1.0 * d_num / row) * log2(1.0 * d_num / row));
	avoid_nan_2 = d_num == row ? 0 : (-(1.0 * (row - d_num) / row) * log2(1.0 * (row - d_num) / row));
	double hd = avoid_nan_1 + avoid_nan_2;

	if (hd == 0.0)
		return -1;

	//����������
	int tmp = 0;
	double min_hda = 10.0, hda = 0.0;//��С������
	int index_hda = -1;//��С�����ص��±꣬��Ӧ��������ֵ
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

		if (col.find(i) == col.end())//���ĳһ�����Բ�������������Լ������棬˵�������Ѿ�����ѡ��
			continue;
		for (int j = 0; j<rol_value[i]; j++)
		{
			tmp = ID3_data[i][j][0] + ID3_data[i][j][1];//ĳ������֮��ĳһ��ֵ������
			///////////////////////////////////////////////////////////////////////////////////////////////
			//Ԥ���жϼ��㹫ʽ�ķ�ĸ�Ƿ�Ϊ��
			//��Ϣ�����ʵļ��㹫ʽ
			avoid_nan_1 = tmp == 0 ? 0 : (-1.0*tmp / row*log(1.0*tmp / row));
			C45 += avoid_nan_1;
			///////////////////////////////////////////////////////////////////////////////////////////////
			//giniϵ���ļ��㹫ʽ
			gini += tmp != 0 ? 1.0*tmp / row*(1.0 - (1.0*ID3_data[i][j][0] / tmp)*(1.0*ID3_data[i][j][0] / tmp) - (1.0*ID3_data[i][j][1] / tmp)*(1.0*ID3_data[i][j][1] / tmp)) : 0;
			//��Ϣ�����������صļ��㹫ʽ
			avoid_nan_1 = ID3_data[i][j][0] == 0 ? 0 : (-1.0 * ID3_data[i][j][0] * log2(1.0 * ID3_data[i][j][0] / tmp));
			avoid_nan_2 = ID3_data[i][j][1] == 0 ? 0 : (-1.0 * ID3_data[i][j][1] * log2(1.0 * ID3_data[i][j][1] / tmp));
			//hda += 1.0 * tmp / row * (-1.0 * ID3_data[i][j][0] / tmp * log2(1.0 * ID3_data[i][j][0] / tmp) - 1.0 * ID3_data[i][j][1] / tmp * log2(1.0 * ID3_data[i][j][1] / tmp));
			hda += 1.0 / row * (avoid_nan_1 + avoid_nan_2);
		}
		//cout << hda << endl;
		//ѡ����������С�ģ�Ҳ������Ϣ��������
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
//���ڵ�
struct node
{
	bool if_end;
	int attribute;//����
	int value_attribute;//����ֵ
	int result;//�ýڵ���������жϵĽ���������Դ�Ϊ������ǩ��������0/1
	int floor;//����
	vector<node*> child;//�ӽڵ�
	node()
	{
		if_end = false;
		attribute = 0;
		value_attribute = result = floor = -1;
	}
};

//�ж��Ƿ񵽴�ݹ�߽磬�����ݼ���ʣ���������ж�
bool meet_bound(vector<int*> data, set<int> col)
{
	if (data.empty() || col.size() <= 1)
		return true;
	return false;
}
//��������ֵ�������ݼ�
vector<vector<int*>> divide_set(vector<int*> data, int attri)
{
	vector<vector<int*>> Set;//��Ҫ���ض�����ݼ�����vector����
	for (int j = 0; j < rol_value[attri]; j++)
	{
		vector<int*> tmp;
		tmp.clear();
		//�����������в�ͬ����ֵ�������ݼ�
		for (int i = 0; i < data.size(); i++)
		{
			if (attri == 0)//��һ������ɢ������
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
//����
void build_tree(node* p, const vector<int*> &data, const set<int> col)
{
	if (meet_bound(data,col))
	{
		int attri[13][2];
		memset(attri, 0, sizeof(attri));
		int attribute = *(col.begin());//Ĭ��ѡ��ʣ���������еĵ�һ��
		int label = 0;
		//ͳ��ʣ�����ݼ��еı�ǩ����
		for (int i = 0; i < data.size(); i++)
		{
			label = data[i][Column - 1] == 1 ? 1 : 0;
			if (attribute == 0)
			{
				if (data[i][0] <= 20)//��һ�����Եķֲ淽��
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
		//�����ֲ�ڵ���ΪҶ�ӽڵ�
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
	//ͨ������ID3/C4.5/Gini��ѡ��������
	int select_attri = ID3_C45_GINI(data.size(), data, col, SELECT);

	//�߽��ж������еı�ǩȫ��һ�£���ʱ����ѡ����Ϊ-1
	if (select_attri == -1)
	{
		p->result = data[0][Column - 1] == 1 ? 1 : 0;
		return;
	}
	//ͳ���������еı�ǩ����
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
	new_col.erase(select_attri);//ɾ��ѡ����
	data_set = divide_set(data, select_attri);//�������ݼ�
	//������֧�ڵ㣬�ݹ齨��
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
//BFS�������������鿴С���ݼ��������
void BFS(node* p)
{
	queue<node*> bfs;
	bfs.push(p);
	int T = 0, F = 0;//�ж�1��0��Ҷ����
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
//��֦�������Щֻ��һ����֧�Ľڵ�
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
//Ԥ������ʱ��ĵݹ����
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
//Ԥ������
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

//���սڵ�
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

//�Ե����Ϻ��֦
void hinder_cut(node* p,node* root, const int data[MAX_ROW][MAX_COLUMN], int row)
{
	if (p->child.empty())
		return;
	for (int i = 0; i < p->child.size(); i++)
	{
		hinder_cut(p->child[i], root, data, row);//�ȵݹ��ҵ���ײ�ڵ�
		double before_acc = Predict(root, data, row);//���㵱ǰ׼ȷ��
		p->child[i]->if_end = true;//���赱ǰ�ڵ�ΪҶ�ӽڵ�
		double hinder_acc = Predict(root, data, row);//���������׼ȷ��
		if (before_acc - hinder_acc < 0.1)//��ֵ����Ϊ׼ȷ���½�0.1
		{
			revoke(p->child[i]);//ɾ������
			p->child.erase(p->child.begin() + i);
		}
		else
			p->if_end = false;
	}
}

//�������ݿռ�
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

	string two_two_train = "D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\���ȶ�\\train.csv";
	string two_two_validation = "D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\���ȶ�\\train.csv";

	string four_one_train = "D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\�ı�һ\\train.csv";
	string four_one_val = "D:\\Junior Three\\Artificial Intelligence\\lab4_Decision_Tree\\�ı�һ\\validation.csv";

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

	//cut_only_one(root);//��֦

	//cout << "Before hinder cut: " << endl;
	//BFS(root);

	//temp = Text_Handle(four_one_val, val_row_data);
	//int val_row = 0;
	//val_row = temp.first;
	//cout << Predict(root, val_row_data, val_row, false, true) << endl;//Ԥ��

	//hinder_cut(root, root, val_row_data, val_row);

	//cout << "After hinder cut: " << endl;
	//BFS(root);
	//cout << Predict(root, val_row_data, val_row, false, true) << endl;//Ԥ��

	temp = Text_Handle(yanshou_test, test_row_data);
	int test_row = 0;
	test_row = temp.first;

	//for (int i = 0; i < test_row; i++)
	//{
	//	for (int j = 0; j < Column; j++)
	//		cout << test_row_data[i][j] << " ";
	//	cout << endl;
	//}

	cout << Predict(root, test_row_data, test_row) << endl;//Ԥ��


	revoke(root);//�������ڵ���ռ�ڴ�
	delete_data(RAW_DATA);//�������ݼ�ռ�е��ڴ�
	system("pause");
}