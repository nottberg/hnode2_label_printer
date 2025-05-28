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

typedef enum HNLPLabelBoundaryType
{
    HNLP_LB_TYPE_NOTSET,
    HNLP_LB_TYPE_SQUARE
}HNLP_LB_TYPE_T;

class HNLPLabelBoundary
{
    public:
        HNLPLabelBoundary();
        ~HNLPLabelBoundary();

        virtual HNLP_LB_TYPE_T getType() = 0;

        virtual HNLP_LS_RESULT_T initFromJSONObject( Poco::JSON::Object::Ptr defObj ) = 0;

        virtual void debugPrint() = 0;

    private:
};

class HNLPSquareLabelBoundary : public HNLPLabelBoundary
{
    public:
        HNLPSquareLabelBoundary();
        ~HNLPSquareLabelBoundary();

        virtual HNLP_LB_TYPE_T getType();

        virtual HNLP_LS_RESULT_T initFromJSONObject( Poco::JSON::Object::Ptr defObj );

        double getWidth();
        double getLength();

        virtual void debugPrint();

    private:

        double m_width;
        double m_length;
};

typedef enum HNLPAreaBoundaryType
{
    HNLP_AB_TYPE_NOTSET,
    HNLP_AB_TYPE_SQUARE
}HNLP_AB_TYPE_T;

class HNLPAreaBoundary
{
    public:
        HNLPAreaBoundary();
        ~HNLPAreaBoundary();

        virtual HNLP_AB_TYPE_T getType() = 0;

        virtual HNLP_LS_RESULT_T initFromJSONObject( Poco::JSON::Object::Ptr defObj ) = 0;

        virtual void debugPrint() = 0;

    private:
};

class HNLPSquareBoundary : public HNLPAreaBoundary
{
    public:
        HNLPSquareBoundary();
        ~HNLPSquareBoundary();

        virtual HNLP_AB_TYPE_T getType();

        virtual HNLP_LS_RESULT_T initFromJSONObject( Poco::JSON::Object::Ptr defObj );

        double getInsetWidth();
        double getInsetLength();
        double getWidth();
        double getLength();

        virtual void debugPrint();

    private:

        double m_insetWidth;
        double m_insetLength;
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

        HNLP_LB_TYPE_T getLabelBoundaryType();
        bool isLabelBoundary( HNLP_LB_TYPE_T type );

        double getLabelBoundaryWidth();
        double getLabelBoundaryLength();

        HNLP_AB_TYPE_T getImagingBoundaryType();
        bool isImagingBoundary( HNLP_AB_TYPE_T type );

        double getImagingBoundaryInsetWidth();
        double getImagingBoundaryInsetLength();

        double getImagingBoundaryWidth();
        double getImagingBoundaryLength();

        void clear();

        HNLP_LS_RESULT_T initFromJSONObject( Poco::JSON::Object::Ptr defObj );
         
        void debugPrint();

    private:
        std::string m_id;

        std::string m_vendorName;
        std::string m_vendorLabelID;
        std::string m_vendorDescription;

        std::string m_color;
        std::string m_cupsMedia;

        HNLPLabelBoundary *m_labelBoundary;

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

        HNLPLabelSpec* getSpec( std::string id );

        void debugPrint();

    private:

        std::string getNextUniqueID();

//        HNLPPrinter* getOrCreatePrinter( std::string idBuf );

        uint m_nextSpecIndex;

        std::map< std::string, HNLPLabelSpec* > m_specList;
};

#endif // __HNLP_LABEL_SPEC_MANAGER_H__
