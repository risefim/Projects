#ifndef WATCHABLE_H_
#define WATCHABLE_H_

#include <string>
#include <vector>


class Session;

class Watchable{
public:
    Watchable(long id, int length, const std::vector<std::string>& tags);
    virtual std::string toString() const = 0;
    virtual Watchable* getNextWatchable(Session&) const = 0;
    std::string getTags ()const;//why string and not vector
    std::vector<std::string>getvTags();
    long getId()const;
    int getLength()const;
    virtual std::string getName()=0;
    virtual Watchable* clone()=0;
    //Rule of three:
    virtual ~Watchable();
    //Watchable (const Watchable &watch);
private:
    const long id;
    int length;
    std::vector<std::string> tags;
};

class Movie : public Watchable{
public:
    Movie(long id, const std::string& name,
            int length, const std::vector<std::string>& tags);
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    std::string getName();
    Movie* clone();
    //Rule of three:no need
    //virtual ~Movie();
private:
    std::string name;
};


class Episode: public Watchable{
public:
    Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags);
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    //Episode* getNextEpisode(Session& ); not appear on .h file on site course
    std::string getName();
    Episode* clone();
    //Rule of three:
    //virtual ~Episode();
private:
    std::string seriesName;
    int season;
    int episode;
    long nextEpisodeId;
};

#endif
