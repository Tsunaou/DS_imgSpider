import math
from bitarray import bitarray
import ctypes

lib = ctypes.cdll.LoadLibrary('lib/mmhlib.so')

class BF:
    def __init__(self,size,hash_count):#初始化布隆过滤器，size:布隆过滤器中二进制数组的大小，即m，hash_count:哈希函数的个数,即k

        self.bitArray = bitarray(size) #初始化二进制数组
        self.bitArray.setall(0) #二进制数组全部置0
        self.size = size 
        self.hashCount = hash_count

    def add(self,item):#向布隆过滤器中添加字符串元素item
        item = item.encode()#= =调用C就是坑，把字符串转化为二进制形式
        for i in range(self.hashCount):
            index = lib.murmurHash(item,i)%self.size #以i为随机种子得到哈希映射值
            self.bitArray[index] = True #将相应二进制数组位置置1

    def isContains(self,item):#判断元素item是否已经存储过

        item = item.encode()
        for i in range(self.hashCount):
            index = lib.murmurHash(item,i)%self.size #以i为随机种子得到哈希映射值
            if not self.bitArray[index]: #若有一个哈希函数没有映射过这样的字符，则认为没有存储过该元素
                return False
        return True #否则认为存储过该元素

def optimalParameters(eleNum,errors):# 通过给定的需要存储的元素个数和需要达到的误判率计算布隆过滤器中的最优二进制数组大小和哈希函数个数

    size = math.ceil(-eleNum*math.log(errors)/(math.log(2)*math.log(2)))
    hashCount = math.ceil(math.log(2)*size/eleNum)
    return size,hashCount

