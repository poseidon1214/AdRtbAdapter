
#ifndef _CLASS_HEAD_
#define _CLASS_HEAD_

#include "AdFeature_extractor.h"

#include <string>

#include "linear.h"

namespace gfs
{
class Classifier
{
public:
	bool init(std::string model_path);
	double predict_ctr(const RawInstance& instance);
	Classifier();
    virtual ~Classifier(){
        if(_ll_model != NULL){
            free_and_destroy_model((struct model **)&_ll_model);
            _ll_model = NULL;
        }
    }
private:
	const struct model* _ll_model;
	FeatureExtractor _fe;

	bool _load_liblinear_model(std::string liblinear_model_path);
	bool _init_feature_extractor(std::string fdict_path);


};

}//gfs

#endif
