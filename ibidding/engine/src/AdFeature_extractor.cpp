#include "AdFeature_extractor.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <sstream>

#include "AdGlobal.h"

using namespace gfs;

FeatureExtractor::FeatureExtractor(bool istrain)
{
	this->istrain = istrain;
	return;
}
	
FeatureExtractor::~FeatureExtractor()
{
	if(istrain)
		_save_fdict_to_file(_fdict_path);
	return;
}

bool FeatureExtractor::init(const std::string dict_path)
{
	_fdict_path = dict_path;
	return _load_fdict_from_file(dict_path);
}

bool FeatureExtractor::_load_fdict_from_file(std::string dict_path)
{
	std::ifstream fp(dict_path.c_str());

	_fdict.clear();

	for (std::string line; std::getline(fp, line, '\n');) {
		std::vector<std::string> fields(_split_by_sep(line, '\t'));

		if (2 != fields.size()) {
			_fdict.clear();
			return false;
		}

		_fdict[fields[0]] = std::strtoul(fields[1].c_str(), NULL, 10);
	}
//    AD_PRINT("the feature dictionary path: %s", dict_path.c_str());
//    AD_PRINT("load %d columns into the feature dictionary.", (unsigned long int)_fdict.size());
	std::cerr<<"the feature dictionary path: "<<dict_path<<std::endl;
	std::cerr<<"load "<<_fdict.size()<<" columns into the feature dictionary."<<std::endl;
	return true;
}

bool FeatureExtractor::_save_fdict_to_file(std::string dict_path)
{
	std::ofstream fp(dict_path.c_str(), std::ofstream::trunc);

	for(FDICT::const_iterator it = _fdict.begin(); it != _fdict.end(); ++it) {
		fp<<it->first<<"\t"<<it->second<<std::endl;

	}
	std::cerr<<"the feature dictionary path: "<<dict_path<<std::endl;
	std::cerr<<"save "<<_fdict.size()<<" columns into the feature dictionary."<<std::endl;

	return true;
}

size_t FeatureExtractor::_get_feature_id(const std::string& name, bool create_new)
{
	FDICT::iterator it = _fdict.lower_bound(name);

	if (it != _fdict.end() && ! (_fdict.key_comp()(name, it->first))) {
		return it->second;
	} else if (create_new) {
		// zero is reserverd for NOT FOUND
		_fdict.insert(it, FDICT::value_type(name, _fdict.size() + 1));
		return _fdict.size();
	} else {
		return 0;
	}
	
}

const Features FeatureExtractor::extract_for_train(const RawInstance& ins)
{
	return _extract_features(ins, true);
}

const Features FeatureExtractor::extract_for_predict(const RawInstance& ins)
{
	return _extract_features(ins, false);
}

bool FeatureExtractor::_append_feature(Features& features, const std::string& type, const std::string& name, double val, bool create_new)
{
	size_t fid = _get_feature_id(type + ":" + name, create_new);
	if (0==fid){
//		std::cerr<<"feature not exist in training set: "<<type<<":"<<name<<std::endl;
		return false;
	} else {
		features.push_back(Feature(fid, val));
		return true;
	}
}

//one more feature, to use the history ctr data
const Features FeatureExtractor::_extract_features(const RawInstance& ins, const bool create_new)
{
	Features ret;
	std::map<std::string, std::vector<std::string> > combined_features;
	combined_features["region"] = std::vector<std::string>();
	combined_features["tags"] = std::vector<std::string>();
	combined_features["host"] = std::vector<std::string>();
	combined_features["pid"] = std::vector<std::string>();


	{
		_append_feature(ret, "common", "all_feature", 1, create_new);
	}


	{//region

		_append_feature(ret, "region", ins.region, 1, create_new);
		std::vector<std::string> &region_vec = combined_features["region"];
		
		region_vec.push_back("r" + ins.region);
		std::vector<std::string> regions = _split_by_sep(ins.region, '-');
		if ( 2 < regions.size()) {
		    std::string province = regions[0] + "@" + regions[1];
		    region_vec.push_back("r" + province);
			_append_feature(ret, "region", province, 1, create_new);
		}
	/*	
		if ( 1 < regions.size()) {
		    std::string country = regions[0] + "@";
		    region_vec.push_back("r" + country);
			_append_feature(ret, "region", country, 1, create_new);
		}
	*/
	}//region

	{//user tags
		std::vector<std::string> utags = _split_by_sep(ins.user_tags, '|');
		std::vector<std::string> &tag_vec = combined_features["tags"];
		for (size_t idx = 0; idx < utags.size(); ++idx) {
			_append_feature(ret, "utag",  utags[idx], 1, create_new);
			tag_vec.push_back("ut"+utags[idx]);
		}
	}//user tags

	{//host
	    std::vector<std::string> &host_vec = combined_features["host"];
		_append_feature(ret, "host",  ins.host, 1, create_new);
		host_vec.push_back("h"+ins.host);

		std::vector<std::string> host_details = _split_by_sep(ins.host, '.');
		size_t hlen= host_details.size();
		if (2 < hlen) {
			// have sub domain
		    std::string sub_domain = host_details[hlen-2] + "@" + host_details[hlen-1];
			_append_feature(ret, "host", sub_domain, 1, create_new);
			host_vec.push_back("h" + sub_domain);
		}

	}//host

	_append_feature(ret, "pid", ins.pid, 1, create_new);
	combined_features["pid"].push_back("p" + ins.pid);
/*
	{
	    //add pid related features
	    if(pid_features.find(ins.pid) != pid_features.end()){
		const std::vector<std::string> &pid_vec = pid_features[ins.pid];
		for(std::size_t pid_index = 0; pid_index < pid_vec.size(); pid_index++){
		    _append_feature(ret,"pidf",pid_vec[pid_index],  1, create_new);
		}
	    }
	}
*/
	//generate combined features
	for(std::size_t i = 0; i < combined_features["region"].size(); i++){
	    for(std::size_t ii_ = 0; ii_ < combined_features["tags"].size(); ii_++){
		_append_feature(ret, "cb", combined_features["region"][i] + ":" + combined_features["tags"][ii_], 1, create_new);
	    }
	    for(std::size_t ii_ = 0; ii_ < combined_features["host"].size(); ii_++){
		_append_feature(ret, "cb", combined_features["region"][i] + ":" + combined_features["host"][ii_], 1, create_new);
	    }
	    for(std::size_t ii_ = 0; ii_ < combined_features["pid"].size(); ii_++){
		_append_feature(ret, "cb", combined_features["region"][i] + ":" + combined_features["pid"][ii_], 1, create_new);
	    }
	}
	for(std::size_t i = 0; i < combined_features["host"].size(); i++){

	    for(std::size_t ii_ = 0; ii_ < combined_features["pid"].size(); ii_++){
		_append_feature(ret, "cb", combined_features["host"][i] + ":" + combined_features["pid"][ii_], 1, create_new);
	    }
	}
	_append_feature(ret, "aid", ins.adver_id, 1, create_new);

	//liblinear need feature id in asc order
	std::sort(ret.begin(), ret.end());

	return ret;
}

std::vector<std::string> FeatureExtractor::_split_by_sep(const std::string& str, const char sep)
{
	std::vector<std::string> ret;

	std::istringstream ss(str);
	for (std::string field; std::getline(ss, field, sep);) {
		ret.push_back(field);
	}
	return ret;
}
