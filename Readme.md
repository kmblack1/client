测试工具每5次调用一次正确的用户名,其它情况用户名随机生成.
```bash
#linux
make
./client -u http://192.168.1.252:7654 -t 16 -n 100000
#windows,在cmd中直接切换到Release目录
chcp 65001
client -u http://192.168.1.252:7654 -t 16 -n 100000
```
