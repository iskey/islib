#include "string.h"
#include "stdlib.h"

#define max(a, b) (((a)> (b))? (a): (b))

int lengthOfLongestSubstring(char *str)
{
    //longest substring
    int ans = 0;
    //The left position of the illegal string
    int left = 0;
    int i = 0;
    int len = strlen(str);
    int last[255];
    memset(last, -1, sizeof(last));

    for(i = 0; i < len; i++){
        if(last[str[i]] >= left) left = last[str[i]] + 1;
        last[str[i]] = i;
        ans = max(ans, i - left + 1);
    }

    return ans;
}

char *longestSubstring(char *str)
{
    //longest substring
    int ans = 0;
    //The left position of the illegal string
    int left = 0;
    //The max left
    int max_left = 0;
    int max_right = 0;
    int i = 0;
    int len = strlen(str);
    int last[255];
    memset(last, -1, sizeof(last));

    for(i = 0; i < len; i++){
        if(last[str[i]] >= left) left = last[str[i]] + 1;
        last[str[i]] = i;
        if((i - left + 1) > ans){
            max_left = left;
            max_right = i;
            ans = i - left + 1;
        }
    }

    //printf("max_left = %d , max_right = %d.\n", max_left, max_right);

    if (max_right <strlen(str))
        str[max_right + 1] = 0;

    return str + max_left;
}

int main(int argc, char *argv[])
{
    printf("The Length of longest substring is %d\n", lengthOfLongestSubstring(argv[1]));
    printf("The longest substring is %s\n", longestSubstring(argv[1]));

    return 0;
}
