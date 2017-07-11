#ifndef EDICT_HEADER
#define EDICT_HEADER

#include <string>
#include <map>

class edict : public std::map<std::string, std::string>
{
    //Encodeable dictionary
    typedef std::map<std::string, std::string> base_class;
public:
    //using base_class::base_class;
    edict();
    edict ( const std::string& str );
    std::string tostring() const;
};

#endif
