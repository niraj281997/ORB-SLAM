#include <stdio.h>
#include <stdlib.h>

int main()
{
	int n,*arr,i=0,j=0,k=0,temp1,temp;
	while(n<3){
	printf("\nEnter a number : ");
	scanf("%d",&n);
	if(n<3)
	printf("\n\nEnter a number greater then 3 ");
	}
	printf("\nWhich triple addition number you want to find: ");
	scanf("%d",&temp1);
	arr=calloc(n,sizeof(int));
	while(i<n)
	{
		printf("array[%d]=",i+1);
		scanf("%d",(arr+i++));
	}
/*	for(i=0;i<n-1;i++)
		for(j=0;j<n-1;j++)
			if(arr[j]>arr[j+1])
			{
				temp=arr[j];
				arr[j]=arr[j+1];
				arr[j+1]=temp;
			}
	for(i=0,j=n-1;i<j;i++,j--)
	{
				temp=arr[j];
				arr[j]=arr[j+1];
				arr[j+1]=temp;
	}
	*/

	for (i=0;i<n-2;i++)
		for (j=i+1;j<n-1;j++)
			for(k=j+1;k<n;k++)
				if(arr[i]+arr[j]+arr[k]==temp1)
					printf("arr[%d]+arr[%d]+arr[%d]==%d",i,j,k,temp1);
				else 
					printf("Result not found ");



}
