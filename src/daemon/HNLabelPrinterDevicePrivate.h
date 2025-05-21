#ifndef __HN_LABEL_PRINTER_DEVICE_PRIVATE_H__
#define __HN_LABEL_PRINTER_DEVICE_PRIVATE_H__

#include <string>
#include <vector>

#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/OptionSet.h"

#include <hnode2/HNodeDevice.h>
#include <hnode2/HNodeConfig.h>
#include <hnode2/HNEPLoop.h>
#include <hnode2/HNReqWaitQueue.h>

#include "HNLPAction.h"

#define HNODE_LABEL_PRINTER_DEVTYPE   "hnode2-label-printer-device"

typedef enum HNLabelPrinterDeviceResultEnum
{
  HNLPD_RESULT_SUCCESS,
  HNLPD_RESULT_FAILURE,
  HNLPD_RESULT_BAD_REQUEST,
  HNLPD_RESULT_SERVER_ERROR
}HNLPD_RESULT_T;

class HNLabelPrinterDevice : public Poco::Util::ServerApplication, public HNDEPDispatchInf, public HNDEventNotifyInf, public HNEPLoopCallbacks 
{
    private:
        bool _helpRequested   = false;
        bool _debugLogging    = false;
        bool _instancePresent = false;

        std::string _instance; 
        std::string m_instanceName;

        HNodeDevice m_hnodeDev;

        HNLPPrinterManager m_printerMgr;

        HNLPLabelSpecManager m_specMgr;

        HNLPLabelLayoutManager m_layoutMgr;

        HNLPAction     *m_curUserAction;
        HNReqWaitQueue  m_userActionQueue;

        HNEPTrigger m_configUpdateTrigger;

        HNEPLoop m_testDeviceEvLoop;

        // Format string codes
        uint m_errStrCode;
        uint m_noteStrCode;

        // Health component ids
        std::string m_hc1ID;
        std::string m_hc2ID;
        std::string m_hc3ID;

        // Keep track of health state change simulation
        uint m_healthStateSeq;

        void displayHelp();

        bool configExists();
        HNLPD_RESULT_T initConfig();
        HNLPD_RESULT_T readConfig();
        HNLPD_RESULT_T updateConfig();

        void generateNewHealthState();

        void startAction();

        HNLPD_RESULT_T loadLabelDefinitionsFromLibrary();

    protected:
        // HNDevice REST callback
        virtual void dispatchEP( HNodeDevice *parent, HNOperationData *opData );

        // Notification for hnode device config changes.
        virtual void hndnConfigChange( HNodeDevice *parent );

        // Event loop functions
        virtual void loopIteration();
        virtual void timeoutEvent();
        virtual void fdEvent( int sfd );
        virtual void fdError( int sfd );

        // Poco funcions
        void defineOptions( Poco::Util::OptionSet& options );
        void handleOption( const std::string& name, const std::string& value );
        int main( const std::vector<std::string>& args );


};

#endif // __HN_LABEL_PRINTER_DEVICE_PRIVATE_H__
