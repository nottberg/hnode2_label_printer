#ifndef __HNLP_ACTION_REQUEST_H__
#define __HNLP_ACTION_REQUEST_H__

#include <stdint.h>

#include <string>
#include <mutex>
#include <thread>
#include <sstream>
#include <vector>

#include <hnode2/HNRestHandler.h>
#include <hnode2/HNReqWaitQueue.h>

#include "HNLPPrinterManager.h"

typedef enum HNLPActionRequestType 
{
    HNLP_AR_TYPE_NOTSET                = 0,
    HNLP_AR_TYPE_DEVICE_STATUS         = 1,
    HNLP_AR_TYPE_DEVICE_PRINTER        = 2  
}HNLP_AR_TYPE_T;

typedef enum HNIDStartActionBitsEnum
{
    HNID_ACTBIT_CLEAR     = 0x0000,
    HNID_ACTBIT_COMPLETE  = 0x0001,
    HNID_ACTBIT_UPDATE    = 0x0002,
//    HNID_ACTBIT_RECALCSCH = 0x0004,
    HNID_ACTBIT_ERROR     = 0x0008,
//    HNID_ACTBIT_SENDREQ   = 0x0010
} HNID_ACTBIT_T;

#if 0
typedef enum HNLPActionZoneUpdateMaskEnum
{
    HNLP_ZU_FLDMASK_CLEAR  = 0x00000000,
    HNLP_ZU_FLDMASK_NAME   = 0x00000001,
    HNLP_ZU_FLDMASK_DESC   = 0x00000002,
    HNLP_ZU_FLDMASK_SPW    = 0x00000004,
    HNLP_ZU_FLDMASK_MAXSPC = 0x00000008,
    HNLP_ZU_FLDMASK_MINSPC = 0x00000010,
    HNLP_ZU_FLDMASK_SWLST  = 0x00000020
}HNLP_ZU_FLDMASK_T;
#endif

typedef enum HNLPActionRequestResult
{
    HNLP_AR_RESULT_SUCCESS,
    HNLP_AR_RESULT_FAILURE,
    HNLP_AR_RESULT_REQUEST_QUEUE,
    HNLP_AR_RESULT_REQUEST_REJECT,
    HNLP_AR_RESULT_REQUEST_COMPLETE    
}HNLP_AR_RESULT_T;

class HNLPAction : public HNReqWaitAction
{
    public:
        HNLPAction();
       ~HNLPAction();

        void setOpID( std::string opID );
        std::string getOpID();

        void setResponseCode( uint HTTPResponseCode );

        std::stringstream& setResponseStringStream( std::string contentType );

        virtual HNLP_AR_TYPE_T getType() = 0;
    
        virtual HNLP_AR_RESULT_T decodeRequest( HNOperationData *opData, uint& HTTPResponseCode ) = 0;
    
        void setNewID( std::string id );

        virtual bool hasRspContent();

        HNLP_AR_RESULT_T outputRspContent( std::ostream &ostr, std::string &contentType );

        virtual bool hasNewObject( std::string &newID );

        HNLP_AR_RESULT_T getExecResult( uint& HTTPResponseCode );

    private:
        std::string m_opID;

        std::string m_NewID;

        std::string m_rspContentType;

        std::stringstream m_rspStream;
};

class HNLPStatusAction : public HNLPAction
{
    public:
        HNLPStatusAction();
       ~HNLPStatusAction();

        virtual HNLP_AR_TYPE_T getType();

        virtual HNLP_AR_RESULT_T decodeRequest( HNOperationData *opData, uint& HTTPResponseCode );

        HNLP_AR_RESULT_T generateRspContent();

        virtual bool hasNewObject( std::string &newID );
        virtual bool hasRspContent();

    private:

};

class HNLPSpecAction : public HNLPAction
{
    public:
        HNLPSpecAction();
       ~HNLPSpecAction();

        virtual HNLP_AR_TYPE_T getType();

        virtual HNLP_AR_RESULT_T decodeRequest( HNOperationData *opData, uint& HTTPResponseCode );

        virtual bool hasNewObject( std::string &newID );
        virtual bool hasRspContent();

    private:

};

class HNLPLayoutAction : public HNLPAction
{
    public:
        HNLPLayoutAction();
       ~HNLPLayoutAction();

        virtual HNLP_AR_TYPE_T getType();

        virtual HNLP_AR_RESULT_T decodeRequest( HNOperationData *opData, uint& HTTPResponseCode );
    
        virtual bool hasNewObject( std::string &newID );
        virtual bool hasRspContent();

    private:

};

class HNLPPrinterAction : public HNLPAction
{
    public:
        HNLPPrinterAction();
       ~HNLPPrinterAction();

        virtual HNLP_AR_TYPE_T getType();

        virtual HNLP_AR_RESULT_T decodeRequest( HNOperationData *opData, uint& HTTPResponseCode );
    
        HNLP_AR_RESULT_T generateRspContent( HNLPPrinterManager *printMgr );

        virtual bool hasNewObject( std::string &newID );
        virtual bool hasRspContent();

    private:

};

class HNLPActionFactory
{
    public:
        static HNLPAction* allocateAction( std::string opStr );
        static void deallocateAction( HNLPAction *action );
};

#endif // __HNLP_ACTION_REQUEST_H__