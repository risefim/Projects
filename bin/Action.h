#ifndef ACTION_H_
#define ACTION_H_

#include <string>
#include <iostream>

class Session;

enum ActionStatus{
	PENDING, COMPLETED, ERROR
};


class BaseAction{
public:
	BaseAction();
	ActionStatus getStatus() const;
	std::string getErrormsg()const;
	virtual void act(Session& sess)=0;
	virtual std::string toString() const=0;
	virtual BaseAction* clone()=0;
    virtual ~BaseAction();
    virtual std::string strStatus()const;
protected:
	void complete();
	void error(const std::string& errorMsg);
	std::string getErrorMsg() const;
private:
	std::string errorMsg;
	ActionStatus status;
};

class CreateUser  : public BaseAction {
public:
	CreateUser(std::string name,std::string type);
	virtual void act(Session& sess);
    CreateUser* clone();
    virtual std::string toString()const;
private:
	std::string username;
	std::string alg;
};

class ChangeActiveUser : public BaseAction {
public:
	ChangeActiveUser(std::string name);
	virtual void act(Session& sess);
	virtual std::string toString() const;
    ChangeActiveUser* clone();
private:
	std::string name;
};

class DeleteUser : public BaseAction {
public:
	DeleteUser(std::string name);
	virtual void act(Session & sess);
	virtual std::string toString() const;
    DeleteUser* clone();
private:
    std::string name;
};
class DuplicateUser : public BaseAction {
public:
	DuplicateUser(std::string old,std::string newuser);
	virtual void act(Session & sess);
	virtual std::string toString() const;
    DuplicateUser* clone();
private:
    std::string old;
    std::string newuser;
};

class PrintContentList : public BaseAction {
public:
	PrintContentList();
	virtual void act (Session& sess);
	virtual std::string toString() const;
    PrintContentList* clone();
};

class PrintWatchHistory : public BaseAction {
public:
	PrintWatchHistory(std::string name);
	virtual void act (Session& sess);
	virtual std::string toString() const;
    PrintWatchHistory* clone();
private:
    std::string name;
};


class Watch : public BaseAction {
public:
	Watch(int id);
	virtual void act(Session& sess);
	virtual std::string toString() const;
    Watch* clone();
private:
    long id;
};


class PrintActionsLog : public BaseAction {
public:
	PrintActionsLog();
	virtual void act(Session& sess);
	virtual std::string toString() const;
    PrintActionsLog* clone();
};

class Exit : public BaseAction {
public:
	Exit();
	virtual void act(Session& sess);
	virtual std::string toString() const;
    Exit* clone();
};
#endif
