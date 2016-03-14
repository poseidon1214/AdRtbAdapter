#include "AdClassifier.h"

using namespace gfs;

Classifier::Classifier():_fe(false){
}

bool Classifier::init(std::string model_path)
{
	bool ret = true;
	
	ret |= _load_liblinear_model(model_path + "/model.liblinear");
	ret |= _init_feature_extractor(model_path + "/fdict.tsv");

	return ret;
}

double Classifier::predict_ctr(const RawInstance& instance)
{
	Features features(_fe.extract_for_predict(instance));

	// features are sparse, shouldn't overflow the stack. declare as local var to avoid memory fragment
	struct feature_node ll_features[features.size()+1];
	double probs[2];

	for (size_t idx=0; idx<features.size(); ++idx) {
		ll_features[idx].index 	= features[idx].idx;
		ll_features[idx].value	= features[idx].val;
	}

	// a dummy feature for EOF
	ll_features[features.size()].index = -1;

	predict_probability(_ll_model, ll_features, probs);
	return probs[0];
}

bool Classifier::_load_liblinear_model(std::string liblinear_model_path)
{
	_ll_model = load_model(liblinear_model_path.c_str());
	if (0==_ll_model) {
		return false;
	}
	return true;
}

bool Classifier::_init_feature_extractor(std::string fdict_path)
{
	return _fe.init(fdict_path);
}
