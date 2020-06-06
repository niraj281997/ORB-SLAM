#include <stdio.h>
int swap(int n)
{
	return (((n&0x0f)<<4)|((n&0xf0)>>4));
}
int main()
{
	int n;
	printf("Number : ");
	scanf("%d",&n);
	printf("\n Swap%d\n",swap(n));
}
