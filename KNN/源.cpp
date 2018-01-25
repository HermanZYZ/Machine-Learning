#include <iostream>
#include <cstdio>
#include <cmath>
#include <map>
#include <algorithm>
#include <fstream>
#include <string>
#include <cstring>
#include <set>
#include <functional>
#include <algorithm>
#include <vector>
#include <sstream>

#define ANGER 1
#define DISGUST 2
#define FEAR 3
#define JOY 4
#define SAD 5
#define SURPRISE 6
//#define KNN_CLA 14
#define KNN_REG 14

#define HIG 1
#define MID 2
#define LOW 3

#define EMOTIONS 3
using namespace std;

#define KNN_CLA_PROCESS

//#define KNN_REG_PROCESS

const char *Seperate = " ,";//分隔符

#ifdef KNN_CLA_PROCESS

int KNN_CLA = 14;

const int SIZE = 10000;
const int NUM_WORD = 24000;
const int CLA_ROW = 5200;

unsigned short OneHot[CLA_ROW][NUM_WORD], Number[CLA_ROW];//保存一个词在某一行文本出现的次数；一行中有多少单词
float Value[CLA_ROW];//某一行文本对应的tfidf的平方和
int row, column;//多少文本，多少非重复单词数
float TFIDF[CLA_ROW][NUM_WORD];//TF-IDF值越大，关键词越重要
map<string, int> word_index;//word -> index
map<int, string> index_word;//index -> word
map<int, int> Emotion_Map;//保存某一行的情绪标签
set<int> articles[NUM_WORD];//某个词在多少篇文章中出现过

const int VALI_ROW = 5100;

//int Vali_Hot[VALI_ROW][NUM_WORD];
map<int, int> Vali_Emotion_Map;
float Vali_TFIDF[VALI_ROW][NUM_WORD];
//int Vali_Number[VALI_ROW];
float Vali_Value[NUM_WORD];
float Count[6] = { 0,0,0,0,0,0 };//用来辅助计算求出的结果
multimap<float, int, greater<float>> Validate;//用来排序比较距离或者相似度的大小及对应的标签

string Judge(int motion)//将情绪编号转换成字符串
{
	switch (motion)
	{
	//case ANGER:return "anger"; break;
	//case DISGUST:return "disgust"; break;
	//case FEAR:return "fear"; break;
	//case JOY:return "joy"; break;
	//case SAD:return "sad"; break;
	//case SURPRISE:return "surprise"; break;
	case HIG:return "HIG"; break;
	case MID:return "MID"; break;
	case LOW:return "LOW"; break;
	default:return "LOW"; break;
	}
}

int if_emotion(string OneWord)//判断是不是情绪标签，注意文件首先用excel处理一下，在标签后面加上*
{
	if (OneWord == "joy*")return JOY;
	if (OneWord == "sad*")return SAD;
	if (OneWord == "fear*")return FEAR;
	if (OneWord == "anger*")return ANGER;
	if (OneWord == "surprise*")return SURPRISE;
	if (OneWord == "disgust*")return DISGUST;
	if (OneWord == "HIG*")return HIG;
	if (OneWord == "MID*")return MID;
	if (OneWord == "LOW*")return LOW;
	return 0;
}

void KNN_cla_Text_Handle(string Path)
{
	string OneLine;
	char * next_token = NULL;
	char *OneWord;
	char *oneline = new char[SIZE];

	int Emotion = 0;
	ifstream Semeval(Path, std::ios::in);
	if (Semeval)
	{
		getline(Semeval, OneLine);//处理掉title
		while (getline(Semeval, OneLine))
		{
			strcpy_s(oneline, SIZE, OneLine.c_str());//const char转成char 
			OneWord = strtok_s(oneline, Seperate, &next_token);
			while (OneWord)//统计词频,One-Hot
			{
				if ((Emotion = if_emotion(OneWord)) != 0)
				{
					Emotion_Map[row] = Emotion;
				}
				else if(strlen(OneWord) < 4)
				{
					
				}
				else if (word_index.find(OneWord) == word_index.end())
				{
					OneHot[row][column] = 1;
					word_index[OneWord] = column;
					articles[column].insert(row);//出现该单词的文章
					index_word[column] = OneWord;
					column++;
					Number[row] ++;//一行的单词数
				}
				else
				{
					OneHot[row][word_index[OneWord]] ++;//词频
					Number[row] ++;
					articles[word_index[OneWord]].insert(row);
				}
				OneWord = strtok_s(NULL, Seperate, &next_token);
			}
			row++;
		}
	}
	else
	{
		cout << "No such file!";
	}
	Semeval.close();

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			TFIDF[i][j] = 1.0 * OneHot[i][j] * (log(1.0 * row / (1.0 + 1.0 * articles[j].size()))) / (1.0 * Number[i]);//分母加一的作用是避免分母为零																									   //Value[i] += OneHot[i][j] * OneHot[i][j];
			Value[i] += TFIDF[i][j] * TFIDF[i][j];
		}
	}
	delete oneline;
}

vector<int> KNN_cla(string Path)
{
	string OneLine;
	char * next_token = NULL;
	char *OneWord;
	char *oneline = new char[SIZE];

	int Vali_row = 0;
	int Emotion = 0;
	Vali_Emotion_Map.clear();
	memset(OneHot, 0, sizeof(OneHot));
	memset(Number, 0, sizeof(Number));
	ifstream Vali(Path, std::ios::in);

	int Vali_Emotion_Num[] = { 0,0,0,0 };
	int Correct_Num[] = { 0,0,0,0 };

	vector<int> result;

	if (Vali)
	{
		while (getline(Vali, OneLine))
		{
			strcpy_s(oneline, SIZE, OneLine.c_str());//const char转成char 
			OneWord = strtok_s(oneline, Seperate, &next_token);
			while (OneWord)//统计词频,One-Hot
			{
				if ((Emotion = if_emotion(OneWord)) != 0)
				{
					Vali_Emotion_Map[Vali_row] = Emotion;
					Vali_Emotion_Num[Emotion] ++;
				}
				else if (strlen(OneWord) < 4)
				{
					
				}
				else if (word_index.find(OneWord) == word_index.end())//处理测试集不存在的词，直接丢掉
				{
					/*Vali_Hot[Vali_row][column] = 1;
					word_index[OneWord] = column;
					index_word[column] = OneWord;
					column++;*/
					Number[Vali_row]++;
				}
				else
				{
					OneHot[Vali_row][word_index[OneWord]] ++;//词频
					Number[Vali_row]++;
				}
				OneWord = strtok_s(NULL, Seperate, &next_token);
			}
			Vali_row++;
		}
		Vali.close();
	}
	else
	{
		cout << "No such file!";
		Vali.close();
		return result;
	}

	for (int i = 0; i < Vali_row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			Vali_TFIDF[i][j] = 1.0 * OneHot[i][j] / Number[i] * log(1.0*row / (1 + 1.0 * articles[j].size()));
			Vali_Value[i] += Vali_TFIDF[i][j] * Vali_TFIDF[i][j];
		}
	}

	double distance = 0;
	int temp_i = 0, Correct = 0;
	double temp = Count[0];
	double Cos = 0;
	//传测试集进来
	//fstream result("D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\classification_dataset\\result1.csv", ios::out);
	cout << "Vali_row:" << Vali_row << endl;
	for (int i = 0; i < Vali_row; i++)
	{
		Validate.clear();
		memset(Count, 0, sizeof(Count));
		for (int j = 0; j < row; j++)
		{
			distance = 0;
			Cos = 0;
			for (int k = 0; k < column; k++)
			{
				//distance += ((Vali_Hot[i][k] - OneHot[j][k]) * (Vali_Hot[i][k] - OneHot[j][k]));//欧式距离
				//distance += (Vali_TFIDF[i][k] - TFIDF[j][k])*(Vali_TFIDF[i][k] - TFIDF[j][k]);//欧式距离
				Cos += Vali_TFIDF[i][k] * TFIDF[j][k];//计算相似度的分子部分
				//cout << distance << endl;

			}
			distance = (1.0 * Cos / (sqrt(Vali_Value[i]) * sqrt(Value[j])));// (distance + 1);//余弦
			Validate.insert(pair<double, int>(distance, Emotion_Map[j]));
		}
		multimap<float, int>::iterator it = Validate.begin();//降序排序
		temp_i = 3, temp = Count[0];
		for (int k = 0; k < KNN_CLA; k++, it++)//KNN
		{
			Count[it->second] += it->first;//将计算出来的相同标签的相似度求和
		}
		for (int k = 1; k <= EMOTIONS; k++)//判定方法
		{
			if (Count[k] > temp)//求出相似度和最大的标签
			{
				temp = Count[k];
				temp_i = k;
			}
		}
		//result << Judge(temp_i + 1) << endl;//写文件
		result.push_back(temp_i);
		cout << temp_i << " " << Vali_Emotion_Map[i] << endl;
		if (temp_i == Vali_Emotion_Map[i])
		{
			Correct++;
			Correct_Num[temp_i] ++;
		}
	}
	float avg = 0.0;
	for (int i = 1; i <= EMOTIONS; i++)
	{
		cout << Judge(i) << ": " << 1.0 * Correct_Num[i] / Vali_Emotion_Num[i] << " ";
		cout << Correct_Num[i] << " " << Vali_Emotion_Num[i] << " ";
		avg += 1.0 * Correct_Num[i] / Vali_Emotion_Num[i];
	}
	cout << endl;
	cout << "Correct, Total and Rate:" << Correct << " " << Vali_row << " ";
	cout << 1.0 * Correct / Vali_row << endl;
	cout << "AVG: " << avg / 3 << endl;
	delete oneline;
	//result.close();
	return result;
}

#endif // KNN_CLA_PROCESS

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef KNN_REG_PROCESS
//一下声明的数据类型作用和分类的类似
int reg_OneHot[800][5000], reg_Number[1000];
double reg_Value[800];
int reg_row, reg_column;
double reg_TFIDF[800][4000];
map<string, int> reg_word_index;//word -> index
map<int, string> reg_index_word;//index -> word
double reg_Emotion[800][5000];
set<int> reg_articles[4000];

void KNN_reg_Text_Handle(string Path)
{
	string OneLine;
	char * next_token = NULL;
	char *OneWord;
	char *oneline = new char[255];

	int Emotion_column = 0;
	ifstream reg(Path, std::ios::in);
	if (reg)
	{
		getline(reg, OneLine);//处理掉title
		while (getline(reg, OneLine))
		{
			strcpy_s(oneline, 255, OneLine.c_str());//const char转成char 
			OneWord = strtok_s(oneline, Seperate, &next_token);
			while (OneWord)//统计词频,One-Hot
			{
				if (OneWord[0] >= '0' && OneWord[0] <= '9')
				{
					reg_Emotion[reg_row][Emotion_column] = atof(OneWord);
					//cout << reg_Emotion[reg_row][Emotion_column] << " ";///////////////////////
					Emotion_column++;
				}
				else if (reg_word_index.find(OneWord) == reg_word_index.end())
				{
					reg_OneHot[reg_row][reg_column] = 1;
					reg_word_index[OneWord] = reg_column;
					reg_index_word[reg_column] = OneWord;
					reg_articles[reg_column].insert(reg_row);
					reg_Number[reg_row] ++;//一行的单词数
					reg_column++;
				}
				else
				{
					reg_OneHot[reg_row][reg_word_index[OneWord]] ++;//词频
					reg_Number[reg_row] ++;
					reg_articles[reg_word_index[OneWord]].insert(reg_row);
				}

				OneWord = strtok_s(NULL, Seperate, &next_token);
			}
			reg_row++;
			Emotion_column = 0;
		}
	}
	else
	{
		cout << "No such file!";
	}
	reg.close();

	for (int i = 0; i < reg_row; i++)
	{
		for (int j = 0; j < reg_column; j++)
		{
			reg_TFIDF[i][j] = 1.0 * reg_OneHot[i][j] * (log(1.0 * reg_row / (1.0 + 1.0 * reg_articles[j].size()))) / (1.0 * reg_Number[i]);//分母加一的作用是避免分母为零
			reg_Value[i] += reg_TFIDF[i][j] * reg_TFIDF[i][j];
		}
	}
}

int reg_vali_OneHot[800][5000], reg_vali_Number[800];
double reg_vali_Value[800];
int reg_vali_row;
double reg_vali_Emotion[800][10];
multimap<double, int, greater<double>> reg_Validate;
double Calculate[7] = { 0,0,0,0,0,0,0 };
double reg_vali_TFIDF[800][4000];

void KNN_reg(string Path)
{
	string OneLine;
	char * next_token = NULL;
	char *OneWord;
	char *oneline = new char[255];

	int Emotion_vali_column = 0;
	ifstream vali_reg(Path, std::ios::in);
	if (vali_reg)
	{
		getline(vali_reg, OneLine);//处理掉title
		while (getline(vali_reg, OneLine))
		{
			strcpy_s(oneline, 255, OneLine.c_str());//const char转成char 
			OneWord = strtok_s(oneline, Seperate, &next_token);
			while (OneWord)//统计词频,One-Hot
			{
				if (OneWord[0] >= '0' && OneWord[0] <= '9')
				{
					reg_vali_Emotion[reg_vali_row][Emotion_vali_column] = atof(OneWord);
					//cout << reg_Emotion[reg_row][Emotion_column] << " ";///////////////////////
					Emotion_vali_column++;
				}
				else if (reg_word_index.find(OneWord) == reg_word_index.end())
				{
					reg_vali_OneHot[reg_vali_row][reg_column] = 1;
					reg_word_index[OneWord] = reg_column;
					reg_index_word[reg_column] = OneWord;
					reg_column++;
					reg_vali_Number[reg_vali_row] ++;//一行的单词数
				}
				else
				{
					reg_vali_OneHot[reg_vali_row][reg_word_index[OneWord]] ++;//词频
					reg_vali_Number[reg_vali_row] ++;
				}
				OneWord = strtok_s(NULL, Seperate, &next_token);
			}
			reg_vali_row++;
			Emotion_vali_column = 0;
		}
	}
	else
	{
		cout << "No such file!";
	}
	vali_reg.close();

	for (int i = 0; i < reg_vali_row; i++)
	{
		for (int j = 0; j < reg_column; j++)
		{
			reg_vali_TFIDF[i][j] = 1.0 * reg_vali_OneHot[i][j] * (log(1.0 * reg_row / (1.0 + 1.0 * reg_articles[j].size()))) / (1.0 * reg_vali_Number[i]);//分母加一的作用是避免分母为零
			reg_vali_Value[i] += reg_vali_TFIDF[i][j] * reg_vali_TFIDF[i][j];
		}
	}

	double distance = 0, Total = 0;
	double Cos = 0;
	//传测试集进来
	cout << "Calculate Done!" << endl;
	fstream result("D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\regression_dataset\\result1.csv", ios::out);
	//计算回归
	for (int i = 0; i < reg_vali_row; i++)
	{
		reg_Validate.clear();
		memset(Calculate, 0, sizeof(Calculate));
		for (int j = 0; j < reg_row; j++)
		{
			distance = 0;
			Cos = 0;
			for (int k = 0; k < reg_column; k++)
			{
				//distance += ((reg_vali_OneHot[i][k] - reg_OneHot[j][k]) * (reg_vali_OneHot[i][k] - reg_OneHot[j][k]));//欧式距离
				//distance += (Vali_TFIDF[i][k] - TFIDF[j][k])*(Vali_TFIDF[i][k] - TFIDF[j][k]);
				Cos += reg_vali_TFIDF[i][k] * reg_TFIDF[j][k];
				//cout << distance << endl;
			}
			distance = 1.0 * Cos / (sqrt(reg_vali_Value[i]) * sqrt(reg_Value[j]));//避免为0
			reg_Validate.insert(pair<double, int>(distance, j));//插入的是下标，用下标来寻找
		}

		Total = 0;
		multimap<double, int>::iterator it = reg_Validate.begin();
		for (int k = 0; k < KNN_REG; k++, it++)//KNN，计算每一个标签的可能性，还没有归一化
		{
			Calculate[ANGER] += reg_Emotion[it->second][0] * it->first;//可能性 * 相似度 / 总的相似度
			Calculate[DISGUST] += reg_Emotion[it->second][1] * it->first;
			Calculate[FEAR] += reg_Emotion[it->second][2] * it->first;
			Calculate[JOY] += reg_Emotion[it->second][3] * it->first;
			Calculate[SAD] += reg_Emotion[it->second][4] * it->first;
			Calculate[SURPRISE] += reg_Emotion[it->second][5] * it->first;
			Total += it->first;//求出归一化分母的总和
		}
		if (Total == 0)//某一个文本所有单词在训练集中都没有出现过的，统一瞎猜为joy
		{
			Total = 1;
			Calculate[JOY] = 1;
		}
		//for (int k = 1; k < 7; k++)
		//{
		//	//cout << Calculate[k] << " ";
		//	Total += Calculate[k];
		//}
		//cout << Total << endl;
		//cout << Calculate[ANGER] / Total << "," << Calculate[DISGUST] / Total << "," << Calculate[FEAR] / Total << "," << \
				//	Calculate[JOY] / Total << "," << Calculate[SAD] / Total << "," << Calculate[SURPRISE] / Total <<"--"<< Total << endl;
		result << Calculate[ANGER] / Total << "," << Calculate[DISGUST] / Total << "," << Calculate[FEAR] / Total << "," << \
			Calculate[JOY] / Total << "," << Calculate[SAD] / Total << "," << Calculate[SURPRISE] / Total << endl;
	}
	result.close();
}

#endif // KNN_REG_PROCESS

int main(int argc, char const *argv[])
{
	string cla_test = "D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\classification_dataset\\test_set1.csv";
	string cla_validation = "D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\classification_dataset\\validation_set.csv";
	string cla_train = "D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\classification_dataset\\train_set.csv";

	string reg_train = "D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\regression_dataset\\train_set.csv";
	string reg_validation = "D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\regression_dataset\\validation_set.csv";
	string reg_test = "D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\regression_dataset\\test_set.csv";

	string Test_train = "D:\\Junior Three\\Artificial Intelligence\\zhouwuxiawuKNN\\train_set.csv";
	string Test_test = "D:\\Junior Three\\Artificial Intelligence\\zhouwuxiawuKNN\\test_set.csv";

	string Pro_train = "Train.csv";
	string Pro_vali = "Validation.csv";
	string Pro_test1 = "Test1.csv";
	string Pro_test2 = "Test2.csv";

#ifdef KNN_CLA_PROCESS

	memset(OneHot, 0, sizeof(OneHot));
	memset(Number, 0, sizeof(Number));
	memset(TFIDF, 0, sizeof(TFIDF));
	memset(Value, 0, sizeof(Value));
	memset(Vali_Value, 0, sizeof(Vali_Value));
	//memset(articles, 0, sizeof(articles));
	word_index.clear();
	row = 0; //行数
	column = 0;//列数

	int ValiOrTest = 0;//Vali 0 Test 1
	cout << "Input K and Validation(0) or Test(1,2):";
	cin >> KNN_CLA >> ValiOrTest;

	vector<int> result;

	KNN_cla_Text_Handle(Pro_train);
	
	if (ValiOrTest == 0)
	{
		KNN_cla(Pro_vali);
		system("pause");
		return 0;
	}
	
	ofstream Result;
	stringstream temp;
	temp << KNN_CLA;
	string K;
	temp >> K;
	string Pro_result = "" + K + "Validation.csv";
	
	if (ValiOrTest == 1)
	{
		result = KNN_cla(Pro_test1);
		Result.open(Pro_result, std::ios::out);
		for (int i = 0; i < result.size(); i++)
		{
			Result << Judge(result[i]) << '\n';
		}
		Result.close();
	}
	else if (ValiOrTest >= 2)
	{
		memset(Vali_Value, 0, sizeof(Vali_Value));
		result.clear();
		result = KNN_cla(Pro_test2);
		Result.open(Pro_result, std::ios::app);
		for (int i = 0; i < result.size(); i++)
		{
			Result << Judge(result[i]) << '\n';
		}
		Result.close();
	}
	cout << "Size:" << result.size() << endl;

#endif // KNN_CLA_PROCESS

#ifdef KNN_REG_PROCESS
	memset(reg_OneHot, 0, sizeof(reg_OneHot));
	memset(reg_Number, 0, sizeof(reg_Number));
	memset(reg_TFIDF, 0, sizeof(reg_TFIDF));
	memset(reg_Value, 0, sizeof(reg_Value));
	memset(reg_vali_Value, 0, sizeof(reg_vali_Value));

	reg_row = 0;
	reg_column = 0;
	KNN_reg_Text_Handle(reg_train);

	KNN_reg(reg_validation);

#endif // KNN_REG_PROCESS
	system("pause");
	return 0;
}