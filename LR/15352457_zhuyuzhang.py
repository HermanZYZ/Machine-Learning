from numpy import *
import matplotlib.pyplot as plt

data_infile = "D:\\Junior Three\\Artificial Intelligence\\Lab5_LR\\train.csv"
test_infile = "D:\\Junior Three\\Artificial Intelligence\\Lab5_LR\\test.csv"

yanshou_tr = "D:\\Junior Three\\Artificial Intelligence\\Lab5_LR\\thurs9-10\\train.csv"
yanshou_te = "D:\\Junior Three\\Artificial Intelligence\\Lab5_LR\\thurs9-10\\test.csv"

pro_2_cla = "D:\\Junior Three\\Artificial Intelligence\\Project\\binary_classification\\new_train.csv"
pro_2_test = "D:\\Junior Three\\Artificial Intelligence\\Project\\binary_classification\\new_test.csv"

IteraterTime = 490 #867
DIVIDE = (0.75, 0.25) #训练集和验证集的比例，四舍五入用round，返回浮点数

def Read_data(infile, Col, is_test = False) : # 读入数据集，路径，列数，是否为测试集
    data = loadtxt(infile, dtype = float, usecols = range(Col) , delimiter=',')
    row = len(data)
    add = ones(row)
    if not is_test :
        label = data[:,-1]
        data = c_[add, data[:,:-1]]
        return data, label
    else :
        data = c_[add, data]
        return data

def sigmoid(v) : #logistics函数
    return 1.0 / (1.0 + exp(-v))

def pidDynamic(w, err, lastError, accumulateError) :
    errorDifferential = (err - lastError) / 0.0001
    lastError = err
    accumulateError = accumulateError + err
    kp = 0.5
    ki = 3
    kd = 0.012
    return w - (kp * err + ki * accumulateError + kd * errorDifferential), lastError, accumulateError

def LR(eta, trainDataSet, trainLabelSet, IteraterTime) :
    w = zeros(trainDataSet.shape[1])              #这里列数为维度数，行数为样本数
    wt = dot(trainDataSet, w)
    lastError = dot((sigmoid(wt) - trainLabelSet), trainDataSet)
    accumulateError = zeros(trainDataSet.shape[1])
    for i in range(IteraterTime) :
        wt = dot(trainDataSet,w)
        err = dot((sigmoid(wt) - trainLabelSet), trainDataSet)
        if sqrt(dot(err,err)) < 0.00001 :
            break
        else :
            w, lastError, accumulateError = pidDynamic(w, err, lastError, accumulateError)
            # w = w - eta * err
    return w

def regularLR(eta, trainDataSet, trainLabelSet, IteraterTime, Lambda) :
    w = ones(trainDataSet.shape[1])              #这里列数为维度数，行数为样本数
    for i in range(IteraterTime) :
        wt = dot(trainDataSet,w)
        err = dot((sigmoid(wt) - trainLabelSet), trainDataSet)
        err = err + Lambda * sum(w)
        err /= trainDataSet.shape[0]
        # if sqrt(dot(err,err)) < 0.00001 :
        #     break
        # else :
        w = w - eta * err
    return w

def LRSingle(eta, trainDataSet, trainLabelSet, IteraterTime) :
    w = ones(trainDataSet.shape[1])              #这里列数为维度数，行数为样本数
    for i in range(IteraterTime) :
        for j in range(trainDataSet.shape[0]) :
            wt = trainDataSet[j] * w
            err = (sigmoid(wt) - trainLabelSet[j]) * trainDataSet[j]
            if len(err.nonzero()) == 0 :
                break
            else :
                w = w - eta * err
    return w

def Classify(w, DataSet):
    valProbability = sigmoid(dot(DataSet, w))
    predictLabel = valProbability > 0.5
    return predictLabel

def Validation(w, valDataSet, valLabelSet) :
    result = Classify(w, valDataSet)
    TP = FP = TN = FN = 0
    for i in range(len(result)) :
        if result[i] == valLabelSet[i] and valLabelSet[i] == 1:
            TP = TP + 1
        elif result[i] != valLabelSet[i] and valLabelSet[i] == 0 :
            FP = FP + 1
        elif result[i] == valLabelSet[i] and valLabelSet[i] == 0 :
            TN = TN + 1
        else :
            FN = FN + 1
    # print(TP, FP, TN, FN)
    accuracy = (TP + TN) / (TP + FP + TN + FN)
    if TP == FP == 0 :
        recall = 0
    else:
        recall = 1.0 * TP / (TP + FN)
    if TP == FP == 0:
        precision = 0
    else:
        precision = 1.0 * TP / (TP + FP)
    if TP != 0 :
        f1 = 2.0 * precision * recall / (precision + recall)
    else:
        f1 = 0
    return accuracy, recall, precision, f1

if __name__ == '__main__' :
    data, label = Read_data(pro_2_cla, 8)
    # test_data = Read_data(test_infile,40, True)
    #
    # ys_data, ys_label = Read_data(yanshou_tr, 7)
    # ys_te_data = Read_data(yanshou_te, 6, True)
    # ys_w = LR(1, ys_data,ys_label,1)
    # ys_pre_label = Classify(ys_w, ys_te_data)
    # print (ys_pre_label)

    # eta = [i for i in range(100,1000)]
    eta = [392]
    numberOfLine = int(round(DIVIDE[0] * data.shape[0])) #data.shape返回一个元组，值分别为行数和列数
    # re_w = regularLR(0.013, data[:numberOfLine], label[:numberOfLine], 200, 575)
    # predictLabel = Classify(re_w, test_data)
    # f = open("testResult.txt", 'w')
    # for i in predictLabel:
    #     if i :
    #         f.write(str(1))
    #     else:
    #         f.write(str(0))
    #     f.write('\n')
    best_w = []
    best_f1 = 0
    Result = [[],[],[],[]]
    for i in eta : #575 0.7855
        re_w = LR(0.013,data[:numberOfLine], label[:numberOfLine], i)
        accuracy, recall, precision, f1 = Validation(re_w, data[numberOfLine:data.shape[0]], label[numberOfLine:label.shape[0]])
        if f1 > best_f1:
            best_w = re_w
        Result[0].append(accuracy)
        Result[1].append(recall)
        Result[2].append(precision)
        Result[3].append(f1)
    # w = LR(eta, data[:numberOfLine], label[:numberOfLine], IteraterTime)#Batch Gradient
    # Validation(w, data[numberOfLine:data.shape[0]], label[numberOfLine:label.shape[0]])

    # for i in eta :
    #     w_single = LR(0, data[:numberOfLine], label[:numberOfLine], i)#Single Gradient
    #     accuracy, recall, precision, f1 = Validation(w_single, data[numberOfLine:data.shape[0]], label[numberOfLine:label.shape[0]])
    #     Result[0].append(accuracy)
    #     # Result[1].append(recall)
    #     # Result[2].append(precision)
    #     # Result[3].append(f1)

    print(re_w)
    print(eta[Result[0].index(max(Result[0]))])
    print("F1",max(Result[3]))
    print(Result)
    plt.plot(eta, Result[0], label="Accuracy",lw=1)
    plt.plot(eta, Result[1], label="Recall")
    plt.plot(eta, Result[2], label="Precision")
    plt.plot(eta, Result[3], label="F1")
    # plt.plot(eta, singleResult, label = "Single Gradient")
    plt.grid()
    plt.xlabel('Lambda')
    plt.ylabel('Accuracy')
    plt.legend()
    plt.show()

    test_data = Read_data(pro_2_test, 7, True)
    predictLabel = Classify(best_w, test_data)
    f = open("Result.csv", 'w')
    for i in predictLabel:
        if i :
            f.write(str(1))
        else:
            f.write(str(0))
        f.write('\n')