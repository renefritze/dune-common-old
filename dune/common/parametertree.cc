// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <fstream>
#include <set>
#include <algorithm>

#include <dune/common/exceptions.hh>
#include <dune/common/parametertree.hh>

using namespace Dune;
using namespace std;

namespace {
    struct ToLower {
        int operator()(int c)
        {
            return std::tolower(c);
        }
    };
}

ParameterTree::ParameterTree()
{
}

void ParameterTree::report() const
{
	report("");
}

void ParameterTree::report(const string prefix) const
{
    reportStream(std::cout, prefix);
}

void ParameterTree::reportStream(ostream& stream, const string& prefix) const
{
	typedef map<string, string>::const_iterator ValueIt;
	ValueIt vit = values.begin();
	ValueIt vend = values.end();
	
	for(; vit!=vend; ++vit)
        stream << vit->first << " = \"" << vit->second << "\"" << endl;
	
	typedef map<string, ParameterTree>::const_iterator SubIt;
	SubIt sit = subs.begin();
	SubIt send = subs.end();
	for(; sit!=send; ++sit)
	{
        stream << "[ " << prefix + sit->first << " ]" << endl;
		(sit->second).report(prefix + sit->first + ".");
	}
}

bool ParameterTree::hasKey(const string& key) const
{
	string::size_type dot = key.find(".");
	
	if (dot != string::npos)
	{
		string prefix = key.substr(0,dot);
		if (subs.count(prefix) == 0)
			return false;
		
		const ParameterTree& s = sub(prefix);
		return s.hasKey(key.substr(dot+1));
	}
	else
		return (values.count(key) != 0);
}

bool ParameterTree::hasSub(const string& key) const
{
	string::size_type dot = key.find(".");
	
	if (dot != string::npos)
	{
		string prefix = key.substr(0,dot);
		if (subs.count(prefix) == 0)
			return false;
		
		const ParameterTree& s = sub(prefix);
		return s.hasSub(key.substr(dot+1));
	}
	else
		return (subs.count(key) != 0);
}

ParameterTree& ParameterTree::sub(const string& key)
{
	string::size_type dot = key.find(".");
	
	if (dot != string::npos)
	{
		ParameterTree& s = sub(key.substr(0,dot));
		return s.sub(key.substr(dot+1));
	}
	else
		return subs[key];
}

const ParameterTree& ParameterTree::sub(const string& key) const
{
	string::size_type dot = key.find(".");
	
	if (dot != string::npos)
	{
                const ParameterTree& s = sub(key.substr(0,dot));
		return s.sub(key.substr(dot+1));
	}
	else
                return subs.find(key)->second;
}

string& ParameterTree::operator[] (const string& key)
{
	string::size_type dot = key.find(".");
	
	if (dot != string::npos)
	{
		if (not(hasSub(key.substr(0,dot))))
 			subKeys.push_back(key.substr(0,dot));
		ParameterTree& s = sub(key.substr(0,dot));
		return s[key.substr(dot+1)];
	}
	else
	{
		if (not(hasKey(key)))
 			valueKeys.push_back(key);
		return values[key];
	}
}

const string& ParameterTree::operator[] (const string& key) const
{
    string::size_type dot = key.find(".");
    
    if (dot != string::npos)
    {
        if (not(hasSub(key.substr(0,dot))))
            DUNE_THROW(Dune::RangeError, "Key '" << key << "' not found in ParameterTree");
        const ParameterTree& s = sub(key.substr(0,dot));
        return s[key.substr(dot+1)];
    }
    else
    {
        if (not(hasKey(key)))
            DUNE_THROW(Dune::RangeError, "Key '" << key << "' not found in ParameterTree");
        return values.find(key)->second;
    }
}

string ParameterTree::get(const string& key, const string& defaultValue) const
{
	if (hasKey(key))
		return (*this)[key];
	else
		return defaultValue;
}

string ParameterTree::get(const string& key, const char* defaultValue) const
{
	if (hasKey(key))
		return (*this)[key];
	else
		return defaultValue;
}


int ParameterTree::get(const string& key, int defaultValue) const
{
	stringstream stream;
	stream << defaultValue;
	string ret = get(key, stream.str());
	
	return atoi(ret.c_str());
}

double ParameterTree::get(const string& key, double defaultValue) const
{
    if(hasKey(key))
        return atof((*this)[key].c_str());
    else
        return defaultValue;
}

bool ParameterTree::get(const string& key, bool defaultValue) const
{
	stringstream stream;
	if (defaultValue)
		stream << 1;
	else
		stream << 0;
	
	string ret = get(key, stream.str());
    std::transform(ret.begin(), ret.end(), ret.begin(), ToLower());

    if (ret == "yes" || ret == "true")
        return true;
	
    if (ret == "no" || ret == "false")
        return false;
	
	return (atoi(ret.c_str()) !=0 );
}

// This namespace here is needed to make the code compile...
namespace Dune {

template<>
string ParameterTree::get<string>(const string& key) const
{
	if (hasKey(key))
		return (*this)[key];

        DUNE_THROW(RangeError, "Key '" << key << "' not found in parameter file!");
}

template<>
int ParameterTree::get<int>(const string& key) const
{
        if (hasKey(key))
            return std::atoi((*this)[key].c_str());

        DUNE_THROW(RangeError, "Key '" << key << "' not found in parameter file!");
}

template<>
double ParameterTree::get<double>(const string& key) const
{
    if (hasKey(key))
        return std::atof((*this)[key].c_str());

    DUNE_THROW(RangeError, "Key '" << key << "' not found in parameter file!");
}

template<>
bool ParameterTree::get<bool>(const string& key) const
{
    if (hasKey(key))
    {

        string ret = (*this)[key];
        std::transform(ret.begin(), ret.end(), ret.begin(), ToLower());
        
        if (ret == "yes" || ret == "true")
            return true;
	
        if (ret == "no" || ret == "false")
            return false;
	
        return (std::atoi(ret.c_str()) !=0 );
    }
    DUNE_THROW(RangeError, "Key '" << key << "' not found in parameter file!");
}


}  // end namespace Dune

string ParameterTree::ltrim(const string& s)
{
    std::size_t firstNonWS = s.find_first_not_of(" \t\n\r");

    if (firstNonWS!=string::npos)
        return s.substr(firstNonWS);
    return string();
}

string ParameterTree::rtrim(const string& s)
{
    std::size_t lastNonWS = s.find_last_not_of(" \t\n\r");

    if (lastNonWS!=string::npos)
        return s.substr(0, lastNonWS+1);
    return string();
}

const ParameterTree::KeyVector& ParameterTree::getValueKeys() const
{
	return valueKeys;
}

const ParameterTree::KeyVector& ParameterTree::getSubKeys() const
{
	return subKeys;
}
