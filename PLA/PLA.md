# 中山大学数据科学与计算机学院

# 移动信息工程专业-人工智能

# 本科生实验报告

（2017-2018 学年秋季学期）

课程名称：**Artificial Intelligence**

| 教学班号 | 15M3 | 专业（方向） | 移动信息工程（互联网） |
| :------: | :--: | :----------: | :--------------------: |
|          |      |              |                        |

## 一、实验题目

感知机学习算法(Perceptron Learning Algorithm)：利用PLA算法解决二元分类问题。



## 二、实验内容

### 1.算法原理

​	PLA(Perceptrons Learning Alogrithm)是一种用来二维或者高维的线性可划分问题的分类的算法。就像是在二维中找到一条线，将线性可划分问题分成两类，尽量使得准确划分；或者是在高维中找到一个超平面，将问题区分成两类。假设从$\overrightarrow{X}$到$\overrightarrow{Y}$中存在一个映射关系$f$， 其中$\overrightarrow{Y}$是二维的。PLA算法就是模拟这个映射关系$f$，使得求出来的函数尽可能接近$f$。 

​	在PLA算法中，$\overrightarrow{X}$ 就像是问题的各个属性的值构成的向量，$\overrightarrow{w}$ 就像是各个属性的权重，通过$\overrightarrow{X}\overrightarrow{w}$ ，也就是将各个属性值加权求和，然后通过阈值或者符号将问题区分。

### 2.伪代码

**原始PLA**：

​	input：迭代次数generation、训练数据$X=\{\overrightarrow{x_1},\overrightarrow{x_2},...,\overrightarrow{x_n}\}$
​	output: $\overrightarrow{w}$ 

​	begin:

​		for i := 0, i  = i + 1, until i == generation or can not find such j below, do

​			for j := 0, j = j + 1,until j == n, do

​				if sign($\overrightarrow{w^T_t}x_j)$ $\neq y_j$  , then 

​					$\overrightarrow{w^T_{t+1}} \leftarrow x_jy_j$

​	end



**口袋PLA：**

​	input：迭代次数generation、训练数据$X=\{\overrightarrow{x_1},\overrightarrow{x_2},...,\overrightarrow{x_n}\}$
​	output: $\overrightarrow{w}$ 

​	begin:

​		for i := 0, i  = i + 1, until i == generation or can not find such j below, do

​			for j := 0, j = j + 1,until j == n, do

​				if sign($\overrightarrow{w^T_t}x_j)$ $\neq y_j$  , then 

​					$\overrightarrow{w^T_{t+1}} \leftarrow x_jy_j$

​					break

​			calculate the error rate $e$ using $\overrightarrow{w^T_{t+1}}$ and the error rate $E$ using $w$

​			if $e < E$, then $w\leftarrow\overrightarrow{w^T_{t+1}}$ 

​	end



**预测：**

​	input: $\overrightarrow{w}$、待预测数据$X=\{\overrightarrow{x_1},\overrightarrow{x_2},...,\overrightarrow{x_n}\}$ 

​	output：预测标签lable $Y=\{\overrightarrow{y_1},\overrightarrow{y_2},...,\overrightarrow{y_n}\}$

​	begin:

​		$Y=Sign(\overrightarrow{w}X)=Sign(\overrightarrow{wx_1},\overrightarrow{wx_2},...,\overrightarrow{wx_n})$ 

​	end 



### 3.关键代码（带注释）

**原始PLA代码：** 

~~~C++
void Calculate_original(int generation,double w[MAX_COLUMN])
{
	for (int i = 0; i < Column; i++)//初始化w
		w[i] = 1;
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
				//break;
			}
		}
	}
}
~~~



**口袋PLA代码：**

~~~c++
double Calculate_error(double w[MAX_COLUMN],double data[MAX_ROW][MAX_COLUMN],int lable[MAX_COLUMN],int row)//计算错误率
{
	int error = 0;
	double result = 0;
	for (int i = 0; i < row; i++)
	{
		result = 0;
		for (int j = 0; j < Column; j++)
		{
			result += w[j] * data[i][j];
		}
		if (Sign(result) != lable[i])
		{
			error++;
		}
	}
	return 1.0*error / row;
}

void Calculate_pocket(int generation,double w[MAX_COLUMN])
{
	for (int i = 0; i < Column; i++)
		w[i] = 1;
	double result = 0;
	double error_rate = 1.0;
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

		if ((temp_rate = Calculate_error(w,train_data,train_lable,train_row)) < error_rate)//从口袋拿出最优进行判断
		{
			memcpy(w_best, w,sizeof(w_best));
			error_rate = temp_rate;
		}
	}
}
~~~



## 三、实验结果及分析

### 1.实验结果展示示例

**小数据集验证：**

(1)最后一列为标签

| train1 |  -4  |  -1  |  1   |
| :----: | :--: | :--: | :--: |
| train2 |  0   |  3   |  -1  |
|  test  |  2   |  1   |  -1  |

模型计算出的$\overrightarrow{w}={1,-3,-3}$ ，满足PLA算法正确结果要求。



(2)最后一列为标签

| train1 |  1   |  1   |  0   |  -1  |
| :----: | :--: | :--: | :--: | :--: |
| train2 |  3   |  3   |  1   |  1   |
| trian3 |  4   |  3   |  -1  |  1   |
|  test  |  2   |  1   |  0   |  -1  |

模型计算出的$\overrightarrow{w}={-2,1,0,0}$，满足PLA算法正确结果要求。



### 2.评测指标展示及分析

#### a.口袋算法实现中错误率的计算指标

​	在口袋算法的实现中，对于利用错误率（或其他指标）来评估$\overrightarrow{w}$的优劣的问题，有利用训练集和验证集分别评估的想法。在老师的建议下，对两种做法进行验证：

|  算法  | 迭代次数  | 用于评估的数据集 | 在验证集上的准确率 |
| :--: | :---: | :------: | :-------: |
| 口袋算法 | 50000 |   验证集    |   84.7%   |
| 口袋算法 | 50000 |   训练集    |   83.9%   |



​	第一种方法可以避免过度拟合训练集，第二种方法可以避免过度拟合验证集。其实在上面的实验中，应该再划分出一个数据集对两种方法进行测试评估，而不是直接使用验证集。但是在实际情况中，一般验证集比较接近测试集，所以，利用验证集更能评估$\overrightarrow{w}$的优劣，提高预测的准确率。



#### b.口袋算法中$\overrightarrow{w}$的优劣评估指标

​	对于$\overrightarrow{w}$， 由于在问题分类上，准确率最高并不一定代表了模型分类效果最好。有时候如果需要提取有效信息，可能就需要用到精准率。所以，针对各个评测指标，进行实验分析如下：

|  算法  | 迭代次数  | 评估指标 |  准确率  |   召回率   |   精准率    |    F1    |
| :--: | :---: | :--: | :---: | :-----: | :------: | :------: |
| 口袋算法 | 10000 | 准确率  | 0.846 |  0.375  | 0.526316 | 0.437956 |
| 口袋算法 | 10000 | 召回率  | 0.233 | 0.93125 | 0.164641 | 0.279812 |
| 口袋算法 | 10000 | 精准率  | 0.846 |  0.375  | 0.526316 | 0.437956 |
| 口袋算法 | 10000 |  F1  | 0.796 | 0.74375 | 0.421986 | 0.538462 |

​	观察可以发现，在利用准确率和精准率作为评估指标的时候，模型计算出来的各个指标都是相同的，而且各项指标都比较均衡，说明利用准确率或者精准率来评估$\overrightarrow{w}$效果还不错。而在利用召回率作为评估指标的时候，除了召回率很高之外，其他指标的很低，说明模型走向了极端。经过分析，发现是因为数据集标签分布不均衡的。在数据集当中，标签为1的数据项只有20%左右，所以即使将所有标签为1的数据项预测准确，整个文本预测的准确率也是只有20%左右。



### 4、尝试优化及结果分析

**优化后的原始PLA**：

​	input：迭代次数generation、训练数据$X=\{\overrightarrow{x_1},\overrightarrow{x_2},...,\overrightarrow{x_n}\}$
​	output: $W=\{\overrightarrow{w_1},\overrightarrow{w_2},...,\overrightarrow{w_n}\}$ ,$Y=\{\overrightarrow{y_1},\overrightarrow{y_2},...,\overrightarrow{y_k}\}$ 

​	begin:

​		for i := 0, i  = i + 1, until i == generation or can not find such j below, do

​			for j := 0, j = j + 1,until j == n, do

​				if sign($\overrightarrow{W^T_t}x_j)$ $\neq y_j$  , then 

​					$\overrightarrow{W^T_{t+1}} \leftarrow x_jy_j$

​		calculate $Y_i=\{\overrightarrow{y_{i,1}},...,\overrightarrow{y_{i,n}}\}=W^Tx_i=\{\overrightarrow{w_1^T},\overrightarrow{w_2^T},...,\overrightarrow{w_n^T}\}x_i$

​		$y_i=Mode(Y_i)$ 

​	end



**优化后的原始PLA代码：**

​	在原始PLA代码的基础上，将$\overrightarrow{w}$ 的初始化改成随机设定，然后多次运行将预测结果输出来并编写程序统计比对。



**实验结果分析：**

|  迭代  |  n   |  准确率  |   召回率   |   精准率    |    F1    |
| :--: | :--: | :---: | :-----: | :------: | :------: |
| 1000 |  3   | 0.843 | 0.16875 | 0.529412 | 0.255924 |

​	优化过后的原始PLA算法指标上和没有优化的PLA算法（准确率0.844）相差不大。但是由于优化的PLA采取的是多个$\overrightarrow{w}$进行计算然后采取投票机制预测的，所以优化过后的模型在稳定性以及鲁棒性上优于没有优化过的。



**优化过的口袋PLA：**

​	input：迭代次数generation、训练数据$X=\{\overrightarrow{x_1},\overrightarrow{x_2},...,\overrightarrow{x_n}\}$
​	output:$W=\{\overrightarrow{w_1},\overrightarrow{w_2},...,\overrightarrow{w_n}\}$ ,$Y=\{\overrightarrow{y_1},\overrightarrow{y_2},...,\overrightarrow{y_k}\}$ 

​	begin:

​		for i := 0, i  = i + 1, until i == generation or can not find such j below, do

​			for j := 0, j = j + 1,until j == n, do

​				if sign($\overrightarrow{W^T_t}x_j)$ $\neq y_j$  , then 

​					$\overrightarrow{W^T_{t+1}} \leftarrow x_jy_j$

​					break

​			calculate the error rate $e$ using $\overrightarrow{W^T_{t+1}}$ and the error rate $E$ using $W$

​			if $e < E$, then $W\leftarrow\overrightarrow{W^T_{t+1}}$ 

​		calculate $Y_i=\{\overrightarrow{y_{i,1}},...,\overrightarrow{y_{i,n}}\}=W^Tx_i=\{\overrightarrow{w_1^T},\overrightarrow{w_2^T},...,\overrightarrow{w_n^T}\}x_i$

​		$y_i=Mode(Y_i)$ 

​	end



**优化后的口袋PLA代码：**

​	在原始PLA代码的基础上，将$\overrightarrow{w}$ 的初始化改成随机设定，然后多次运行将预测结果输出来并编写程序统计比对。



**实验结果分析：**

|  迭代   |  n   |  准确率  |  召回率  |   精准率    |    F1    |
| :---: | :--: | :---: | :---: | :------: | :------: |
| 50000 |  3   | 0.846 | 0.375 | 0.526316 | 0.437956 |



​	和上面的实验结果进行比较，发现和利用准确率评估$\overrightarrow{w}$ 迭代10000的指标数据一模一样。经过分析发现，口袋算法是一种类似贪心的算法，迭代次数越多，效果越好。所以经过了上万次的迭代，$\overrightarrow{w}$  已经收敛一致，所以对结果的影响不会很大。



## 四、思考题

#### 有什么其他的手段可以解决数据集非线性可分的问题？

* 计算多个$\overrightarrow{w}$ 然后利用投票机制或者计算距离进行预测：因为对于二元分类问题，可能存在多个较优的分界线或者。通过计算多个$\overrightarrow{w}$，得到多个划分方法，再结合投票机制或者计算距离加权计算，有利于提高准确率，解决非线性可分问题。
* 利用核函数将低维向量映射到高维向量。在数学上有很多利用高维运算解决低维问题十分有效的例子，例如四元数。在非线性可分问题上，可能在向量所在维度上非线性可分，但是通过映射到高维之后，往往就受线性约束，变成线性可分的了。关于核函数，常用的一般有线性内核，多项式内核，高斯内核，sigmoid内核等。

#### 请查询相关资料，解释为什么要用这四种评测指标，各自的意义是什么。

* 准确率 = 准确识别的问题总数 /  需要识别的问题总数。准确率主要用来评价模型对问题的判断能力。在二元分类问题上，准确力就是评价模型准确将问题分类的能力。但是，准确率并不总是有效地评价模型的各项效果。像这一次实验当中，标签为-1的数据项占了大部分（超过80%），所以如果模型不进行计算，直接把所有的数据项预测为-1，准确率也有80%以上，但是这样的模型是不具有意义的。
* 召回率 = 准确识别的有效信息总数 /  数据集中存在的有效信息总数。召回率和精准率同样主要用来评价结果的质量，主要广泛用于信息检索和统计分类。召回率用来计算检索到的相关信息和所有的相关信息的比例，衡量的是模型的查全率。假如说实验中标签为1的是有效信息，那么准确检测出来为1的数据和数据集中所有标签为1的数据的比例，就是召回率。这个指标可以评价模型对有效信息的提取能力。
* 精准率 = 准确识别的有效信息总数 / 实际识别为有效信息的总数。精准率表示了模型检索出来的有效信息的准确率的能力，也就是在模型预测为有效信息的数据当中，实际具有有效信息的数据的比率。精准率衡量的是模型的查准率，也是评价模型的鲁棒性和抗噪声的能力。
* F1 = 2 * 精准率 * 召回率 / （精准率 + 召回率），即精准率和召回率的调和平均值。在二元分类问题上，如果模型能够在不牺牲精度的情况下获得一个较高的召回率，说明模型的效果很好。但是，从实验结果可以看出，对于一个检索系统来讲，召回率和精度可能两全其美：召回率高时，精度低，精度高时，召回率低。所以，为了准确评价模型的效果，将两者融合，得到两个的调和平均值F1。由于两个指标相互制约，根据需要为 F1选择一个合适的度，寻求召回率和准确率中间的一个平衡点。



