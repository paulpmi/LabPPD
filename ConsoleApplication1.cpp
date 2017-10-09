// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include<iostream>
#include<mutex>
#include<thread>
#include<map>
#include<string>
#include <stdlib.h>
using namespace std;

class Account {
	int uniqueNr;
	int serialNumber;
	map<int, pair<string, int>> log;
	mutex mtx;
	int money;
	int checkMoney;

public: 
	Account(int uniqueNr, int money) {
		this->uniqueNr = uniqueNr;
		this->serialNumber = 0; 
		this->money = money; 
		this->checkMoney = money;
	}
public:
	void returnLog() {
		cout << money<< endl;
	}
public:
	void check() {

		this->mtx.lock();
		int copyMoney = this->checkMoney;
		map<int, string>::iterator it;
		for (auto &x : this->log) {
			if (x.second.first.find("Send"))
				this->checkMoney += x.second.second;
			else
				this->checkMoney -= x.second.second;
		}
		if (this->checkMoney != this->money)
			cout << "Error in Transactions";
		this->checkMoney = copyMoney;
		this->mtx.unlock();
	}

private:
	void receive(int money) {
		this->mtx.lock();
		this->money = this->money + money;
		string message = "Receive " + to_string(money);
		pair<string, int> a;
		a = pair<string, int>(message, money);
		this->log[this->serialNumber] = a;
		this->serialNumber++;
		this->mtx.unlock();
	}

public:
	void send(int money, Account* receiver)
	{
		if (this->money < money)
			cout << "Insufficient Founds";
		else {
			this->mtx.lock();
			this->money = this->money - money;
			receiver->receive(money);
			string message = "Send " + to_string(money);
			pair<string, int> a;
			a = pair<string, int>(message, money);
			this->log[this->serialNumber] = a;
			this->serialNumber++;
			this->mtx.unlock();
		}
	}
public:
	thread sendThread(int money, Account *receiver) {
		return thread([=] {send(money, receiver); });
	}

public:
	thread checkThread() {
		return thread([=] {check(); });
	}
};

int main()
{
	Account *a1 = new Account(1, 100);
	Account *a2 = new Account(2, 200);
	Account *a3 = new Account(3, 300);
	
	int iterations = 0;
	int v1, v2, v3;
	
	while (iterations < 100) {
		v1 = rand() % 100 + 1;
		v2 = rand() % 100 + 1;
		v3 = rand() % 100 + 1;
		thread t1 = a1->sendThread(v1, a2);
		thread t2 = a2->sendThread(v2, a3);
		thread t3 = a3->sendThread(v3, a1);
		if (iterations % 10 == 0) {
			thread ct1 = a1->checkThread();
			thread ct2 = a2->checkThread();
			thread ct3 = a3->checkThread();
			t1.join();
			t2.join();
			t3.join();
			ct1.join();
			ct2.join();
			ct3.join();
		}
		else {
			t1.join();
			t2.join();
			t3.join();
		}
		iterations++;
	}
	cout << endl;
	a1->returnLog();
	a2->returnLog();
	a3->returnLog();
    return 0;
}

