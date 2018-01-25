#include<iostream>
#include<sstream>
#include<cstring>
#include<fstream>

using namespace std;

int lable[1100][2];
int Lable[1100];

int main()
{
	stringstream line;
	string Line;
	fstream file("result1.csv",ios::in);
	int row = 0;
	while(getline(file,Line))
	{
		line.str("");
		line.clear();
		line<<Line;
		int temp;
		char tmp;
		while(line>>temp)
		{
			if(line)
				line>>tmp;
			if(temp==1)
				lable[row][1]++;
			else
				lable[row][0]++;
		}
		if(temp == 1)
			lable[row][1]--;
		else
			lable[row][0]--;
		Lable[row] = temp;
		row++;
	}

	int correct = 0;
	int temp = 0;
	int TP = 0, FN = 0, TN = 0, FP = 0;
	for(int i=0;i<row;i++)
	{
		temp = lable[i][1] > lable[i][0] ? 1 : -1;
		if (temp == Lable[i] && Lable[i] > 0)
		{
			TP++;
		}
		else if (temp != Lable[i] && Lable[i] > 0)
		{
			FN++;
		}
		else if (temp == Lable[i] && Lable[i] < 0)
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
	cout<<accuracy<<" "<<recall<<" "<<precision<<" "<<f1<<endl;
}