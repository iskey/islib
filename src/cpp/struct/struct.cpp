#include <vector>
#include <string>
#include <algorithm>
#include "stdio.h"
#include "string.h"
#include <iostream>

using namespace std;

struct Student
{
	char name[10];
	int age;
	int gender;
};

struct Friend
{
	string name;
	int age;
	Friend(string name, int age) {this->name = name ; this->age = age;}
	bool operator ==(const struct Friend &i_friend) const {return this->name == i_friend.name;}
};

struct MyStudent: public Student
{
	vector<struct Friend> friends;

	bool addFriend(struct Friend i_name);
	bool isMyFriend(struct Friend i_name);
	bool setName(string i_name);
	bool showFriends();
};

bool MyStudent::isMyFriend(struct Friend i_friend)
{
	if (this->friends.empty())
		return false;

	vector<struct Friend>::iterator it = find(this->friends.begin(), this->friends.end(), i_friend);
	if (it != this->friends.end())
	{
		cout << "You are already my friend: " << (*it).name << endl;
		return true;
	}
	else
		return false;
}

bool MyStudent::addFriend(struct Friend i_friend)
{
	if(!this->isMyFriend(i_friend))
		this->friends.push_back(i_friend);
}

bool MyStudent::setName(string i_name)
{
	strncpy(this->name, i_name.c_str(), sizeof(this->name));
	return true;
}

bool MyStudent::showFriends()
{
	while(!this->friends.empty())
	{
		struct Friend friend_name = this->friends.back();
		cout<< friend_name.name << endl;
		this->friends.pop_back();
	}
}
int main()
{
	vector<struct Student> stu;

	struct MyStudent foo;
	foo.setName("xie");
	foo.age= 30;
	foo.gender = 1;

	foo.addFriend(Friend("xiaoming", 10));
	foo.addFriend(Friend("xiaogang", 10));
	foo.addFriend(Friend("xiaohong", 10));
	foo.addFriend(Friend("xiaohong", 10));
	foo.addFriend(Friend("xiaohong", 10));
	foo.addFriend(Friend("xiaohong", 10));

	foo.showFriends();
	return 0;
}
