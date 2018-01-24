/*************************************************************************
	> File Name: performance_test.c
	> Author:Victor Geng 
	> Mail: gengchao23@163.com
	> Created Time: 2018年01月24日 星期三 11时25分32秒
 ************************************************************************/

#include<stdio.h>
#include<time.h>

int BubbleSort(int* arrays, int start, int end);

int main()
{
    int n;
    scanf("%d",&n);

    int *arrays = new int[n];
    for (int i = 0;i < n;++i){
        scanf("%d",arrays + i);
    }
    
    
    /*srand((int)time(0));
    for(int i=0;i<n;++i){
        arrays[i]=rand();
    }*/
    
    int start = 0;
    int end = 0;
    while (1){
        printf("请输入要查询的区间，格式：[start,end]\n");
        scanf("[%d,%d]",&start,&end);
        int min = BubbleSort(arrays,start,end);
        printf("最小值为：%d\n",min);
        printf("是否继续？y/n\n");
        char index = 'n';
        scanf("%c",&index);
        if (index == 'n'){
            break;
        }
    }


    delete arrays;
}

int BubbleSort(int* arrays, int start, int end)
{
    int n=end-start+1;
    
    if (n<=0)
       return 0;

    for (int i = start;i<n;++i){
        for (int j = i;j<n;++j){
            if (arrays[i] > arrays[j]){
                int temp = arrays[i];
                arrays[i] = arrays[j];
                arrays[j] = temp;
            }
        }
    }
    return arrays[i];
}
