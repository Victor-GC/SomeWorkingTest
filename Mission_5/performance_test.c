/*************************************************************************
	> File Name: performance_test.c
	> Author:Victor Geng 
	> Mail: gengchao23@163.com
	> Created Time: 2018年01月24日 星期三 11时25分32秒
 ************************************************************************/

#include<stdio.h>
#include<time.h>
#include<stdlib.h>

int BubbleSort(int* arrays, int start, int end);

int main()
{
    int n;
    scanf("%d",&n);

    int *arrays = (int*)malloc(sizeof(int) * n);
    for (int i = 0;i < n;++i){
        scanf("%d",arrays + i);
    }
    for (int i=0;i < n;++i){
        printf("%d ",arrays[i]);
    }
    printf("\n");

    /*srand((int)time(0));
    for(int i=0;i<n;++i){
        arrays[i]=rand();
    }*/
    
    int start = 0;
    int end = 0;
    while (1){
        start = 0;
        end = 0;
        printf("请输入要查询的区间，格式：start,end\n");
        if (!scanf("%d,%d",&start,&end)){
            printf("输入非法！\n");
            continue;
        }
        start -= 1;
        end -= 1;
        if (start < 0 || end < 0|| start >= n || end >=n){
           printf("输入查询范围有误，请重新输入！\n\n");
            continue;
        }

        int min = BubbleSort(arrays,start,end);
        printf("区间[%d,%d]内的最小值为：%d\n",start,end,min);
        
        printf("是否继续？y/n\n");
        char index = 'n';
        scanf("%c",&index);
        if (index == 'n'){
            break;
        }

        printf("\n");
    }


    free(arrays);
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
    return arrays[start];
}
