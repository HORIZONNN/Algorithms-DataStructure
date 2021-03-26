#include <iostream>
#include <stack>
#include <stdexcept>
#include <exception>
#include <string.h>
#include <math.h>
#include <vector>
#include "main.h"
#include <queue>

using namespace std;


// 一个是升序降序的排列，写一个reverse函数，另一个是输入合法性，可以考虑用 #define 来统一写

/*
************************************************************************************************************************
------------------------------------------------**  排序算法性质对比  **------------------------------------------------

            时间（好/平均/坏）  空间  稳定性  中间结果    基本思想

插入排序      N / N2  / N2       1    稳定    相对排序    找新元素在已排序数组中的相对位置

选择排序     N2 / N2  / N2       1    不稳定  绝对排序    找到未排序元素中的最小（大）值

冒泡排序      N / N2  / N2       1    稳定    绝对排序+   找到未排序元素中的最大（小）值，并相对移动中间元素位置。
                                              相对移动    相当于改进的选择排序，当原数组排序情况较好时时间复杂度大幅下降

希尔排序    N1.3/  N2 / N2       1    不稳定  多重相对    根据递减的间隔序列，依次执行多次插入排序。相当于对于不同的排序
                                              排序        难度优化过的插入排序

堆排序     NlogN/NlogN/NlogN     1    不稳定  绝对排序    利用最大（小）堆的数据结构特征来找最大最小值，对比选择排序就是
                                                          把找最大（小）值的方法改成了堆，效率更高，实现logN的查找速度

快速排序   NlogN/NlogN/ N2       1    不稳定  相对排序    利用分治法的思想，采用递归的设计方式，利用巧妙的划分来排序

归并排序   NlogN/NlogN/NlogN     n    稳定    相对排序    利用分治法的思想，先划分，然后排序子问题，然后再归并

基数排序    n*k / n*k / n*k     n+k   稳定    相对排序    对于数字类数据，按位进行比较排序，主要是能通过索引散列的方式进
                                                          行简单的划分，适合数据多又值不大的数据排序

计数排序    n+k / n+k / n+k     n+k   稳定    相对排序    通过计数统计的方式来提高时间效率，适用于小范围多数据的排序
------------------------------------------------------------------------------------------------------------------------
************************************************************************************************************************
*/


/*
选择排序的不稳定是由于跨元素进行元素内容交换导致的
希尔排序的不稳定是由于同一间隔下，不同的间隔子数组的排序不同导致的
堆排序的不稳定是由于堆的构建造成的，不保证相对顺序
*/


template<typename T>
void Swap(T& data1, T& data2)
{
    T temp = data1;
    data1 = data2;
    data2 = temp;
}

/*
********************************************************************************
--------------------------------**  插入排序  **--------------------------------

 插入排序就是先排好前i个元素，再将第i+1个元素插入到前面已排好的元素中的合适位置
 这样就又保证了前i+1个元素是排序的，然后循环到所有元素排好序
********************************************************************************
*/
template<typename T>
void insertSort(T data[], int length, bool ascend=true)
{
    if (data == nullptr || length == 0)
        return;
    // 假定前i个元素已经排序好
    for (int i=1; i<length; i++)
    {
        // 将第i个元素插入到前面排序好的数组的合适位置中
        int j;
        T temp = data[i];
        // 内层循环需要注意，如果将j赋值成i-1，则后续需要改成data[j+1] = temp;
        for(j=i; j>0 && temp<data[j-1]; j--)
            data[j] = data[j-1];
        data[j] = temp;
    }
}

/*
********************************************************************************
--------------------------------**  选择排序  **--------------------------------

 查找数组中元素最小（大）的，与数组首元素交换位置；
 然后数组low的大小+1，继续查找从而保证每次查找的都是剩余数组中的最小的
********************************************************************************
*/
template<typename T>
void selectSort(T data[], int length, bool ascend=true)
{
    if (data == nullptr || length == 0)
        return;
    // i表示首元素的索引
    for (int i=0; i<length-1; i++)
    {
        int minIndex = i;             // 先暂时把头元素设为最小
        for (int j=i+1; j<length; j++)     // 然后从头元素后面一个元素开始比较
            if (data[j] < data[minIndex]) minIndex = j;
        if (i != minIndex)
            Swap(data[i], data[minIndex]);       // 交换最小的元素与首元素的位置

    }
}

/*
********************************************************************************
--------------------------------**  冒泡排序  **--------------------------------

 从头开始向尾遍历，如果后面的数比前面的数小，就交换，一直到最后,这样能够保证交换
 到最后最大（小）的数一定在最后一位。然后再缩小区间（去掉最后一位），继续进行交
 换。
********************************************************************************
*/
template<typename T>
void bubbleSort(T data[], int length, bool ascend=true)
{
    // i标识当前未排序数组的最大下标
    for (int i=length-1; i>0; i--)
    {
        bool flag = true; // 数组是否排好序的标识
        // 遍历从0到i的整个区域
        for(int j=0; j<i; j++)
        {
            // 如果发现相对位置不对的元素对，就交换两者位置
            // 把flag设为false标识当前数组还没排序好，这是最好时间复杂度能为n
            // 的关键
            if (data[j] > data[j+1])
            {
                Swap(data[j], data[j+1]);
                flag = false;
            }
        }
        // 如果当前这次循环flag没被改变，说明数组已被排好序，直接返回
        if (flag) return;
    }
}

/*
********************************************************************************
--------------------------------**  希尔排序  **--------------------------------

每次将原数组中间隔为gap的gap个子数组排序（一般都是插入排序），然后减小gap的值重
复这一过程，直到gap=1排序完成。其中gap的所有取值构成一个增量子序列。

例如：[5, 1, 2, 8, 4]
令Gaps = [1, 2]
gap = 2时，分别排序 [2,5] 和 [1,8,4] 得到 [2,1,5,4,8]
gap = 1时，排序 [2,1,5,4,8] 得到[1,2,4,5,8]，排序结束。
********************************************************************************
*/
template<typename T>
void shellSort(T data[], int length, bool ascend=true)
{
    // 定义gap递增子序列，这里取子序列按照3次幂变大，这个随意，只要第一位是1就行
    int gapIndex=0, gap = 1, Gaps[20];
    while(gap < length/3 || gap == 1)
    {
        Gaps[gapIndex++] = gap;
        gap = gap*3 + 1;
    }
    // 不同gap的循环
    for(gapIndex--; gapIndex>=0; gapIndex--)
    {
        gap = Gaps[gapIndex];
        // 内层就是一个插入排序
        for (int index=gap; index<length; index++)
        {
            T temp = data[index];
            int subIndex;
            for (subIndex = index; subIndex >= gap && temp < data[subIndex-gap]; subIndex -= gap)
                data[subIndex] = data[subIndex - gap];

            data[subIndex] = temp;
        }
    }

}

/*
********************************************************************************
---------------------------------**  堆排序  **---------------------------------

就是先将数组构建成一个最大堆，这样就能保证堆顶的元素是最大（小）值，然后将其与末
尾的元素互换。然后再将剩余的前n-1个元素构建成最大堆，循环重复下去。
基本想法和选择排序差不多，只不过是通过堆的形式来找到最大（小）值

注意：升序要用最大堆，降序要用最小堆。可能有人会想如果升序直接用最小堆，那么堆就
是头部节点，不就省去交换的操作了，会不会更简单。这样虽然省略了交换，但是每次重排
堆会很麻烦。因为用最大堆交换元素后剔除范围的是最后一个叶节点，而如果用最小堆，就
会把根节点剔除掉，这样整个堆都要重排，这样时间复杂度就更高了。
********************************************************************************
*/
// 当最大堆中first节点改变后，调整first的子孙节点使得first为根节点的子树依然是最
// 大堆，其中节点最大不超过last
template<typename T>
void moveAfterTopChange(T data[], int first, int last)
{
    int largest = 2*first + 1; // largest为first的左孩子
    while(largest <= last)
    {
        if (largest < last && data[largest] < data[largest+1]) // 如果右孩子比左孩子大，那么largest指向右孩子
            largest++;
        if (data[first] < data[largest]) // 如果孩子比first节点大，则交换两者的值，first指向largetst节点（刚刚被交换的节点）
        {                                // 然后依次循环下去直到原先堆顶的元素找到合适的位置
            Swap(data[first], data[largest]);
            first = largest;
            largest = 2*first+1;
        } else
            break;
    }
}

template<typename T
>
void heapSort(T data[], int length, bool ascend=true)
{
    // 首先把原数组构建成一个最大堆
    for (int i=length/2-1; i>=0; i--)
        moveAfterTopChange(data, i, length-1);

    for (int i=length-1; i>0; i--)
    {
        Swap(data[0], data[i]); // 每次交换堆顶（最大）元素和未排序的尾节点
        moveAfterTopChange(data, 0, i-1); // 调整堆，使其重新构成最大堆
    }
}

/*
********************************************************************************
--------------------------------**  快速排序  **--------------------------------

本质就是利用递归和二分的思想来提高排序速度。以当前头节点为参照，不断交换数组中数
的位置，最终找到一个位置使得左边的节点都小于这个参照，右边的节点都大于这个参照
********************************************************************************
*/

template<typename T>
int quickSortPartition(T data[], int low, int high)
{
    // 将low位置的数据设为最终分割点的数据
    T midData = data[low];
    while(low < high)
    {
        // 如果右侧有比当前midData小的元素，就将它换到low的位置
        while(low < high && midData <= data[high])
            high--;
        data[low] = data[high];
        // 如果左侧有比当前midData大的元素，就将它换到high的位置
        while(low < high && data[low] <= midData)
            low++;
        data[high] = data[low];
    }
    data[low] = midData;
    return low;
}

template<typename T>
void quickSortCore(T data[], int low, int high)
{
    if (low >= high)
        return;
    int mid = quickSortPartition(data, low, high);
    quickSortCore(data, low, mid-1);
    quickSortCore(data, mid+1, high);
}

template<typename T>
void quickSort(T data[], int length, bool ascend=true)
{
    quickSortCore(data, 0, length-1);
}


/*
********************************************************************************
--------------------------------**  归并排序  **--------------------------------

类似快速排序，也是采用递归分割的方法来实现低时间复杂度的排序算法，只不过不同于快
速排序在分割的过程中的排序的想法，归并排序是分割后再排序，然后通过merge函数再组
合到一起。
********************************************************************************
*/

template<typename T>
void mergeSortMerge(T data[], int low, int mid, int high)
{
    if(low > mid || mid > high)
        return;
    // 申请空间暂存结果
    T* temp = new T[high-low+1];
    int indexLow = low;
    int indexHigh = mid;
    int indexTemp = 0;
    // 循环对比前后数组大小依次加入
    while(indexLow < mid && indexHigh <= high)
    {
        if (data[indexLow] <= data[indexHigh])
            temp[indexTemp++] = data[indexLow++];
        else
            temp[indexTemp++] = data[indexHigh++];
    }
    // 把前后数组中剩余的元素直接加入（前面的循环结束意味着必定有一个数组已经遍历到头了）
    while(indexLow < mid)
        temp[indexTemp++] = data[indexLow++];
    while(indexHigh <= high)
        temp[indexTemp++] = data[indexHigh++];

    // 循环遍历，将暂存数组的结果复制到原数组中
    indexTemp--;
    indexHigh--;
    while(indexHigh >= low)
        data[indexHigh--] = temp[indexTemp--];

    delete[] temp;
}

template<typename T>
void mergeSortCore(T data[], int low, int high)
{
    // 递归终止条件
    if (low >= high)
        return;
    int mid = (low + high) / 2;
    // 先进行递归排序操作
    // 这里一定要设成mid而不是mid-1。因为 / 操作是向下取整。如果是mid那么在low=0，high=1这种情况的时候就会无限递归
    mergeSortCore(data, low, mid);
    // 这里一定要射程mid+1而不是mid，跟上面那个配合
    mergeSortCore(data, mid+1, high);
    // 然后进行merge组合上述已排序的前后两个序列
    mergeSortMerge(data, low, mid, high);
}

template<typename T>
void mergeSort(T data[], int length, bool ascend=true)
{
    // 直接调用Core函数，其实就是为了转换普通调用接口到递归调用接口上
    mergeSortCore(data, 0, length-1);
}

/*
********************************************************************************
--------------------------------**  基数排序  **--------------------------------

基数排序只适用于数字类型的数据排序，从低位到高位进行循环吗，按位数字大小进行排序
********************************************************************************
*/

template<typename T>
void radixSort(T data[], int length, bool ascend=true)
{
    int radix = 10;
    queue<T> queues[radix];
    int flag=true; // 是否继续进位排序的标识
    int factor = 1;
    while(flag)
    {
        flag = false;
        int index;
        // 将各个数据按照对应位数的模值的结果加入到相应的队列中
        for (int i = 0; i < length; i++)
        {
            index = data[i] / factor % radix;
            queues[index].push(data[i]);
            if (index != 0) flag = true; // 如果存在模值不为0的数，就说明还没进行到最高位，就需要接着排序
        }
        // 按位再把各个队列中的数据循环加入到原数组中
        for (int i=0, k =0; i < radix; i++)
        {
            while(!queues[i].empty())
            {
                data[k++] = queues[i].front();
                queues[i].pop();
            }
        }

        factor *= radix;
    }
}


/*
********************************************************************************
--------------------------------**  计数排序  **--------------------------------

先找出这一区间内的最大值和最小值，然后遍历统计落在最大和最小值之间各个数的频次，
然后依次填充到数组中就可以了。算是一种极特殊的桶排序。
********************************************************************************
*/

template<typename T>
void countSort(T data[], int length, bool ascend=true)
{
    T minValue = data[0], maxValue = data[0];
    for (int i=0; i<length; i++)
    {
        if (data[i] < minValue) minValue = data[i];
        if (data[i] > maxValue) maxValue = data[i];
    }

    T* counts = new T[maxValue - minValue + 1];
    memset(counts, 0, sizeof(T)*(maxValue - minValue + 1));
    for (int i=0; i<length; i++)
        counts[data[i] - minValue]++;

    int index = 0;
    for (int i=0; i<maxValue-minValue+1; i++)
    {
        while(counts[i]-- != 0)
            data[index++] = minValue+i;
    }

    delete[] counts;
}


/*
********************************************************************************
---------------------------------**  桶排序  **---------------------------------


********************************************************************************
*/


int main()
{
    int priceArr[] = {11,28,0,15, 17,15, 13};
    int ironLength = 7;
    countSort(priceArr, ironLength);
    for (int i=0; i<ironLength; i++)
        cout << priceArr[i] << " ";
    cout << endl;
    return 0;
}
