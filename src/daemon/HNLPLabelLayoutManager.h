#ifndef __HNLP_LABEL_LAYOUT_MANAGER_H__
#define __HNLP_LABEL_LAYOUT_MANAGER_H__

#include <string>
#include <map>
#include <vector>

#include <cups/cups.h>

#include <hnode2/HNodeConfig.h>

typedef enum HNLPLabelLayoutManagerResult
{
    HNLP_LL_RESULT_SUCCESS,
    HNLP_LL_RESULT_FAILURE  
}HNLP_LL_RESULT_T;

class HNLPLabelLayout
{
    public:
        HNLPLabelLayout();
       ~HNLPLabelLayout();

        void setID( std::string value );
        std::string getID();

    private:
        std::string m_id;
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
        
    //    HNLP_PM_RESULT_T getAvailablePrinterList( std::vector< HNLPPrinter* > &list );

    //    HNLP_PM_RESULT_T getActivePrinter( HNLPPrinter **activePrinter );

    //    HNLP_PM_RESULT_T setActivePrinterByID( std::string id );

    //    void clearActivePrinter();

    private:
//        HNLPPrinter* getOrCreatePrinter( std::string idBuf );

        std::map< std::string, HNLPLabelLayout* > m_layoutList;
};

#endif // __HNLP_LABEL_LAYOUT_MANAGER_H__
