import re,requests,urllib
import queue
import time
import os
import threading
from lib import bloomFilter

hd = {'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.63 Safari/537.36 Qiyu/2.1.0.0'}
#伪装成浏览器，防止被查水表
imaRe = re.compile( 'src="(http.[^ ^\t^\r^\n^]*?\.)(jpg|png|jpeg|gif|bmp)"',re.S )
htmlRe = re.compile( 'href="(http.[^ ^\t^\r^\n^]*?)"',re.S )#正则匹配网址

class imaScript:
    def __init__(self,htmlStart,rootPath,htmlMaxCount=1000,imaMaxCount=10000,errors=0.001,lifetime=2,timeout=1):
        '''
            htmlStart:网络图片爬取的起始网页
            rootPath:图片存储的根目录
            htmlMaxCount:需要爬取的最大网址数
            imaMaxCount:需要爬取的最大图片数
            errors:布隆过滤器需要达到的误判率
            lifetime:每个网址在爬取过程中的最大重复请求数
            timeout:每次请求的最大时长，以秒为单位
        '''
        if not rootPath.endswith('\\'): #让存储目录以'\'结尾，方便存储目录的层次化实现
            rootPath += '\\'
        if not os.path.exists(rootPath):
            os.mkdir(rootPath)
        self.rootPath = rootPath
        self.htmlQueue = queue.Queue()
        self.htmlStart = htmlStart
        self.htmlQueue.put(htmlStart) #将起始网址与以之对应的图片存放文件夹入队列
        self.imaId = 0 #记录下载的图片数量
        self.imaFailCnt = 0 #记录下载失败的图片数量
        self.htmlId = 0 #记录爬取的网址数量
        self.htmlFailCnt = 0 #记录爬取失败的网址数量
        self.htmlMaxCount = htmlMaxCount 
        self.imaMaxCount = imaMaxCount
        par = bloomFilter.optimalParameters(htmlMaxCount,errors) #计算布隆过滤器的最优m和k
        self.BFlist = bloomFilter.BF(par[0],par[1]) #初始化布隆过滤器
        self.BFlist.add(htmlStart) #将初始网址加入布隆存储器中
        self.threadLifetime = lifetime
        self.timeout = timeout

    def run(self,thread_num):

        if thread_num<1: #判断线程数是否小于1
            print('线程数不能小于1;')
            return
        thread_num += 1 #将主线程加上
        self.thread_num = thread_num
        st = time.time() #记录开始爬虫的时间
        print('爬虫开始;')
        self.threadings = list() #初始化线程队列
        self.__mulScript() #执行多线程爬虫方法
        #输出爬取的结果信息
        print('爬虫结束,总耗时%d秒,信息如下;'%(time.time()-st)) 
        print('爬取网站%d个,其中%d个爬取失败;'%(self.htmlId,self.imaFailCnt))
        print('其中爬取图片%d张,其中%d张下载失败;'%(self.imaId,self.imaFailCnt))
        print('原始图片已全部保存至目录%s下;'%self.rootPath)

    def __mulScript(self):#通过布隆过滤器的检验，将非重复的网址存入htmlQueue队列，同时在threading中添加相应的待执行线程，最后再执行。

        def htmlGet(lifetime,html):# 获取html中的所有子链接，并存入htmlQueue和threading两个队列中。

            if lifetime<=0: #当次数达上限后，放弃该网址的请求
                print('多次请求失败,放弃',html)
                return
            try: #尝试在timeout在时间内请求数据
                rq = requests.get(html,headers=hd,timeout=self.timeout)
            except: #若请求失败则重试，并lifetime-1
                print(html+'重新连接;')
                return htmlGet(lifetime-1,html)
            if rq.status_code!=requests.codes.ok: #若响应码异常则返回
                print()
                print(html)
                print('页面错误;')
                print()
                rq.close()
                return
            code = rq.content.decode('utf8','ignore') #以utf8编码解析网页内容，对于无法解码的情况忽略错误
            rq.close() #断开连接
            htmlList = re.findall( htmlRe,code ) #用正则表达式匹配页面中的所有跳转链接
            for i in htmlList: #遍历所有得到的跳转链接
                if len(self.threadings)>=self.htmlMaxCount: #若已达到了最大爬取网址数，则返回
                    return
                if self.BFlist.isContains(i): #若布隆过滤器检测到重复，则跳过该链接
                    continue
                print("捕获到过滤后的网址"+i)
                t = threading.Thread(target=self.__script,args=(self.threadLifetime,i,)) 
                self.threadings.append(t) #爬取该跳转链接的线程入线程队列
                self.htmlQueue.put(i) #html入htmlQueue队列，以供之后的深度优先遍历搜索
        t = threading.Thread(target=self.__script,args=(self.threadLifetime,self.htmlStart,))
        self.threadings.append(t) #将爬取根网址图片的线程入线程队列
        while not self.htmlQueue.empty(): #深度优先遍历搜寻跳转链接
            element = self.htmlQueue.get() #获取队头元素
            htmlGet(self.threadLifetime,element) #以该队头元素搜寻其子链接，并填充线程队列
            if len(self.threadings)>self.htmlMaxCount: #若达到了最大网页爬取数，则跳出搜索
                break
        print('总共找到网站%d个;'%len(self.threadings)) #输出信息
        self.htmlId = len(self.threadings) #记录总爬取网页数
        for t in self.threadings: #遍历线程队列
            t.setDaemon(True) #设置守护线程
            t.start() #开启线程
            while True: #死循环，直到正在运行的线程数少于最大线程数，才会开启新的线程
                if threading.activeCount()<self.thread_num: 
                    break
        for t in self.threadings: #阻塞主线程的执行
            t.join()


    def __script(self,lifetime,html):# 将html中的所有图片下载到rootPath中。

		
        if self.imaId>=self.imaMaxCount: #当图片数量达到最大图片数量时，返回
            return
        if lifetime<=0: #当多次请求失败后，放弃该地址
            print('多次请求失败,放弃',html)
            self.htmlFailCnt += 1
            return
        try: #尝试在指定的timeout时间内请求网页数据 
            rq = requests.get(html,headers=hd,timeout=self.timeout)
        except: #请求失败后再次尝试请求，并lifetime-1
            print(html+'重新连接;')
            return self.__script(lifetime-1,html)

        if rq.status_code!=requests.codes.ok: #若响应码异常则返回
            print()
            print(html)
            print('页面错误;')
            print()
            rq.close()
            return
        code = rq.content.decode('utf8','ignore') #以utf8编码解析网页内容，对于无法解码的情况忽略错误
        rq.close() #断开连接

        imaList = re.findall(imaRe,code) #利用正则表达式匹配网页中的所有图片url地址
        print('爬取网站:'+ html +' ')
        for imaLink in imaList: #遍历该网页的所有的图片url地址
            if self.imaId>=self.imaMaxCount: #若图片下载达数量上限，则返回
                return
            self.imaId += 1
            name = str(self.imaId) + '.%s'%( str(imaLink[1]) ) #图片存储的图片名
            try: #尝试下载图片
                urllib.request.urlretrieve(''.join(imaLink),self.rootPath+name)
            except: #若下载失败，则记录失败的次数，并输出信息
                self.imaFailCnt += 1
                print()
                print(''.join(imaLink))
                print("图片下载失败;")
                print()
