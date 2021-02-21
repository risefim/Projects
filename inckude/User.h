#ifndef USER_H_
#define USER_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
class Watchable;
class Session;

class User{
public:
    User(const std::string& name);
    virtual Watchable* getRecommendation(Session& s) = 0;
    std::string getName() const;
    std::vector<Watchable*> get_history() const;
    void setName (std::string name);
    virtual User* clone()=0;
    void printWatchHistory();
    void addToHistory(Watchable* content);
    virtual void watch (Watchable* content)=0;
    bool findInHistory(long id);
    void copy(const User& auser);
    void clean();
    void steal(User& rhs);
    void backupHistory (User& old);
    User (const User& auser);//Copy Constructor
    virtual ~User();//Destructor
    User& operator=(const User &aUser);//Copy assignment
    User (User&& rhs);
    User& operator=(User&& rhs);
protected:
    std::vector<Watchable*> history;
private:
    std::string name;

};

class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string& name);
    LengthRecommenderUser(const std::string& name,int counter);
    Watchable* getRecommendation(Session& s);
    LengthRecommenderUser* clone();
    void watch(Watchable *content);
    ~LengthRecommenderUser();
private:
    int counter;
    float avg;

};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const std::string& name);
    RerunRecommenderUser(const std::string& name,int counter);
    virtual Watchable* getRecommendation(Session& s);
    RerunRecommenderUser* clone();
    void watch(Watchable *content);
    ~RerunRecommenderUser();
private:
    int counter;
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const std::string& name);
    GenreRecommenderUser(const std::string& name,std::vector<std::pair<std::string,int>> popularity);
    virtual Watchable* getRecommendation(Session& s);
    GenreRecommenderUser* clone();
    void watch(Watchable *content);
    int contatinsGenre(std::string genre)const;
    void sort();
    ~GenreRecommenderUser();
private:
    std::vector<std::pair<std::string,int>> popularity;
};

#endif
