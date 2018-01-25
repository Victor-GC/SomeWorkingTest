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
void QuickSort(int* arrays, int start, int end);
int QuickSortCore(int* arrays, int start, int end);

int main()
{
    int n;
    scanf("%d",&n);

    int *arrays = (int*)malloc(sizeof(int) * n);
    int *arrays1 = (int*)malloc(sizeof(int) * n);
    /*
    for (int i = 0;i < n;++i){
        scanf("%d",arrays + i);
    }
    for (int i=0;i < n;++i){
        printf("%d ",arrays[i]);
    }
    printf("\n");
    */

    srand((int)time(0));
    for(int i = 0 ;i < n;++i){
        arrays[i] = rand();
        arrays1[i] = arrays[i];
    }
    
    int start = 0;
    int end = 0;
    while (1){
        start = 0;
        end = 0;
        printf("请输入要查询的区间，格式：start,end\n");
        scanf("%*[^\n]");
        scanf("%*c");
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
        printf("冒泡算法求得区间[%d,%d]内的最小值为：%d\n",start + 1,end + 1,min);
        
        QuickSort(arrays1,start,end);
        min = arrays1[start];
        printf("快排算法求得区间[%d,%d]内的最小值为：%d\n",start + 1,end + 1,min);
        
        printf("是否继续？y/n\n");
        char index = 'n';
        scanf("%*[^\n]");
        scanf("%*c");
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

void QuickSort(int* arrays, int start, int end)
{
    if (start >= end){
        return;
    }
    int pivot = QuickSortCore(arrays, start,end);
    QuickSort(arrays,start,pivot-1);
    QuickSort(arrays,pivot+1,end);
}

int QuickSortCore(int* arrays, int start, int end)
{
    int pivot = arrays[end];
    int i = start -1;
    for (int j = start; j < end; ++j){
        if (arrays[j] <= pivot){
            i += 1;
            int temp = arrays[i];
            arrays[i] = arrays[j];
            arrays[j] = temp;
        }
    }

    arrays[end] = arrays[i+1];
    arrays[i+1] = pivot;
    
    return i+1;
}
