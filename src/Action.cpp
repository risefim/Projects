#include "../include/Session.h"
#include "../include/Watchable.h"
#include "../include/Action.h"
#include "../include/User.h"
#include <fstream>
#include <iostream>
using namespace std;

BaseAction:: BaseAction():errorMsg(),status(PENDING){}
ActionStatus BaseAction::getStatus() const{return this->status;}
std::string BaseAction::getErrormsg()const{return errorMsg;}
void BaseAction::complete() {this->status=COMPLETED;}
void BaseAction::error(const std::string& errorMsg){
    this->status=ERROR;this->errorMsg=errorMsg;cout<<errorMsg<<endl;}
std::string BaseAction::getErrorMsg() const{return errorMsg;}
std::string BaseAction::strStatus()const{
    std::string status;
    if (this->getStatus()==1){
        status="COMPLETED";
    }
    else{if(this->getStatus()==2){
            status="ERROR";
        }
        else{status="PENDING";}
}
    return status;
}
BaseAction::~BaseAction() {}
//=======================================================
CreateUser::CreateUser(std::string name,std::string type):BaseAction(),username(name),alg(type){
    //this->username=name;
    //this->alg=type;
}
void CreateUser::act(Session &sess) {
    //fail cases:
    if (sess.findUser(this->username)){
        this->error("Error:the new user name is already taken");
        //std::cout<<"Error!the new user name is already taken"<<endl;
    }
    else {
        if (alg!="len"&&alg!="gen"&&alg!="rer"){
            this->error("Error:wrong algorithm type");
        }else{//Success:Complete action
            sess.addUser(username,alg);
            this->complete();
        }
    }
}
CreateUser* CreateUser:: clone(){return new CreateUser(this->username,this->alg);}
std::string CreateUser::toString()const{
    return "CreateUser "+string(strStatus());}
//=======================================================
ChangeActiveUser::ChangeActiveUser(std::string name):BaseAction(),name(name){
    //this->name=name;
}
void ChangeActiveUser::act(Session &sess) {
    if (!sess.findUser(this->name)){//fail case
        this->error("Error changing:user not found");
        //std::cout<<"Wrong username"<<endl;
    }
    else {
        cout<<"Changing active user to:"<<name<<endl;
        sess.changeActiveUser(name);
        this->complete();
    }
}
std::string ChangeActiveUser::toString() const{
    return "ChangeActiveUser "+strStatus();
}
ChangeActiveUser* ChangeActiveUser:: clone(){return new ChangeActiveUser(this->name);}
//=======================================================
DeleteUser::DeleteUser(string name):BaseAction(),name(name){}
void DeleteUser::act(Session &sess) {
    if (!sess.findUser(name))//fail case:user not exist
    {
        this->error("Error deleting:user not found!");
    }
    else{
        sess.deleteUser(name);
        this->complete();
    }
}
std::string DeleteUser::toString() const{return "DeleteUser "+strStatus();}
DeleteUser* DeleteUser::clone(){return new DeleteUser(this->name);}
//=======================================================
DuplicateUser::DuplicateUser(string old,string newuser):BaseAction(),old(old),newuser(newuser){
    //this->old=old;
    //this->newuser=newuser;
}
void DuplicateUser::act(Session &sess) {
    //fail cases:old not found or new exists
    if (!sess.findUser(old)){
        this->error("Duplicate error:user to duplicate not foumd!");
    }
    else{
        if (sess.findUser(newuser)){
            this->error("Duplicate error:new user already exists!");
        }
        else{
            sess.duplicateUser(old,newuser);
            this->complete();
        }
    }
}
std::string DuplicateUser::toString() const{return "DuplicateUser "+strStatus();}
DuplicateUser* DuplicateUser::clone(){return new DuplicateUser(this->old,this->newuser);}//What???
//=======================================================
PrintContentList::PrintContentList():BaseAction(){}
void PrintContentList::act(Session &sess) {
    sess.printContent();
    this->complete();}
std::string PrintContentList::toString() const{return "PrintContentList "+strStatus();}
PrintContentList* PrintContentList::clone(){return new PrintContentList(*this);}
//=======================================================
PrintWatchHistory::PrintWatchHistory(string name):BaseAction(),name(name){
    //this->name=name;
     }
void PrintWatchHistory::act(Session &sess) {
    if (!sess.findUser(name)){//Unreachable statement
        //cout<<"(Actiion.cpp)Searching in userMap:"<<this->name<<endl;
        //cout<<"Error(watchist)"<<endl;
        this->error("Watchlist error:user not found!");
    }
    else{
        //cout<<"PrintWatchHIstory(Action.cpp)"<<endl;
        sess.printWatchHistory(name);
        this->complete();
    }
}
std::string PrintWatchHistory::toString() const{return "PrintWatchHistory "+strStatus();}
PrintWatchHistory* PrintWatchHistory::clone(){return new PrintWatchHistory(this->name);}
//=======================================================
Watch::Watch(int id):BaseAction(),id(id){
    this->id=id-1;
}
void Watch::act(Session &sess) {
    Watchable *watch=sess.watchContent(id);
    if (watch==nullptr){this->error("Watch error:invalid id!");}
    else{
        cout<<"Watching "<<watch->toString()<<endl;
        this->complete();
    }
}
std::string Watch::toString() const{return "Watch "+strStatus();}
Watch* Watch::clone(){return new Watch(this->id);}
//=======================================================
PrintActionsLog::PrintActionsLog():BaseAction(){}
void PrintActionsLog::act(Session &sess) {
    sess.printActionsLog();
    this->complete();
}
std::string PrintActionsLog::toString() const{return "PrintActionsLog "+strStatus();}
PrintActionsLog* PrintActionsLog::clone(){return new PrintActionsLog(*this);}
//=======================================================
Exit::Exit():BaseAction(){}
void Exit::act(Session &sess) {this->complete();}
std::string Exit::toString() const{return "Exit "+strStatus();}
Exit* Exit::clone(){return new Exit(*this);}