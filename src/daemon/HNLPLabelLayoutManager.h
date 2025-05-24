#ifndef __HNLP_LABEL_LAYOUT_MANAGER_H__
#define __HNLP_LABEL_LAYOUT_MANAGER_H__

#include <string>
#include <map>
#include <vector>

#include <Poco/JSON/Object.h>

#include <cups/cups.h>

#include <hnode2/HNodeConfig.h>

typedef enum HNLPContentAreaType
{
    HNLP_LLCA_TYPE_TEXTBOX
}HNLP_LLCA_TYPE_T;

typedef enum HNLPLabelLayoutManagerResult
{
    HNLP_LL_RESULT_SUCCESS,
    HNLP_LL_RESULT_FAILURE  
}HNLP_LL_RESULT_T;

class HNLPLabelContentArea
{
    public:
        HNLPLabelContentArea();
       ~HNLPLabelContentArea();

        void setID( std::string value );
        std::string getID();

        virtual HNLP_LLCA_TYPE_T getType() = 0;

        virtual HNLP_LL_RESULT_T initFromJSONObject( Poco::JSON::Object::Ptr defObj ) = 0;

        virtual void debugPrint() = 0;

    private:

        std::string m_ID;
};

class HNLPLabelTextContent : public HNLPLabelContentArea
{
    public:
        HNLPLabelTextContent();
       ~HNLPLabelTextContent();

        virtual HNLP_LLCA_TYPE_T getType();

        virtual HNLP_LL_RESULT_T initFromJSONObject( Poco::JSON::Object::Ptr defObj );

        virtual void debugPrint();

    private:

        HNLP_LL_RESULT_T initBoundingBoxFromJSONObject( Poco::JSON::Object::Ptr bbObj );
        HNLP_LL_RESULT_T initTextPropertiesFromJSONObject( Poco::JSON::Object::Ptr tpObj );

        double m_bbWidthInset;
        double m_bbLengthInset;
        double m_bbWidth;
        double m_bbLength;
        double m_bbOrientation;

        std::string m_tpFormatStr;
        std::string m_tpFont;

        double m_tpFontSize;
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

        std::vector< HNLPLabelContentArea* > m_contentAreaList;
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
        
        HNLPLabelLayout* getLayout( std::string id );

        void debugPrint();

    private:
        std::string getNextUniqueID();

//        HNLPPrinter* getOrCreatePrinter( std::string idBuf );

        std::map< std::string, HNLPLabelLayout* > m_layoutList;

        uint m_nextLayoutIndex;
};

#endif // __HNLP_LABEL_LAYOUT_MANAGER_H__
