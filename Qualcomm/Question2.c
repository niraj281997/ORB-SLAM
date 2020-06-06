#include <stdio.h>
int ret(int n,int k)
{
	if(k==1)
		return n;
	int res=1;
		while(res<=k)
			res<<=1;
		return (res-1);
}
int main()
{
	int n,k;
	printf("Enter n and k : ");
	scanf("%d%d",&n,&k);
	printf("%d",ret(n,k));
}
