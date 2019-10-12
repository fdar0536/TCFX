#ifndef ASSPARSER_HPP
#define ASSPARSER_HPP

#ifdef _MSC_VER
#pragma once
#endif    /* _MSC_VER */

#include <string>
#include <vector>
#include <map>

#include "ass.hpp"
#include "asscommon.hpp"
#include "utf8.hpp"
#include "../common/basecommon.hpp"

using namespace std;

class SYMBOL_SHOW AssParser : protected CoreAss, protected CoreUtf8
{
public:
    
    AssParser(const string &fileName);
    
    shared_ptr<AssMeta> meta() const;
    
    map<string, shared_ptr<AssStyle>> styles() const;
    
    vector<shared_ptr<AssDialog>> dialogs() const;
    
    void upgradeDialogs();
    
    bool dialogIsUpgraded() const;
    
    bool isSylAvailable() const;
    
    bool isWordAvailable() const;
    
    bool isCharAvailable() const;
    
private:
    
    // https://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
    istream &safeGetline(istream &is, string &t);
    
    typedef enum _PARSER_SECTION
    {
        Idle,
        Script_Info,
        V4_Styles,
        Events
    } PARSER_SECTION;
    
    PARSER_SECTION section;
    
    void parseLine(string &);
    
    shared_ptr<AssMeta> metaData;
    
    map<string, shared_ptr<AssStyle>> styleData;
    
    bool dialogParsed;
    
    vector<shared_ptr<AssDialog>> dialogData;
    
    bool sylReady;
    
    bool wordReady;
    
    bool charReady;
    
    void parseDialogs();
    
    typedef struct _TEXT_SIZE
    {
        double width;
        double height;
        double ascent;
        double descent;
        double internal_leading;
        double external_leading;
    } TEXT_SIZE;
    
    TEXT_SIZE *textSize(string &text, shared_ptr<AssStyle> &style);
};

#endif // ASSPARSER_HPP
