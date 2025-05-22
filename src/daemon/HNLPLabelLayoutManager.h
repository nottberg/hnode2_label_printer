#ifndef __HNLP_LABEL_LAYOUT_MANAGER_H__
#define __HNLP_LABEL_LAYOUT_MANAGER_H__

#include <string>
#include <map>
#include <vector>

#include <Poco/JSON/Object.h>

#include <cups/cups.h>

#include <hnode2/HNodeConfig.h>

typedef enum HNLPLabelLayoutManagerResult
{
    HNLP_LL_RESULT_SUCCESS,
    HNLP_LL_RESULT_FAILURE  
}HNLP_LL_RESULT_T;

class HNLPLabelContentAreaBase
{
    public:
        HNLPLabelContentAreaBase();
       ~HNLPLabelContentAreaBase();

    private:

};

class HNLPLabelTextContent : public HNLPLabelContentAreaBase
{
    public:
        HNLPLabelTextContent();
       ~HNLPLabelTextContent();

    private:

};

class HNLPLabelTarget
{
    public:
        HNLPLabelTarget();
       ~HNLPLabelTarget();

       void setVendorName( std::string value );
       void setVendorLabelID( std::string value );

    private:

        std::string m_vendorName;
        std::string m_vendorLabelID;
};

class HNLPLabelLayout
{
    public:
        HNLPLabelLayout();
       ~HNLPLabelLayout();

        void setID( std::string value );
        std::string getID();

        HNLP_LL_RESULT_T initFromJSONObject( Poco::JSON::Object::Ptr defObj );
 
        void debugPrint();

    private:
        std::string m_id;

        std::string m_layoutID;
        std::string m_layoutName;
        std::string m_layoutDescription;

        std::vector< HNLPLabelTarget > m_tgtLabelList;

        std::vector< HNLPLabelContentAreaBase* > m_contentAreaList;
};

class HNLPLabelLayoutManager
{
    public:
        HNLPLabelLayoutManager();
       ~HNLPLabelLayoutManager();

        HNLP_LL_RESULT_T init();

        HNLP_LL_RESULT_T initConfigSection( HNodeConfig &cfg );
        HNLP_LL_RESULT_T readConfigSection( HNodeConfig &cfg );
        HNLP_LL_RESULT_T updateConfigSection( HNodeConfig &cfg );

        HNLP_LL_RESULT_T resetDefinitions();

        HNLP_LL_RESULT_T defineLayoutFromJSONObject( Poco::JSON::Object::Ptr defObj );

        void debugPrint();

    private:
        std::string getNextUniqueID();

//        HNLPPrinter* getOrCreatePrinter( std::string idBuf );

        std::map< std::string, HNLPLabelLayout* > m_layoutList;

        uint m_nextLayoutIndex;
};

#endif // __HNLP_LABEL_LAYOUT_MANAGER_H__
