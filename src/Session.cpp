#include "../include/Session.h"
#include "../include/Watchable.h"
#include "../include/Action.h"
#include "../include/User.h"
#include "../include/json.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

Session::Session(const std::string &configFilePath):content(),actionsLog(),userMap(),activeUser(),tags(){
    ifstream i(configFilePath);
    nlohmann::json j;
    i >> j;
    std::string s=j.dump();
    if(userMap.size()==0){
        this->userMap.insert({"default",new LengthRecommenderUser("default",0)});}
        this->activeUser=userMap.at("default");
    parse (s);
    this->popularitySort(tags);
}
void Session:: start(){
    std::cout<<"SPLFlix is on now!"<<endl;
    string action="";
    while (action!="exit"){
        std::getline(std::cin, action);
        execute(action);
    }
}
void Session::popularitySort(std::vector<std::pair<std::string,int>>& ar) {
    for (int i=0;(unsigned)i<content.size();i++){//check every watchable
        vector<string>tags=this->content[i]->getvTags();
        for (int j=0;(unsigned)j<tags.size();j++){//check each tag for watchable
            bool contains=false;
            for (int k=0;(unsigned)k<ar.size();k++){//check overall tags vector for Session
                if (ar[k].first==tags[j]){contains=true;}
            }
            if (!contains){ar.push_back({tags[j],0});}//new genre
        }
        bool sorted=false;
        while (!sorted){
            sorted=true;
            for (int i=0;(unsigned)i<ar.size()-1;i++){
                if (ar[i].first>ar[i+1].first){
                    string temp=ar[i].first;
                    ar[i].first=ar[i+1].first;
                    ar[i+1].first=temp;
                    sorted=false;
                }
            }
        }
    }
}
std::vector<std::pair<std::string,int>> Session::getContentTags()const{return tags;}
void Session::parse (std::string con)
    {
        std::string movies=MoviesData(con);
        long id=1;
        bool empty=false;
        if(movies.size()==11&&movies.substr(0,1)=="{"&&movies.substr(9,2)==":["){
            empty=true;
        }
        while (!empty){
            std::string data=WatchableData(movies);
            Movie *mov=movie(data,id);
            content.push_back(mov);
            if (movies.size()==0){
                empty=true;}
        }
        empty=false;
        if (con.size()==14&&con.substr(0,9)=="tv_series"&&con.substr(11,3)=="[]}"){
            empty=true;
        }
        while (!empty)//Same on TV series
        {
            std::string single=WatchableData(con);
            int len=getLength(single);
            single=single.substr(9,single.size());
            std::string name=getName(single);
            single=single.substr(12,single.size());
            std::vector<int> ses=getSeasons(single);
            single=single.substr(11,single.size());
            std::vector<std::string>tags=getTags(single);
            for (int k=0;(unsigned)k<ses.size();k++){
                for (int j=0;j<ses[k];j++){
                    //=====Initializing Single Series ep by ep======
                    Episode *ep=new Episode (id,name,len,k+1,j+1,tags);
                    id++;
                    content.push_back(ep);
                }
            }
            if (con=="}"){empty=true;
            }
        }
}
Movie* Session::movie(std::string data,long& id) {//Change signature to &(?)
    int i=0;
    int len;
    string name;
    vector<string>tags;
    while ((unsigned)i<data.size()){
        if ((unsigned)(i+6)<data.size()&&data.substr(i,6)=="length"){//We'd reached length data
            len=getLength(data);
        }
        if ((unsigned)(i+4)<data.size()&&data.substr(i,4)=="name"){
            data=data.substr(7,data.length());//Cutting annoying Parts(name)
            name=getName(data);
        }
        if (data.substr(0,4)=="tags")
        {
            data=data.substr(8,data.length());//Cutting annoying Parts(tags)
            //cout<<"aftercut string:"<<data<<endl;
            tags=getTags(data);
        }
        data=data.substr(1,data.length());//Why was 4???
    }
    //std::cout<<"Length of movie is:"<<len<<endl;
    //std::cout<<"Name movie is:"<<name<<endl;
    //std::cout<<"Num of tags is:"<<tags.size()<<endl;
    //std::cout<<"Movie tags are:";
    int u=0;
    while ((unsigned)u<tags.size()){
        //cout<<tags[u]<<";";
        u++;}
    //cout<<endl;
    Movie *mov=new Movie (id,name,len,tags);
    id++;
    return mov;
}
int Session::getLength(std::string& data) {
    string len="";
    while (data.substr(0,1)!=","){
        //cout<<"Current sign:"<<data.substr(0,1)<<endl;
        if (data.substr(0,1)>="0"&&data.substr(0,1)<="9"){
            //cout<<"Connecting:"<<data.substr(0,1)<<endl;
            len=len+data.substr(0,1);
        }
        //cout<<"Cutting:"<<data.substr(0,1)<<endl;
        data=data.substr(1,data.length());
    }
   // cout<<"Rest of data(length function)"<<data<<endl;
    int l=std::stoi(len);
    return l;
}
string Session::getName(std::string& data) {
    string name;
    while (data.substr(0,1)!=","){
        //cout<<data.substr(0,1)<<+";";
        name=name+data.substr(0,1);
        data=data.substr(1,data.length());
    }
    //cout<<"Rest of data(name function):"<<data<<endl;
    return name.substr(0,name.length()-1);//Subtr'ing unnecessary "
}
std::vector<std::string> Session::getTags(std::string& data) {//Allocate on heap/stack?
    std::vector<std::string>tags;
    string acc="";
    while (data.substr(0,1)!="]"){
        if (data.substr(0,1)==","){
            tags.push_back(acc.substr(0,acc.size()));
            data=data.substr(1,data.length());//cutting the ','
            acc="";
        }
        //cout<<"Current sign:"<<data.substr(0,1)<<endl;
        if (data.substr(0,1)!=string(1,'"')){acc=acc+data.substr(0,1);}
        //else{cout<<"Trouble sign is:"<<data.substr(0,1)<<endl;}
        data=data.substr(1,data.length());
        //cout<<"Current(tags func.):"<<data<<endl;
    }
    tags.push_back(acc.substr(0,acc.size()));
    return tags;
}
std::string Session::WatchableData(std::string& data){
    string single="";
    //cout<<"Breakpoint #1:"<<data<<endl;
    while (data.substr(0,2)!="]}"){
        //cout<<"Breakpoint #last one"<<endl;
        //cout<<"Data size is"<<data.size()<<endl;
        single+=data.substr(0,1);
        data=data.substr(1,data.length());
    }
    single+="]}";
    if (data.size()<=3){data="";}//last watchable
    else{data=data.substr(3,data.length());//cutting the ']}'
     }
    return single;
}
std::string Session::MoviesData (std::string& data){
    int i=9;
    bool cut=false;
    while ((unsigned)i<data.size()-10&&!cut){
        if (data.substr(i,9)=="tv_series"){
            //cout<<"Index found!"<<endl;
            cut=true;}
        i++;
    }
    string output="nothing";
    if (cut) output=data.substr(0,i-4);
    data=data.substr(i-1,data.size());
    return output;
}
std::vector<int> Session::getSeasons(std::string& data){
    std::vector<int>ses;
    string cur="";
    //cout<<"Current Single:"<<data<<endl;
    while(data.substr(0,1)!="]"){
        if(data.substr(0,1)==","){
            //cout<<"cur status:"<<cur<<endl;
            ses.push_back(std::stoi(cur));
            cur="";
            //cout<<"Current season len is:"<<cur<<endl;
        }else{cur=cur+data.substr(0,1);}
        data=data.substr(1,data.size());
    }
    ses.push_back(std::stoi(cur));
    return ses;
}
void Session::execute (std::string command){
    if (command.size()>=10){//Create,change,delete
        std::cout<<"Current input:"<<command<<endl;
        if (command.substr(0,10)=="createuser"){
            std::cout<<"Creating user..."<<endl;
            std::cout<<"String len:"<<command.size()<<endl;
            std::string name="";
            std::string alg;
            command=command.substr(11,command.size()-11);
            std::cout<<"Current input:"<<command<<endl;
            int i=0;
            while (command.substr(i,1)!=" "){
                name=name+command.substr(i,1);
                i++;
            }
            i++;
            alg=command.substr(i,command.size()-i);
            std::cout<<"Current input:"<<command<<endl;
            std::cout<<"User name:"<<name<<endl;
            std::cout<<"size:"<<name.size()<<endl;
            std::cout<<"User algorithm:"<<alg<<endl;
            std::cout<<"size:"<<alg.size()<<endl;
            CreateUser *create=new CreateUser (name,alg);
            create->act(*this);
            actionsLog.push_back(create);//Add action to log
        }
        if (command.substr(0,10)=="changeuser"){
            string name=command.substr(11,command.size()-11);
            cout<<"Username to activate is:"<<name<<endl;
            ChangeActiveUser *change=new ChangeActiveUser(name);
            change->act(*this);
            actionsLog.push_back(change);//Add action to log
        }
        if (command.substr(0,10)=="deleteuser"){
            string name=command.substr(11,command.size()-11);
            cout<<"Username to delete is:"<<name<<endl;
            DeleteUser *delet=new DeleteUser (name);
            delet->act(*this);
            actionsLog.push_back(delet);//Add action to log
        }
        if (command.substr(0,7)=="dupuser"){
            string orig="";
            int i=8;
               while(command.substr(i,1)!=" "){
                   orig=orig+command.substr(i,1);
                   i++;

               }
               i++;
               string newuser="";
               while ((unsigned)i<command.size()){
                   newuser=newuser+command.substr(i,1);
                   i++;
               }
               DuplicateUser *dup=new DuplicateUser (orig,newuser);
               dup->act(*this);
            actionsLog.push_back(dup);//Add action to log
        }
    }
    else{
        if (command=="watchhist"){
            cout<<"Watch history for "<<activeUser->getName()<<endl;
            PrintWatchHistory *history=new PrintWatchHistory(activeUser->getName());
            history->act(*this);
            actionsLog.push_back(history);
        }
        if (command!="watchhist"&&command.size()>=5
            &&command.substr(0,5)=="watch"){
            long id=stol(command.substr(6,command.size()-6));
            Watch *watch=new Watch(id);
            watch->act(*this);
            actionsLog.push_back(watch);
            if (watch->getStatus()==COMPLETED){//Add loop where needed!
                //cout<<"Checking recommendation for"<<activeUser->get_history()
                //[activeUser->get_history().size()-1]->toString()<<endl;
               Watchable* reco=activeUser->get_history()
                       [activeUser->get_history().size()-1]->getNextWatchable(*this);
               //bool test=reco==nullptr;
               //cout<<"Movie next recommendation decided by user "<<to_string(test)<<endl;
               if (reco==nullptr){
                   //cout<<"Good watch-what is next?"<<endl;
                   reco=activeUser->getRecommendation(*this);
                   //cout<<reco->toString()<<endl;
               }
                string answer="y";
               if (reco== nullptr){answer="n";}
                while(answer=="y"){
                cout<<"We recommend watching: "<<reco->toString()<<"[y/n]?"<<endl;
                getline(cin,answer);
                if (answer=="y"){
                    Watch *cur=new Watch(reco->getId());
                    cur->act(*this);
                    actionsLog.push_back(cur);//According to forum
                    reco=reco->getNextWatchable(*this);//next watchable-may be an ep
                    if (reco==nullptr){reco=activeUser->getRecommendation(*this);}
                    if(reco==nullptr){answer="n";}//Failed to find proper reco}
                    }
            }
            }
            else {cout<<"Breakpoint-watch command"<<endl;}
        }
        if (command=="content"){
            {
                PrintContentList *print=new PrintContentList();
                print->act(*this);
                actionsLog.push_back(print);//Add action to log
            }
        }
        if (command=="log"){
            PrintActionsLog *log=new PrintActionsLog();
            log->act(*this);
            actionsLog.push_back(log);
        }
        if (command=="exit"){
            Exit *exit=new Exit();
            exit->act(*this);
            actionsLog.push_back(exit);
        }
    }
}
bool Session::findUser(std::string name) const {//True if found
    bool output=false;
    for (auto it=userMap.begin();it!=userMap.end();it++){
        if (it->first==name){output=true;}}
    return output;
    }//checked!
void Session::addUser(string name,string alg){
    //Initialize and add to map
    if (alg=="len"){LengthRecommenderUser *user=new LengthRecommenderUser(name,0);userMap.insert({name,user});}
    if (alg=="gen"){
        GenreRecommenderUser *user=new GenreRecommenderUser(name,this->tags);userMap.insert({name,user});}
    if (alg=="rer"){RerunRecommenderUser *user=new RerunRecommenderUser(name,0);userMap.insert({name,user});}
}//checked!
void Session::changeActiveUser(string name){this->activeUser=userMap.at(name);}//checked!
void Session::deleteUser(std::string name) {
    //step 1:delete from map
    User *delet=this->userMap.at(name);
    userMap.erase(name);
    //step 2:safe delete
    delete delet;
    delet=nullptr;
}//checked!
void Session::duplicateUser(std::string old, std::string newuser) {
    User *user=userMap.at(old)->clone();
    user->setName(newuser);
    userMap.insert ({newuser,user});
}//checked
void Session::printContent() {for(int i=0;(unsigned)i<content.size();i++){
    cout<<to_string(content[i]->getId())+". "<<content[i]->toString()
    +" "<<content[i]->getLength()<<" minutes "<<content[i]->getTags()<<endl;}}//checked!
void Session::printActionsLog(){
    for (int i=(int)(actionsLog.size()-1);i>=0;i--){
        string status;
        if (actionsLog[i]->getStatus()==1){
            status="COMPLETED";
        }
        else{if(actionsLog[i]->getStatus()==2){
                status="ERROR";
        }
        else{status="PENDING";}}
        cout<<actionsLog[i]->toString()<<" "<<actionsLog[i]->getErrormsg()<<endl;}
}//checked!
void Session::printWatchHistory(string name){
    cout<<"Watch history for user "<<name<<endl;
    userMap.at(name)->printWatchHistory();
}//checked!
Watchable* Session:: watchContent (int id){//id starts from zero or one?
    Watchable *output;
    if ((unsigned)id<getContent().size()){
        output=getContent()[id];
        activeUser->watch(output);
    }
    else{output=nullptr;}
    return output;
}//checked!
std::vector<Watchable*> Session::getContent(){return this->content;}
User* Session::getActiveUser(){return this->activeUser;}
Watchable* Session::lenfit(double avg) {
    Watchable *output;
    int best_id=-1;
    double best_len=2147483647;//max length of watchable
    double cur;
    for (int i=0;(unsigned)i<content.size();i++){//No content(?)
        //Note:working with id's in user's history,s.t we add +1 to i
        if (!activeUser->findInHistory(long(i+1))){//step I:check if content is new
            cur=abs(content[i]->getLength()*1.0-avg);
            if (cur<best_len){//better than last fit
                best_len=cur;best_id=i;//smaller index in content guaranteed
            }
        }
    }
    //fail case:no good recommendation
    if (best_id==-1){//
        output=nullptr;}
    else{output=content[best_id];}
    return output;
}//checked!
void Session::copy(const Session &as) {
    //Part I:Copying the content
    for(int i=0;(unsigned)i<as.content.size();i++){
        content.push_back(as.content[i]->clone());
    }
    //Part II:Copying the actions log
    for(int i=0;(unsigned)i<as.actionsLog.size();i++){
        actionsLog.push_back(as.actionsLog[i]->clone());
    }
    //Part III:Copying users map//Part IV:setting activeUser
    for (auto it = as.userMap.begin();it != as.userMap.end();it++){
        //Part I:creating user duplicate
        User *cur=it->second->clone();
        //Part II:duplicating history
        vector<Watchable*>hist=it->second->get_history();
        //Part III:adding to userMap
        userMap.insert({cur->getName(),cur});
        //Part IV:activeuser
        if (it->first==as.activeUser->getName()){
            this->activeUser=userMap.at(as.activeUser->getName());
        }
    }
}
void Session::clean(){
    //Part I:Clearing the content
    for(int i=0;(unsigned)i<content.size();i++){
        delete content[i];
        content[i]=nullptr;
    }
    content.clear();
    for(int i=0;(unsigned)i<actionsLog.size();i++){
        delete this->actionsLog[i];
        this->actionsLog[i]=nullptr;
    }
    actionsLog.clear();
    //Part III:Clearing users map//Part IV:clearing activeUser
    for (auto it = userMap.begin();it != userMap.end();it++){
        User *cur=it->second;
        delete cur;
        cur=nullptr;
    }
    userMap.clear();
    activeUser=nullptr;
}
void Session::steal(Session& rhs){
    //Setting up pointers
    this->activeUser=rhs.activeUser;
    //content
    for(int i=0;(unsigned)i<rhs.content.size();i++){
        content.push_back(rhs.content[i]);
        rhs.content[i]=nullptr;
    }
    rhs.content.clear();
    //actions log
    for(int i=0;(unsigned)i<rhs.actionsLog.size();i++){
        actionsLog.push_back(rhs.actionsLog[i]);
        rhs.actionsLog[i]=nullptr;
    }
    rhs.actionsLog.clear();
    //users map,clearing activeUser
    for (auto it = rhs.userMap.begin();it != rhs.userMap.end();it++){
        if (it->first==rhs.activeUser->getName()){rhs.activeUser=nullptr;}
        userMap.insert({it->first,it->second});
        it->second=nullptr;
    }
    rhs.userMap.clear();
}
//Rule of five:
Session::Session(const Session& as):content(),actionsLog(),userMap(),activeUser(),tags(){
    this->copy(as);
    }//Copy constructor
Session& Session::operator=(const Session &as) {
    if (this!=&as){this->clean();this->copy(as);}//self assignment
    return *this;
}//Copy assignment
Session::~Session(){this->clean();}//Destructor
Session::Session(Session&& rhs):content(),actionsLog(),userMap(),activeUser(),tags(){
    this->steal(rhs);}//Move constructor
Session& Session::operator=(Session&& rhs){
    if(&rhs!=this){clean();steal(rhs);}
    return *this;
}//Move assignment