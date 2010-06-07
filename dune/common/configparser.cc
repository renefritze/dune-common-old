// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
#include "configparser.hh"

#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <fstream>
#include <set>

#include <dune/common/exceptions.hh>

using namespace Dune;
using namespace std;

ConfigParser::ConfigParser()
{
}

void ConfigParser::parseFile(std::string file, bool overwrite)
{
    ifstream in(file.c_str());

    if (!in)
        DUNE_THROW(IOError, "Could not open configuration file " << file);

    parseStream(in, "file '" + file + "'", overwrite);
}


void ConfigParser::parseStream(std::istream& in,
                               bool overwrite)
{
    parseStream(in, "stream", overwrite);
}
void ConfigParser::parseStream(std::istream& in,
                               const std::string srcname,
                               bool overwrite)
{
	string prefix;
	set<string> keysInFile;
	while(!in.eof())
	{
		string line;
		getline(in, line);
		line = ltrim(line);
		switch (line[0]) {
			case '#':
				break;
			case '[':
                line = rtrim(line);
				if (line[line.length()-1] == ']')
				{
					prefix = rtrim(ltrim(line.substr(1, line.length()-2)));
					if (prefix != "")
						prefix += ".";
				}
				break;
			default:
				string::size_type mid = line.find("=");
				if (mid != string::npos)
				{
					string key = prefix+rtrim(ltrim(line.substr(0, mid)));
					string value = ltrim(line.substr(mid+1));

					if (value.length()>0)
					{
                        // handle quoted strings
                        if ((value[0]=='\'') or (value[0]=='"'))
                        {
                            char quote = value[0];
                            value=value.substr(1);
                            while (*(rtrim(value).rbegin())!=quote)
                            {
                                if (not in.eof())
                                {
                                    string l;
                                    getline(in, l);
                                    value = value+"\n"+l;
                                }
                                else
                                    value = value+quote;
                            }
                            value = rtrim(value);
                            value = value.substr(0,value.length()-1);
                        }
                        else
                            value = rtrim(value);
					}
					
					if (keysInFile.count(key) != 0)
                        DUNE_THROW(Exception, "Key '" << key <<
                                   "' appears twice in " << srcname << " !");
					else
					{
                        if(overwrite || ! hasKey(key))
                            (*this)[key] = value;
						keysInFile.insert(key);
					}
				}
				break;
		}
	}
	
	return;
}


void ConfigParser::parseCmd(int argc, char* argv [])
{
	string v = "";
	string k = "";
	
	for(int i=1; i<argc; i++)
	{
		string s(argv[i]);
		
		if ((argv[i][0]=='-') && (argv[i][1]!='\000'))
		{
			k = argv[i]+1;
			continue;
		}
		else
			(*this)[k] = argv[i];
	}

	return;
}

ConfigParser& ConfigParser::sub(const string& key)
{
	string::size_type dot = key.find(".");
	
	if (dot != string::npos)
	{
		ConfigParser& s = sub(key.substr(0,dot));
		return s.sub(key.substr(dot+1));
	}
	else
        // Cast is safe, because ConfigParser has the same memory layout as ParameterTree,
        // and this method is temporary anyways.
		return static_cast<ConfigParser&>(subs[key]);
}

const ConfigParser& ConfigParser::sub(const string& key) const
{
	string::size_type dot = key.find(".");
	
	if (dot != string::npos)
	{
        const ConfigParser& s = sub(key.substr(0,dot));
		return s.sub(key.substr(dot+1));
	}
	else
        // Cast is safe, because ConfigParser has the same memory layout as ParameterTree,
        // and this method is temporary anyways.
        return static_cast<const ConfigParser&>(subs.find(key)->second);
}
