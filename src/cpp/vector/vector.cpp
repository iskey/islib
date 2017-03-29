#include <vector>
#include "stdio.h"

using namespace std;

struct Student
{
	char name[10];
	int age;
	int gender;
};

int main()
{
	vector<struct Student> stu;

	struct Student t={"xie", 30, 1};
	struct Student t1={"gao", 24, 0};
	struct Student t2={"qi", 2, 0};

	stu.push_back(t);
	stu.push_back(t1);
	stu.push_back(t2);

	struct Student  *stu_arry;
	stu_arry = &stu[0];

	int i;
	for(i=0; i<3; i++)
	{
		printf("Name:%s, age:%d, gender:%d\n", stu_arry[i].name, stu_arry[i].age, stu_arry[i].gender);
	}

	return 0;
}
