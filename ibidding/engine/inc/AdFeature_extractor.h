
#ifndef _FEATURE_HEAD_
#define _FEATURE_HEAD_

#include <string>
#include <vector>
#include <map>

namespace gfs {

struct Feature
{
public:
	Feature(size_t i, double v):idx(i), val(v){};
	size_t idx;
	double val;

	bool operator<(const Feature& rhs) const{
		return idx < rhs.idx;
	}
};

typedef std::vector<Feature> Features;

class RawInstance
{
public:
	std::string region;
	std::string os;
	std::string user_tags;
	std::string host;
	std::string page_type;
	std::string pid;
	std::string view_screen;
	std::string adver_id;
};

class FeatureExtractor
{
public:
	FeatureExtractor(bool istrain);
	~FeatureExtractor();

	bool init(const std::string dict_path);

	const Features extract_for_train(const RawInstance& ins);
	const Features extract_for_predict(const RawInstance& ins);

private:
	bool istrain;
	typedef std::map<std::string, size_t> FDICT;
	FDICT _fdict;
	std::string _fdict_path;

	bool _load_fdict_from_file(std::string dict_path);
	bool _save_fdict_to_file(std::string dict_path);

	size_t _get_feature_id(const std::string& name, bool create_new);
	std::vector<std::string> _split_by_sep(const std::string& str, const char sep);
	bool _append_feature(Features& features, const std::string& type, const std::string& name, double val, bool create_new);
	const Features _extract_features(const RawInstance& ins, bool create_new);
};

}//namespace gfs

#endif
