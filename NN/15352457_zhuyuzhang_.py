from numpy import *
import matplotlib.pyplot as plt

data_infile = "D:\\Junior Three\\Artificial Intelligence\\Lab6_NN\\BPNN_Dataset\\train.csv"
test_infile = "D:\\Junior Three\\Artificial Intelligence\\Lab6_NN\\BPNN_Dataset\\test.csv"

eta = 0.9

DIVIDE = (0.75, 0.25) #训练集和验证集的比例，四舍五入用round，返回浮点数

def Read_data(infile, Col, is_test = False) : # 读入数据集，路径，列数，是否为测试集
    data = loadtxt(infile, dtype = float, skiprows = 0, usecols = range(2,Col) , delimiter=',')
    row = len(data)
    add = ones(row)
    if not is_test :
        label = data[:,-1]
        data = c_[data[:,:-1], add]
        return data, label
    else :
        data = c_[data, add]
        return data

def sigmoid(v) : #logistics函数
    return 1.0 / (1.0 + exp(-v))

def init_hidden_layers_w(nodes, column):
    w = []
    for i in range(nodes):
        w.append(random.normal(0, 0.01, column))
    return array(w)

def init_output_layer_w(column):
    return array(random.normal(0, 0.01, column))

def forward(x,wh, if_wo = False):
    # Oj = sigmoid(dot(x, wh.T))
    # Oj1 = dot(x[:,:-1], wh[:,:-1].T) + wh[:,-1]
    # Oj1 = sigmoid(Oj1)
    # y = dot(Oj, wo[:-1].T) + wo[-1]
    if not if_wo:
        return sigmoid(dot(x, wh.T))
    else:
        return dot(x, wh.T)

def output_layer_error(O, T):
    return  (T - O)

def hidden_layer_error(O, Errk, wjk):
    Temp_Errk = Errk.T
    for i in range(O.shape[1] - 1):
        Temp_Errk = c_[Temp_Errk, Errk]
    return O * (1 - O) * Temp_Errk * wjk

def backward(Ok, Oj, T, wo, wh, x):
    Errk = output_layer_error(Ok, T)
    Errj = hidden_layer_error(Oj, Errk, wo[:-1])
    # print("****")
    # print(dot(c_[Oj, ones(Oj.shape[0])].T, Errk))
    wo = wo + eta * dot(c_[Oj, ones(Oj.shape[0])].T, Errk) / x.shape[0]
    for i in range(wh.shape[0]):
        wh[i] = wh[i] + eta * dot(x.T, Errj[:,i]) / x.shape[0]
    return wo, wh

def update_wo(Ok, T):
    pass

if __name__ == '__main__' :
    # T = array([1,1])
    # x = array([[1,0,1,1],[1,0,1,1]])
    # wh = array([[0.2,0.4,-0.5,-0.4],[-0.3,0.1,0.2,0.2]])
    #
    # wo = array([-0.3,-0.2,0.1])
    # y, Oj = forward(x,wh,wo)
    # print(y)
    # wo, wh = backward(y, Oj, T, wo, wh, x)
    # print(wo)
    # print(wh)

    data, label = Read_data(data_infile, 26)
    numberOfLine = int(round(DIVIDE[0] * data.shape[0]))  # data.shape返回一个元组，值分别为行数和列数
    Times = 100
    nodes = 128
    LayersOfHidden = 2
    # 初始化每一层隐藏层的w
    wh = init_hidden_layers_w(nodes, data[0].shape[0])
    W = [wh]
    for i in range(1,LayersOfHidden):
        W.append(init_hidden_layers_w(nodes, W[i - 1].shape[1] + 1))
    # 开始前向传播

    OJ = []
    temp = data[:numberOfLine]
    for i in range(LayersOfHidden):
        oj= forward(temp,W[i])
        OJ.append(c_[oj, ones(oj.shape[0])])
        temp = OJ[i]
    wo = init_output_layer_w(OJ[-1].shape[1])
    Ok = forward(OJ[-1], wo, True)

