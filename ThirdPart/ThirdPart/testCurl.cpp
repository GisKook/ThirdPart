#include "CNCurl.h"
using namespace std;

int main(){
	CNCurl a;
	a.SetURL("http://search1.mapabc.com/sisserver?config=RGC&flag=true&resType=xml&x1=116.483643&y1=39.969994&cr=0&a_k=ae576014c2a7a5272be8b7a0801940aa061af2f3d05e7efe581833e920a827f55a0f1f8bca02bdb7");
	string b = a.GetResult();
	a.CleanUp();
	a.SetURL("http://www.baidu.com");
	b = a.GetResult();
	return 0;
}