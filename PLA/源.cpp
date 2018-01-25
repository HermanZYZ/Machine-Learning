#include<iostream>
#include<string>
#include<cmath>
#include<sstream>
#include<fstream>
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

#define Accuracy 0
#define Recall 1
#define Precision 2
#define F1 3

using namespace std;

const int Generation = 100;
const int MAX_ROW = 4500;
const int MAX_COLUMN = 70;

double train_data[MAX_ROW][MAX_COLUMN];
double val_data[MAX_ROW][MAX_COLUMN];
double test_data[MAX_ROW][MAX_COLUMN];
int train_lable[MAX_ROW];
int val_lable[MAX_ROW];
int test_lable[MAX_ROW];
int train_row = 0;
int val_row = 0;
int test_row = 0;
int Column = 0;

double w_original[MAX_COLUMN];
double w_pocket[MAX_COLUMN];
double w_best[MAX_COLUMN];
double Result[4];//Accuracy, Recall, Precision, F1

pair<int,int> Text_Handle(string Path, double data[MAX_ROW][MAX_COLUMN],int lable[MAX_ROW])
{
	stringstream line;
	string temp_line;
	ifstream Semeval(Path, std::ios::in);
	char temp_notation;
	int row = 0, column = 1;

	if (Semeval)
	{
		while (getline(Semeval, temp_line))
		{
			line.str("");//!!!!!!!!
			line.clear();
			line << temp_line;
			column = 1;
			data[row][0] = 1;
			while (line >> data[row][column])
			{
				if (line)
					line >> temp_notation;
				else
					break;
				column++;
			}
			lable[row] = (int)data[row][column - 1];
			data[row][column - 1] = 0.0;
			row++;
		}
	}
	else
	{
		cout << "No file!" << endl;
	}
	Semeval.close();

	return pair<int, int>(row, column - 1);
}

int Sign(double num)
{
	if (num == 0.0)
		return 0;
	return num > 0.0 ? 1 : -1;
}

void Calculate_original(int generation,double w[MAX_COLUMN])
{
	for (int i = 0; i < Column; i++)//初始化w
	{
		w[i] = 1;
		//w[i] = (rand() % 10) + 1;
	}
	double result = 0;
	for (int i = 0; i < generation; i++)//迭代词数
	{
		for (int j = 0; j < train_row; j++)//训练数据
		{
			result = 0.0;
			for (int k = 0; k < Column; k++)//计算
			{
				result += w[k] * train_data[j][k];
			}
			if (Sign(result) == 0 || Sign(result) != train_lable[j])//判断是否更新
			{
				for (int k = 0; k < Column; k++)
				{
					w[k] += (train_lable[j] * train_data[j][k]);
				}
				break;
			}
		}
	}
}

double Calculate_error(double w[MAX_COLUMN],double data[MAX_ROW][MAX_COLUMN],int lable[MAX_COLUMN],int row)
{
	int error = 0;
	double result = 0;
	int TP = 0, FN = 0, TN = 0, FP = 0;
	for (int i = 0; i < row; i++)
	{
		result = 0;
		for (int j = 0; j < Column; j++)
		{
			result += w[j] * data[i][j];
		}
		if (Sign(result) == lable[i] && lable[i] > 0)
		{
			TP++;
		}
		else if (Sign(result) != lable[i] && lable[i] > 0)
		{
			FN++;
		}
		else if (Sign(result) == lable[i] && lable[i] < 0)
		{
			TN++;
		}
		else
			FP++;
	}

	double accuracy = 1.0 * (TP + TN) / (TP + FN + TN + FP);
	double recall = 1.0 * TP / (TP + FN);
	double precision = 1.0 * TP / (TP + FP);
	double f1 = 1.0 * 2 * precision * recall / (recall + precision);
	//return TP == 0 ? 0.0 : f1;
	return accuracy;
}

void Calculate_pocket(int generation,double w[MAX_COLUMN])
{
	for (int i = 0; i < Column; i++)
		w[i] = (rand() % 10) + 1;
	double result = 0;
	double error_rate = 0.0;
	double temp_rate = 0.0;
	for (int i = 0; i < generation; i++)//迭代次数
	{
		for (int j = 0; j < train_row; j++)//训练数据
		{
			result = 0.0;
			for (int k = 0; k < Column; k++)
			{
				result+= w[k] * train_data[j][k];
			}
			if (Sign(result) != train_lable[j])//判断是否更新
			{
				for (int k = 0; k < Column; k++)
				{
					w[k] += (train_lable[j] * train_data[j][k]);
				}
				break;
			}
		}

		if ((temp_rate = Calculate_error(w,val_data,val_lable,val_row)) > error_rate)//从口袋拿出最优进行判断
		{
			memcpy(w_best, w ,sizeof(w_best));
			error_rate = temp_rate;
		}
	}
}

void Predict(double w[MAX_COLUMN],double data[MAX_ROW][MAX_COLUMN],int lable[MAX_ROW],int row)
{
	double result = 0.0;
	int TP = 0, FN = 0, TN = 0, FP = 0;
	fstream result_file("D:\\Junior Three\\Artificial Intelligence\\lab3(PLA)\\lab3数据\\result.csv", ios::out);
	for (int i = 0; i < row; i++)
	{
		result = 0.0;
		for (int j = 0; j < Column; j++)
		{
			result += w[j] * data[i][j];
		}
		int sign = Sign(result);
		cout << sign << endl;
		result_file << sign << endl;
		if (sign == lable[i] && lable[i] > 0)
		{
			TP++;
		}
		else if (sign != lable[i] && lable[i] > 0)
		{
			FN++;
		}
		else if (sign == lable[i] && lable[i] < 0)
		{
			TN++;
		}
		else
			FP++;
	}
	Result[0] = 1.0 * (TP + TN) / (TP + FN + TN + FP);
	Result[1] = 1.0 * TP / (TP + FN);
	Result[2] = 1.0 * TP / (TP + FP);
	Result[3] = 1.0 * 2 * Result[2] * Result[1] / (Result[1] + Result[2]);
	//cout << Result[Accuracy] << " " << Result[Recall] << " " << Result[Precision] << " " << Result[F1] << endl;
}

int main()
{
	srand((unsigned)time(0));

	string small_set = "D:\\Junior Three\\Artificial Intelligence\\lab3(PLA)\\lab3数据\\1.csv";
	string train_set = "D:\\Junior Three\\Artificial Intelligence\\lab3(PLA)\\lab3数据\\train.csv";
	string val_set = "D:\\Junior Three\\Artificial Intelligence\\lab3(PLA)\\lab3数据\\val.csv";
	string test_set = "D:\\Junior Three\\Artificial Intelligence\\lab3(PLA)\\lab3数据\\test.csv";

	string test_train = "D:\\Junior Three\\Artificial Intelligence\\lab3(PLA)\\周五56验收\\fri56train.csv";
	string test_test = "D:\\Junior Three\\Artificial Intelligence\\lab3(PLA)\\周五56验收\\fri56test.csv";
	string pro_Binary = "D:\\Junior Three\\Artificial Intelligence\\Project\\binary_classification\\70.8\\ori_train2_2.csv";

	memset(train_data, 0, sizeof(train_data));
	memset(val_data, 0, sizeof(val_data));
	memset(train_lable, 0, sizeof(train_lable));
	memset(val_lable, 0, sizeof(val_lable));

	pair<int, int> temp;
	//temp = Text_Handle(train_set, train_data, train_lable);
	temp = Text_Handle(test_train, train_data, train_lable);
	train_row = temp.first;
	Column = temp.second;

	//temp = Text_Handle(val_set, val_data, val_lable);
	//val_row = temp.first;

	temp = Text_Handle(test_test, test_data, test_lable);
	test_row = temp.first;

	Calculate_original(Generation,w_original);
	//Calculate_pocket(Generation, w_pocket);
	for (int i = 0; i < Column; i++)
		cout << w_original[i] << " ";
	cout << endl;

	Predict(w_original, test_data, test_lable, test_row);
	//Predict(w_best, val_data, val_lable, val_row);
	//Predict(w_best, test_data, test_lable, test_row);

	system("pause");
	return 0;
}