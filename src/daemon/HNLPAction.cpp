#include <iostream>
#include <sstream>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/StreamCopier.h>

#include "HNLPAction.h"

namespace pjs = Poco::JSON;

HNLPAction::HNLPAction()
{
}

HNLPAction::~HNLPAction()
{
}

void 
HNLPAction::setOpID( std::string id )
{
    m_opID = id;
}

std::string 
HNLPAction::getOpID()
{
    return m_opID;
}

void
HNLPAction::setResponseCode( uint HTTPResponseCode )
{

}

std::stringstream&
HNLPAction::setResponseStringStream( std::string contentType )
{
    m_rspContentType = contentType;

    return m_rspStream;
}

void 
HNLPAction::setNewID( std::string id )
{
    m_NewID = id;
}
        
bool 
HNLPAction::hasNewObject( std::string &newID )
{
    return false;
}

bool 
HNLPAction::hasRspContent()
{
    return false;
}

HNLP_AR_RESULT_T
HNLPAction::outputRspContent( std::ostream &ostr, std::string &contentType )
{
    contentType = m_rspContentType;

    // Render response content
    try{ 
        Poco::StreamCopier::copyStream( m_rspStream, ostr ); 
    } catch( ... ) {
        std::cout << "ERROR: Exception while copying stream to output" << std::endl;
    }

    return HNLP_AR_RESULT_SUCCESS;
}

HNLP_AR_RESULT_T
HNLPAction::getExecResult( uint& HTTPResponseCode )
{
    return HNLP_AR_RESULT_REQUEST_COMPLETE;
}


HNLPStatusAction::HNLPStatusAction()
{

}

HNLPStatusAction::~HNLPStatusAction()
{

}

HNLP_AR_TYPE_T
HNLPStatusAction::getType()
{
    return HNLP_AR_TYPE_DEVICE_STATUS;
}

HNLP_AR_RESULT_T
HNLPStatusAction::decodeRequest( HNOperationData *opData, uint& HTTPResponseCode )
{
    return HNLP_AR_RESULT_REQUEST_QUEUE;
}

bool
HNLPStatusAction::hasRspContent()
{
    return true;
}

HNLP_AR_RESULT_T
HNLPStatusAction::generateRspContent()
{
    std::cout << "=== Get Status Request ===" << std::endl;
    
    // Set response content type
    std::stringstream& ostr = setResponseStringStream( "application/json" );

    // Create a json status object
    pjs::Object jsRoot;
    jsRoot.set( "overallStatus", "OK" );

    // Render response content
    try{ 
        pjs::Stringifier::stringify( jsRoot, ostr, 1 ); 
    } catch( ... ) {
        std::cout << "ERROR: Exception while serializing comment" << std::endl;
    }

    // Set the HTTP response code
    setResponseCode( HNR_HTTP_OK );

    return HNLP_AR_RESULT_SUCCESS;
}

bool
HNLPStatusAction::hasNewObject( std::string &newID )
{
    return false;
}



HNLPSpecAction::HNLPSpecAction()
{

}

HNLPSpecAction::~HNLPSpecAction()
{

}

HNLP_AR_TYPE_T
HNLPSpecAction::getType()
{
    return HNLP_AR_TYPE_DEVICE_STATUS;
}

HNLP_AR_RESULT_T
HNLPSpecAction::decodeRequest( HNOperationData *opData, uint& HTTPResponseCode )
{
    return HNLP_AR_RESULT_REQUEST_REJECT;
}

bool
HNLPSpecAction::hasNewObject( std::string &newID )
{
    return false;
}

bool
HNLPSpecAction::hasRspContent()
{
    return false;
}


HNLPLayoutAction::HNLPLayoutAction()
{

}

HNLPLayoutAction::~HNLPLayoutAction()
{

}

HNLP_AR_TYPE_T
HNLPLayoutAction::getType()
{
    return HNLP_AR_TYPE_DEVICE_STATUS;
}

HNLP_AR_RESULT_T
HNLPLayoutAction::decodeRequest( HNOperationData *opData, uint& HTTPResponseCode )
{
    return HNLP_AR_RESULT_REQUEST_REJECT;
}

bool
HNLPLayoutAction::hasNewObject( std::string &newID )
{
    return false;
}

bool
HNLPLayoutAction::hasRspContent()
{
    return false;
}

HNLPPrinterAction::HNLPPrinterAction()
{

}

HNLPPrinterAction::~HNLPPrinterAction()
{

}

HNLP_AR_TYPE_T
HNLPPrinterAction::getType()
{
    return HNLP_AR_TYPE_DEVICE_PRINTER;
}

HNLP_AR_RESULT_T
HNLPPrinterAction::decodeRequest( HNOperationData *opData, uint& HTTPResponseCode )
{
    return HNLP_AR_RESULT_REQUEST_QUEUE;
}

HNLP_AR_RESULT_T
HNLPPrinterAction::generateRspContent( HNLPPrinterManager *printMgr )
{
    std::cout << "=== OP: " << getOpID() << " ===" << std::endl;

    if( "getPrinterList" == getOpID() )
    {
        std::vector< HNLPPrinter* > prtList;

        HNLP_PM_RESULT_T result = printMgr->getAvailablePrinterList( prtList );

        if( result != HNLP_PM_RESULT_SUCCESS )
        {

        }
  
        // Set response content type
        std::stringstream& ostr = setResponseStringStream( "application/json" );

        // Create a json status object
        pjs::Array jsPrtList;

        for( std::vector< HNLPPrinter* >::iterator it = prtList.begin(); it != prtList.end(); it++ )
        {
            pjs::Object jsPrint;

            jsPrint.set( "id", (*it)->getID() );
            jsPrint.set( "model", (*it)->getModel() );
            jsPrint.set( "info", (*it)->getInfo() );
            jsPrint.set( "location", (*it)->getLocation() );
            jsPrint.set( "uri", (*it)->getURI() );
            jsPrint.set( "accepting", (*it)->isAccepting() ? true : false );
            jsPrint.set( "shared", (*it)->isShared() ? true : false );

            jsPrtList.add( jsPrint );
        }

        // Render response content
        try{ 
            pjs::Stringifier::stringify( jsPrtList, ostr, 1 ); 
        } catch( ... ) {
            std::cout << "ERROR: Exception while serializing comment" << std::endl;
        }

        // Set the HTTP response code
        setResponseCode( HNR_HTTP_OK );
    }
    else
    {
        return HNLP_AR_RESULT_REQUEST_REJECT;
    }

    return HNLP_AR_RESULT_SUCCESS;
}

bool
HNLPPrinterAction::hasNewObject( std::string &newID )
{
    return false;
}

bool
HNLPPrinterAction::hasRspContent()
{
    return true;
}


HNLPAction* 
HNLPActionFactory::allocateAction( std::string opID )
{
    HNLPAction *action = NULL;

    if( "getStatus" == opID )
        action = new HNLPStatusAction;
    else if( "getPrinterList" == opID )
        action = new HNLPPrinterAction;
    else if( "getActivePrinterInfo" == opID )
        action = new HNLPPrinterAction;
    else if( "putActivePrinter" == opID )
        action = new HNLPPrinterAction;
    else if( "getLabelSpecifications" == opID )
        action = new HNLPSpecAction;
    else if( "createLabelSpecification" == opID )
        action = new HNLPSpecAction;
    else if( "deleteLabelSpecification" == opID )
        action = new HNLPSpecAction;
    else if( "getLabelLayouts" == opID )
        action = new HNLPLayoutAction;
    else if( "putLabelTemplate" == opID )
        action = new HNLPLayoutAction;
    else if( "deleteLabelTemplate" == opID )
        action = new HNLPLayoutAction;
    else if( "getPrintJobs" == opID )
        action = new HNLPPrinterAction;
    else if( "postPrintRequest" == opID )
        action = new HNLPPrinterAction;
    else if( "deletePrintJob" == opID )
        action = new HNLPPrinterAction;

    if( action != NULL )
        action->setOpID( opID );

    return action;
}

void
HNLPActionFactory::deallocateAction( HNLPAction *action )
{
    delete action;
}