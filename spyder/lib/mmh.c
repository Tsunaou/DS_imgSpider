/*gcc -o mmhlib.so -shared -fPIC mmh.c*/
#include <string.h>
unsigned int murmurHash(const char *key, const int seed)
{
    //由随机种子seed计算字符串key的哈希值

    int len = strlen(key); //获取字符串长度
    const unsigned int m = 0x5bd1e995;
    const int r = 24;
    unsigned int h = seed ^ len; //异或运算
    // Mix 4 bytes at a time into the hash
    const unsigned char *data = (const unsigned char *)key;
    while(len >= 4)
    {
        unsigned int k = *(unsigned int *)data; //四个字节解码为int
        k *= m;
        k ^= k >> r; //移位运算:k右移r位
        k *= m;
        h *= m;
        h ^= k;
        data += 4;
        len -= 4;
    }
    // Handle the last few bytes of the input array
    switch(len)
    {
    case 3:h ^= data[2] << 16;
    case 2:h ^= data[1] << 8;
    case 1:h ^= data[0];h *= m;
    };
    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}
