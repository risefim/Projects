#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "Action.h"
#include "Watchable.h"

class User;
class Watchable;

class Session{
public:
    Session(const std::string &configFilePath);
    void start();
    void parse(std::string con);
    Movie* movie (std::string data,long& id);
    void episode (std::string data);
    std::string getName(std::string& data);
    int getLength (std::string& data);
    std::vector<std::string> getTags(std::string& data);
    std::string WatchableData (std::string&data);
    std::string MoviesData(std::string& data);
    std::vector<int>getSeasons(std::string& data);
    void execute (std::string command);
    bool findUser(std::string name)const;
    void addUser(std::string name,std::string alg);
    void changeActiveUser(std::string name);
    void deleteUser(std::string name);
    void duplicateUser(std::string old,std::string newuser);
    void printContent();
    void printActionsLog();
    void printWatchHistory(std::string name);
    Watchable* watchContent (int id);
    std::vector<Watchable*>getContent();
    User* getActiveUser();
    Watchable* lenfit(double avg);
    void copy (const Session& other);
    void clean();
    void steal(Session& rhs);
    void popularitySort (std::vector<std::pair<std::string,int>>& ar);
    std::vector<std::pair<std::string,int>> getContentTags()const;
    //Rule of five:
    Session (const Session &as);
    Session& operator=(const Session &as);
    ~Session();
    Session (Session&& rhs);
    Session& operator=(Session&& rhs);
private:
    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<std::string,User*> userMap;
    User* activeUser;
    std::vector<std::pair<std::string,int>> tags;
};
#endif
