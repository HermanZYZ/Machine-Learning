#include <iostream>
#include <map>
#include <cstdlib>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>
#include <functional>

using namespace  std;

#define ANGER 1
#define DISGUST 2
#define FEAR 3
#define JOY 4
#define SAD 5
#define SURPRISE 6
#define Laplace_alpha 1
//0.53
#define Laplace_alpha_reg 0.04

#define NB_CLA
//#define NB_REG

const char *Seperate = " ,";

#ifdef NB_CLA

int Word_Bag[7][4000];//词袋每个词的数量
std::map<string, int> Emotion_word2index[7];//每一种情绪下 词->下标
int Total_word[7];//每一种情绪下词袋的大小
string Temp_Line[20];//临时保存一行字符串
int Column[7];//每一个词袋里面不重复词的数量
int Emotion_quantity[7];//每种情绪出现的次数
set<string> Words;
double Frequency[7][4000];
int Total_Lines = 0;

string Judge(int motion)
{
	switch (motion)
	{
	case ANGER:return "anger"; break;
	case DISGUST:return "disgust"; break;
	case FEAR:return "fear"; break;
	case JOY:return "joy"; break;
	case SAD:return "sad"; break;
	case SURPRISE:return "surprise"; break;
	}
}

int if_emotion(string OneWord)
{
	if (OneWord == "joy*")return JOY;
	if (OneWord == "sad*")return SAD;
	if (OneWord == "fear*")return FEAR;
	if (OneWord == "anger*")return ANGER;
	if (OneWord == "surprise*")return SURPRISE;
	if (OneWord == "disgust*")return DISGUST;
	return 0;
}

void NB_cla_Text_Handle(string Path)
{
	string OneLine;
	char * next_token = NULL;
	char *OneWord;
	char *oneline = new char[255];

	int Emotion = 0;
	int temp = 0;
	ifstream Semeval(Path, std::ios::in);
	if (Semeval)
	{
		getline(Semeval, OneLine);//处理掉title
		while (getline(Semeval, OneLine))
		{
			strcpy_s(oneline, 255, OneLine.c_str());//const char转成char 
			OneWord = strtok_s(oneline, Seperate, &next_token);
			while (OneWord)//统计词频
			{
				if ((Emotion = if_emotion(OneWord)) != 0)
				{
					// Emotion_Map[row] = Emotion;
					Emotion_quantity[Emotion]++;
				}
				else
				{
					Temp_Line[temp++] = OneWord;
					Words.insert(OneWord);
				}
				OneWord = strtok_s(NULL, Seperate, &next_token);
			}

			for (int i = 0; i < temp; ++i)
			{
				if (Emotion_word2index[Emotion].find(Temp_Line[i]) == Emotion_word2index[Emotion].end())
				{
					Word_Bag[Emotion][Column[Emotion]] = 1;
					Emotion_word2index[Emotion][Temp_Line[i]] = Column[Emotion];
					Column[Emotion]++;
					Total_word[Emotion]++;
				}
				else
				{
					int temp_column = Emotion_word2index[Emotion][Temp_Line[i]];
					Word_Bag[Emotion][temp_column]++;
					Total_word[Emotion]++;
				}
			}
			temp = 0;
			Total_Lines++;
		}
	}
	else
	{
		cout << "No such file!";
	}
	Semeval.close();

	//map<string, int>::iterator it;
	//for (int i = 1; i < 7; i++)
	//{
	//	cout << i << " " << Total_word[i]<<": ";
	//	for (int j = 0; j < Column[i]; j++)
	//	{
	//		cout << Word_Bag[i][j] << " ";
	//	}
	//	cout << endl;
	//	it = Emotion_word2index[i].begin();
	//	for (; it != Emotion_word2index[i].end(); it++)
	//		cout << it->first << " " << it->second << endl;
	//}

	for (int i = 1; i < 7; i++)
	{
		for (int j = 0; j < Column[i]; j++)
		{
			Frequency[i][j] = 1.0 * (Word_Bag[i][j] + Laplace_alpha) / (Total_word[i] + Words.size() * Laplace_alpha);//这才是P(xk | ei)
		}
	}
	//for (int i = 1; i < 7; i++)
	//	cout << Emotion_quantity[i] << endl;
}

int Vali_row = 0;
map<int, int> Vali_Emotion_Map;
string Validate[1000][20];//每一行以 * 结尾

void NB_Predict(string Path)
{
	string OneLine;
	char * next_token = NULL;
	char *OneWord;
	char *oneline = new char[255];

	int Emotion = 0;
	Vali_row = 0;
	int Vali_column = 0;
	Vali_Emotion_Map.clear();
	ifstream Vali(Path, std::ios::in);
	if (Vali)
	{
		getline(Vali, OneLine);//处理掉title
		while (getline(Vali, OneLine))
		{
			strcpy_s(oneline, 255, OneLine.c_str());//const char转成char 
			OneWord = strtok_s(oneline, Seperate, &next_token);
			while (OneWord)//统计词频
			{
				if ((Emotion = if_emotion(OneWord)) != 0)
				{
					Vali_Emotion_Map[Vali_row] = Emotion;
				}
				else if (Words.find(OneWord) == Words.end())//处理测试集不存在的词
				{
					//忽略
				}
				else
				{
					Validate[Vali_row][Vali_column] = OneWord;
					Vali_column++;
				}
				OneWord = strtok_s(NULL, Seperate, &next_token);
			}
			Validate[Vali_row][Vali_column] = "*";
			Vali_column = 0;
			Vali_row++;
		}
		Vali.close();
	}
	else
	{
		cout << "No such file!";
		Vali.close();
		return;
	}
	//for (int i = 0,j = 0; i < Vali_row; i++)
	//{
	//	j = 0;
	//	while (Validate[i][j] != "*")
	//	{
	//		cout << Validate[i][j] << " ";
	//		j++;
	//	}
	//	cout << endl;
	//}
	fstream result("D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\classification_dataset\\result_nb_cla_test.csv", ios::out);
	map<double, int,greater<double>> predit;
	map<double, int, greater<double>>::iterator it;
	int Correct = 0;
	double freq = 1.0;
	for (int i = 0, j = 0; i < Vali_row; i++)
	{
		predit.clear();
		for (int k = 1; k < 7; k++, j = 0)//第k种情绪
		{
			freq = 1.0 * Emotion_quantity[k] / Total_Lines;
			while (Validate[i][j] != "*")
			{
				if (Emotion_word2index[k].find(Validate[i][j]) == Emotion_word2index[k].end())//该词不在该情绪词袋中
				{
					freq *= Laplace_alpha / (Words.size() * Laplace_alpha + Total_word[k]);//重点：改掉V_ei，这里不做拉普拉斯平滑会高
				}
				else
				{
					freq *= Frequency[k][Emotion_word2index[k][Validate[i][j]]];
				}
				j++;
			}
			predit.insert(pair<double, int>(freq, k));
		}
		it = predit.begin();

		cout << it->first << " " << Judge(it->second) << endl;
		it++;
		cout << it->first << " " << Judge(it->second) << endl;
		result << Judge(it->second) << endl;
		if (it->second == Vali_Emotion_Map[i])
		{
			Correct++;
		}
	}
	//cout << Correct << " " << Vali_row << endl;
	result.close();
}

#endif // NB_CLA
/////////////////////////////////////////////////////////////////
#ifdef NB_REG

int reg_OneHot[800][5000], reg_Number[1000];//每一个文本出现的各个单词的数量，每一个文本一共出现了多少个单词
int reg_row, reg_column;//多少文本，多少不重复单词
double reg_TF[800][4000];
map<string, int> reg_word_index;//word -> index
map<int, string> reg_index_word;//index -> word
double reg_Emotion[800][7];
int Difference_words_line[800];

int Total_difference_words = 0;

void NB_reg_Text_Handle(string Path)
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
					Emotion_column++;
				}
				else if (reg_word_index.find(OneWord) == reg_word_index.end())
				{
					reg_OneHot[reg_row][reg_column] = 1;
					reg_word_index[OneWord] = reg_column;
					reg_index_word[reg_column] = OneWord;
					reg_Number[reg_row] ++;//一行的单词数
					reg_column++;
					Difference_words_line[reg_row]++;
					Total_difference_words++;
				}
				else
				{
					reg_OneHot[reg_row][reg_word_index[OneWord]] ++;//词频
					reg_Number[reg_row] ++;
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
			reg_TF[i][j] = 1.0 * (reg_OneHot[i][j] + Laplace_alpha_reg) / (1.0 * reg_Number[i] + Total_difference_words * Laplace_alpha_reg);
		}
	}

	//for (int i = 0; i < reg_row; i++)
	//{
	//	for (int j = 0; j < 6; j++)
	//		cout << reg_Emotion[i][j] << " ";
	//	cout << endl;
	//	system("pause");
	//}
}
set<int> Words_in_line[800];//一行中存在哪些单词
int reg_vali_row = 0;//验证集/测试集 一共有多少行
double Predict_emotion[800][7];//保存预测结果

void NB_reg(string Path, string Path_result)
{
	string OneLine;
	char * next_token = NULL;
	char *OneWord;
	char *oneline = new char[255];

	int vali_column = 0;
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
				if (OneWord[0] >= '0' && OneWord[0] <= '9' || OneWord[0] == '?')//忽略，不管它
				{
					//reg_vali_Emotion[reg_vali_row][Emotion_vali_column] = atof(OneWord);
					//Emotion_vali_column++;
				}
				else if (reg_word_index.find(OneWord) == reg_word_index.end())//训练集中不存在的词，丢掉
				{
					//reg_vali_OneHot[reg_vali_row][reg_column] = 1;
					//reg_word_index[OneWord] = reg_column;
					//reg_index_word[reg_column] = OneWord;
					//reg_column++;
					//reg_vali_Number[reg_vali_row] ++;//一行的单词数
				}
				else
				{
					vali_column = reg_word_index[OneWord];
					Words_in_line[reg_vali_row].insert(vali_column);
				}
				OneWord = strtok_s(NULL, Seperate, &next_token);
			}
			reg_vali_row++;
			vali_column = 0;
		}
	}
	else
	{
		cout << "No such file!";
	}
	vali_reg.close();

	set<int>::iterator it = Words_in_line[0].begin();
	double temp = 1.0;
	for (int i = 0; i < reg_vali_row; i++)//验证集中第i个文本
	{
		for (int j = 0; j < reg_row; j++)//训练集中第j个文本
		{
			temp = 1.0;
			it = Words_in_line[i].begin();
			for (; it != Words_in_line[i].end(); it++)
			{
				temp *= reg_TF[j][(*it)];
			}
			//system("pause");
			for (int k = 0; k < 6; k++)//第k中情绪
			{
				Predict_emotion[i][k] += temp*reg_Emotion[j][k];//还没有归一化
			}
		}
	}
	for (int i = 0; i < reg_vali_row; i++)
	{
		temp = 0.0;
		for (int j = 0; j < 6; j++)
		{
			temp += Predict_emotion[i][j];
		}

		for (int j = 0; j < 6; j++)
		{
			Predict_emotion[i][j] /= temp;
		}
	}

	fstream Result(Path_result, ios::out);
	for (int i = 0; i < reg_vali_row; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			Result << Predict_emotion[i][j] << ",";
		}
		Result << endl;
	}
}

#endif // NB_REG
////////////////////////////////////////////////////////////////

int main(int argc, char const *argv[])
{
	string test = "D:\\Junior Three\\Artificial Intelligence\\zhouwuxiawuKNN\\train_set.csv";

	string cla_test = "D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\classification_dataset\\test_set1.csv";
	string cla_validation = "D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\classification_dataset\\validation_set.csv";
	string cla_train = "D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\classification_dataset\\train_set.csv";

	string reg_train = "D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\regression_dataset\\train_set.csv";
	string reg_validation = "D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\regression_dataset\\validation_set.csv";
	string reg_test = "D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\regression_dataset\\test_set.csv";

	string reg_result = "D:\\Junior Three\\Artificial Intelligence\\lab2\\lab2(KNN+NB)\\DATA\\regression_dataset\\result_validation_nb.csv";

	string testTrain = "D:\\Junior Three\\Artificial Intelligence\\lab2\\周五下午_NB\\train_set.csv";
	string test1 = "D:\\Junior Three\\Artificial Intelligence\\lab2\\周五下午_NB\\test_set.csv";

#ifdef NB_CLA

	memset(Word_Bag, 0, sizeof(Word_Bag));
	memset(Total_word, 0, sizeof(Total_word));
	memset(Column, 0, sizeof(Column)); 
	memset(Frequency, 0, sizeof(Frequency)); 
	memset(Emotion_quantity, 0, sizeof(Emotion_quantity));
	NB_cla_Text_Handle(testTrain);
	NB_Predict(test1);

#endif // NB_CLA

#ifdef NB_REG
	memset(reg_OneHot, 0, sizeof(reg_OneHot));
	memset(reg_Number, 0, sizeof(reg_Number));
	memset(reg_TF, 0, sizeof(reg_TF));
	memset(reg_Emotion, 0, sizeof(reg_Emotion));
	memset(Predict_emotion, 0, sizeof(Predict_emotion)); 
	memset(Difference_words_line, 0, sizeof(Difference_words_line));

	NB_reg_Text_Handle(reg_train);
	NB_reg(reg_validation, reg_result);
#endif // NB_REG

	system("pause");
	return 0;
}