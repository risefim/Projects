#include <vector>
#include <string>
#include "../include/Watchable.h"
#include "../include/Session.h"

using namespace std;
//Basic methods:
Watchable::Watchable //Constructor
    (long id, int length, const
    std::vector<std::string> &tags) :id(id),length(length),tags(tags){}
//Rule of three:
Watchable::~Watchable(){tags.clear();}//
//Watchable::Watchable (const Watchable &watch)//Copy constructor
    //:id(watch.id),length(watch.length),tags(watch.tags){}
    std::string Watchable::getTags()const {
    string output="[";
    for (int i=0;(unsigned)i<tags.size();i++){output+=tags[i]+", ";}
    output=output.substr(0,output.size()-2)+"]";
    return output;
}
    long Watchable::getId()const{return this->id;}
    int Watchable::getLength() const {return this->length;}
    vector<string> Watchable::getvTags(){return this->tags;}
//=====================================================================
Movie::Movie
    (long id, const std::string &name,
            int length, const std::vector<std::string> &tags)
                :Watchable(id,length,tags),name(name) {}
std::string Movie::toString() const {
    string output;
    output =name;
    return output;}
Watchable* Movie:: getNextWatchable (Session& s)const {//User's alg decide
    return nullptr;}
std::string Movie::getName() {return this->name;}
Movie* Movie:: clone(){return new Movie(this->getId(),this->getName(),this->getLength(),this->getvTags());}
//Movie::~Movie(){}
//=====================================================================
Episode::Episode(long id, const std::string &seriesName,
            int length, int season, int episode,
                 const std::vector<std::string> &tags):Watchable(id,length,tags),seriesName(seriesName),season(season),episode(episode),nextEpisodeId() {}
std::string Episode::toString() const {
    string output;
    output =output+seriesName+"S"+to_string(season)+"E"+to_string(episode);
    return output;}
std::string Episode::getName(){ return this->seriesName;}
Watchable* Episode::getNextWatchable(Session& sess)const{
    Watchable *output;
    //assumption that all id's starts from 1,not zero
    //Success if episode was not last of TV Series
    //Mark number 1-change history to obj's not pointers
    if ((unsigned)(this->getId()-1)<sess.getContent().size()-1//Episode is not last
    &&sess.getContent()[this->getId()-1]->getName()==//up next ep. is from same series
    sess.getContent()[this->getId()]->getName()){
        output=sess.getContent()[this->getId()];//Assigning next Episode(check correctness)
    }
    else{
        output=nullptr;
    }
return output;
}
Episode* Episode:: clone(){return new Episode(this->getId(),this->getName(),this->getLength(),this->season,this->episode,this->getvTags());}
//Episode::~Episode(){}
