#ifndef __HNLP_LABEL_SPEC_MANAGER_H__
#define __HNLP_LABEL_SPEC_MANAGER_H__

#include <string>
#include <map>
#include <vector>

#include <cups/cups.h>

#include <Poco/JSON/Object.h>

#include <hnode2/HNodeConfig.h>

typedef enum HNLPLabelSpecManagerResult
{
    HNLP_LS_RESULT_SUCCESS,
    HNLP_LS_RESULT_FAILURE  
}HNLP_LS_RESULT_T;

typedef enum HNLPLabelSpecMeasurmentUnits
{
    HNLP_UNIT_MM
}HNLP_UNIT_T;

class HNLPAreaBoundary
{
    public:
        HNLPAreaBoundary();
        ~HNLPAreaBoundary();

        virtual HNLP_LS_RESULT_T initFromJSONObject( Poco::JSON::Object::Ptr defObj ) = 0;

        virtual void debugPrint() = 0;

    private:
};

class HNLPSquareBoundary : public HNLPAreaBoundary
{
    public:
        HNLPSquareBoundary();
        ~HNLPSquareBoundary();

        virtual HNLP_LS_RESULT_T initFromJSONObject( Poco::JSON::Object::Ptr defObj );

        virtual void debugPrint();

    private:

        double m_width;
        double m_length;
};

class HNLPLabelSpec
{
    public:
        HNLPLabelSpec();
       ~HNLPLabelSpec();

        void setID( std::string value );
        std::string getID();

        void setDescription( std::string value );
        std::string getDescription();
    
        void setVendor( std::string value );
        std::string getVendor();

        void setVendorRefNum( std::string value );
        std::string getVendorRefNum();

        void setWidthFromStr( std::string value );
        void setWidth( double value, HNLP_UNIT_T units );
        double getWidth( HNLP_UNIT_T units );

        void setHeightFromStr( std::string value );
        void setHeight( double value, HNLP_UNIT_T units );
        double getHeight( HNLP_UNIT_T units );

        void setMarginWidthFromStr( std::string value );
        void setMarginWidth( double value, HNLP_UNIT_T units );
        double getMarginWidth( HNLP_UNIT_T units );

        void setMarginHeightFromStr( std::string value );
        void setMarginHeight( double value, HNLP_UNIT_T units );
        double getMarginHeight( HNLP_UNIT_T units );

        void clear();

        HNLP_LS_RESULT_T initFromJSONObject( Poco::JSON::Object::Ptr defObj );
 
        void debugPrint();

    private:
        std::string m_id;

        std::string m_vendorName;
        std::string m_vendorLabelID;
        std::string m_vendorDescription;

        std::string m_color;

        HNLPAreaBoundary *m_areaBoundary;
};

/*
"LabelSpecification":{
    "vendorName":"Dymo",
    "vendorLabelID":"30252",
    "vendorDescription":"White Address Labels",
    "color":"white",

    "printableAreaBoundary":{
        "square":{
            "width":28,
            "length":89
        }
    }
}
*/

class HNLPLabelSpecManager
{
    public:
        HNLPLabelSpecManager();
       ~HNLPLabelSpecManager();

        HNLP_LS_RESULT_T init();

        HNLP_LS_RESULT_T initConfigSection( HNodeConfig &cfg );
        HNLP_LS_RESULT_T readConfigSection( HNodeConfig &cfg );
        HNLP_LS_RESULT_T updateConfigSection( HNodeConfig &cfg );

        HNLP_LS_RESULT_T getLabelSpecificationList( std::vector< HNLPLabelSpec* > &list );

        HNLP_LS_RESULT_T resetDefinitions();

        HNLP_LS_RESULT_T defineSpecificationFromJSONObject( Poco::JSON::Object::Ptr defObj );

        //HNLP_LS_RESULT_T createNewSpecification( HNLPLabelSpec **newPtr );

        void debugPrint();

    private:

        std::string getNextUniqueID();

//        HNLPPrinter* getOrCreatePrinter( std::string idBuf );

        uint m_nextSpecIndex;

        std::map< std::string, HNLPLabelSpec* > m_specList;
};

#endif // __HNLP_LABEL_SPEC_MANAGER_H__
