#include "../include/Session.h"
#include "../include/Watchable.h"
#include "../include/Action.h"
#include "../include/User.h"

using namespace std;
 User::User(const std::string& name):history(),name(name){}
std::string User:: getName() const{return this->name;}
std::vector<Watchable*> User:: get_history() const{return this->history; }
void User::setName(std::string name) {this->name=name;}//checked!
void User::printWatchHistory(){
    for (int i=0;(unsigned)i<history.size();i++){
        cout<<to_string(i+1)<<". "
        <<history[i]->toString()<<endl;
    }
}//checked!
void User::addToHistory(Watchable* content){
    //History of every user has it's own memory on heap
     //cout<<"adding to history:"<<content->getName()<<endl;
     this->history.push_back(content->clone());
 }//checked!
bool User:: findInHistory(long id){
    //cout<<"Checking history..."<<endl;
     bool found=false;
     int i=0;
     while ((!found)&&(unsigned)i<history.size()){
         if(history[i]->getId()==id)
             found=true;
         i++;
     }return found;
 }
 void User::copy(const User& auser) {
     this->name=auser.name;
     for (int i=0;(unsigned)i<auser.history.size();i++){
         history.push_back(auser.history[i]->clone());
     }
 }
 void User::clean() {
     for (int i=0;(unsigned)i<history.size();i++){
        delete history[i];
         history[i]=nullptr;
     }
     history.clear();
 }
 void User::steal(User& rhs){
    this->name=rhs.name;
    for(int i=0;(unsigned)i<rhs.history.size();i++){
        history.push_back(rhs.history[i]);
        rhs.history[i]=nullptr;
    }
    rhs.clean();
 }
 void User::backupHistory (User& old){//Backing up watchables
     for (int i=0;(unsigned)i<old.history.size();i++){
         //cout<<"Backing up history for user"<<this->getName()<<endl;
         history.push_back(old.history[i]->clone());
     }
 }
//Rule of five:
    User:: User(const User& aUser):history(),name(){
     this->copy(aUser);
}
    User::~User() {this->clean();
}
    User& User:: operator=(const User &aUser){//Necessary?
       if (this!=&aUser){this->clean();this->copy(aUser);}
       return *this;
}
    User:: User(User&& rhs):history(),name()
    {this->steal(rhs);}
    User& User::operator=(User&& rhs){if(&rhs!=this){clean();steal(rhs);}
        return *this;}
//=======================================================
LengthRecommenderUser::LengthRecommenderUser
        (const std::string& name):User(name),counter(0),avg(0){}
LengthRecommenderUser::LengthRecommenderUser
    (const std::string& name,int counter):User(name),counter(counter),avg(0)
    {}
Watchable*LengthRecommenderUser:: getRecommendation(Session& s){
     //cout<<"Asking algorithm..."<<endl;
    //act by algorithm:
    Watchable *next=s.lenfit(avg);
     return next;}
LengthRecommenderUser* LengthRecommenderUser::clone(){
     LengthRecommenderUser* output=new LengthRecommenderUser (this->getName(),this->counter);
     output->backupHistory(*this);
    return output;
}
void LengthRecommenderUser::watch(Watchable *content) {
     //User watches content,it's added to history,and we calculating new avg time
     this->addToHistory(content);
     counter+=content->getLength();
     avg= static_cast<float>(counter / (history.size() * 1.0));
    //cout<<"Updating user info..."<<"new avg is"<<avg<<endl;
 }
LengthRecommenderUser::~LengthRecommenderUser(){}
//=======================================================
RerunRecommenderUser::RerunRecommenderUser
        (const std::string& name):User(name),counter(0){}
RerunRecommenderUser::RerunRecommenderUser
    (const std::string& name,int counter):User(name),counter(counter){}
void RerunRecommenderUser::watch(Watchable *content) {
     this->addToHistory(content);
 }
Watchable*RerunRecommenderUser:: getRecommendation(Session& s){
     Watchable* output;
     if (s.getActiveUser()->get_history().size()==0){output=nullptr;}
     else{
         //if we reached here-last watchable was a movie-rewatch it
         output=s.getActiveUser()->get_history()[counter%s.getActiveUser()->get_history().size()];
     }
     //if reccomendation returns nullprt-do we have to count it?
     counter++;
     return output;}
RerunRecommenderUser* RerunRecommenderUser::clone(){
    RerunRecommenderUser* output=new RerunRecommenderUser (this->getName(),this->counter);
    output->backupHistory(*this);
    return output;
}
RerunRecommenderUser::~RerunRecommenderUser(){}
//=======================================================
GenreRecommenderUser::GenreRecommenderUser(const std::string& name):User(name),popularity(){}
GenreRecommenderUser::GenreRecommenderUser
        (const std::string& name,std::vector<std::pair<std::string,int>> popularity):User(name),popularity(popularity){}
Watchable*GenreRecommenderUser:: getRecommendation(Session& s){
    //cout<<"Checking recommendation(GenreUser)..."<<endl;
     Watchable *output;
     bool found=false;
     int id;
     for (int i=0;((unsigned)i<popularity.size())&(!found);i++){//checking whole popular user tags until found
         string cur=popularity[i].first;
         for(int j=0;((unsigned)j<s.getContent().size())&(!found);j++){//checking whole content until find
             if (!this->findInHistory(s.getContent()[j]->getId())){//if watchable is not in history-continue check
                 vector<std::string>tags=s.getContent()[j]->getvTags();//check tags
                 for (int k=0;((unsigned)k<tags.size())&(!found);k++){
                     if (tags[k]==cur){//found fit tag and not watched yet
                         id=j;//id of a good content to recommend
                         found=true;
                     }
                 }
             }
         }
     }
     if (found){output=s.getContent()[id];}else{output=nullptr;}
     return output;}
GenreRecommenderUser* GenreRecommenderUser::clone(){
    GenreRecommenderUser* output=new GenreRecommenderUser (this->getName(),this->popularity);
    output->backupHistory(*this);
    return output;
}
void GenreRecommenderUser::watch(Watchable *content) {
    //cout<<"Watching(GenreUser)"<<content->getName()<<endl;
    this->addToHistory(content);
     //We should update views per every tag,if tag does not exist-add to vector
     vector<string>tags=content->getvTags();
     //redefining the popularity of genres(what if empty?)
     for (int i=0;(unsigned)i<tags.size();i++){
        int cur=this->contatinsGenre(tags[i]);
        if (cur!=-1){this->popularity[cur].second++;}
        else{pair<string,int>add;
        add.first=tags[i];
        add.second=1;//user watches the content-start with 1 veiew
        popularity.push_back(add);}
     }
     //sorting the popularity
     this->sort();
 }
 int GenreRecommenderUser::contatinsGenre(std::string genre)const {
     bool contains=false;
     int j=-1;
     for (int i=0;((unsigned)i<popularity.size())&(!contains);i++){
         if (popularity[i].first==genre){contains=true;j=i;}
     }
     return j;
 }
 void GenreRecommenderUser::sort(){
     //cout<<"Updating popularity criterions of user"<<endl;
     //Part I:Sort by popularity:decreasing order
     bool sorted=false;
     while (!sorted){
         sorted=true;
         for (int i=0;(unsigned)i<popularity.size()-1;i++){
             if (popularity[i].second<popularity[i+1].second){
                 //change adjascent pairs if necessary and update that array is not sorted!
                 int temp=popularity[i].second;
                 string name=popularity[i].first;
                 popularity[i].second=popularity[i+1].second;
                 popularity[i+1].second=temp;
                 popularity[i].first=popularity[i+1].first;
                 popularity[i+1].first=name;
                 sorted=false;
             }
         }
     }
     //Part II:Sort in lexicographic order
     sorted=false;
     while (!sorted){
         sorted=true;
         for (int i=0;(unsigned)i<popularity.size()-1;i++){
             if (popularity[i].second==popularity[i+1].second
                &&popularity[i].first>popularity[i+1].first){
                 //change adjascent pairs if necessary and update that array is not sorted!
                 string name=popularity[i].first;
                 popularity[i].first=popularity[i+1].first;
                 popularity[i+1].first=name;
                 sorted=false;
             }
         }
 }
 }
GenreRecommenderUser::~GenreRecommenderUser(){}
