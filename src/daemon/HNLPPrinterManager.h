#ifndef __HNLP_PRINTER_MANAGER_H__
#define __HNLP_PRINTER_MANAGER_H__

#include <string>
#include <map>
#include <vector>

#include <cups/cups.h>

typedef enum HNLPActionPrinterManagerResult
{
    HNLP_PM_RESULT_SUCCESS,
    HNLP_PM_RESULT_FAILURE  
}HNLP_PM_RESULT_T;

class HNLPPrinter
{
    public:
        HNLPPrinter();
       ~HNLPPrinter();

        void setID( std::string value );
        std::string getID();

        void setModel( std::string value );
        std::string getModel();

        void setInfo( std::string value );
        std::string getInfo();

        void setLocation( std::string value );
        std::string getLocation();

        void setURI( std::string value );
        std::string getURI();

        void setAccepting( bool value );
        bool isAccepting();

        void setShared( bool value );
        bool isShared();

    private:
        std::string m_id;

        std::string m_model;

        std::string m_info;

        std::string m_location;

        std::string m_uri;
        
        bool m_accepting;

        bool m_shared;
};

class HNLPPrinterManager
{
    public:
        HNLPPrinterManager();
       ~HNLPPrinterManager();

        HNLP_PM_RESULT_T init();
 
        HNLP_PM_RESULT_T getAvailablePrinterList( std::vector< HNLPPrinter* > &list );

        HNLP_PM_RESULT_T getActivePrinter( HNLPPrinter **activePrinter );

        HNLP_PM_RESULT_T setActivePrinterByID( std::string id );

    private:

        void executePrinterListRefresh();

        static int printerInfoCallback( void *user_data, unsigned flags, cups_dest_t *dest );

        HNLPPrinter* getOrCreatePrinter( std::string idBuf );

        std::map< std::string, HNLPPrinter* > m_printerList;

        HNLPPrinter *m_activePrinter;
};

#endif // __HNLP_PRINTER_MANAGER_H__
