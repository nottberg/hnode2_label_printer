#include <iostream>
#include <sstream>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/StreamCopier.h>

#include "HNLPAction.h"

namespace pjs = Poco::JSON;
namespace pdy = Poco::Dynamic;

HNLPAction::HNLPAction()
{
    m_configChange = false;
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
HNLPAction::setConfigChange( bool value )
{
    m_configChange = value;
}

bool
HNLPAction::hasConfigChange()
{
    return m_configChange;
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
    return HNLP_AR_TYPE_LABEL_SPEC;
}

HNLP_AR_RESULT_T
HNLPSpecAction::decodeRequest( HNOperationData *opData, uint& HTTPResponseCode )
{
    std::string opID = opData->getOpID();

    // POST "/hnode2/"
    if( "createLabelSpecification" == opID )
    {
        m_newSpecReq.clear();

        std::istream& rs = opData->requestBody();

        try 
        {
            // Attempt to parse the json
            pjs::Parser parser;
            pdy::Var varRoot = parser.parse( rs );

            // Get a pointer to the root object
            pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

            if( jsRoot->has( "description" ) )
            {
                m_newSpecReq.setDescription( jsRoot->getValue<std::string>( "description" ) );
            }

            if( jsRoot->has( "vendorRefNum" ) )
            {
                m_newSpecReq.setVendorRefNum( jsRoot->getValue<std::string>( "vendorRefNum" ) );
            }

            if( jsRoot->has( "vendorName" ) )
            {
                m_newSpecReq.setVendor( jsRoot->getValue<std::string>( "vendorName" ) );
            }

            if( jsRoot->has( "width" ) )
            {
                m_newSpecReq.setWidthFromStr( jsRoot->getValue<std::string>( "width" ) );
            }

            if( jsRoot->has( "height" ) )
            {
                m_newSpecReq.setHeightFromStr( jsRoot->getValue<std::string>( "height" ) );
            }

            if( jsRoot->has( "marginWidth" ) )
            {
                m_newSpecReq.setMarginWidthFromStr( jsRoot->getValue<std::string>( "marginWidth" ) );
            }

            if( jsRoot->has( "marginHeight" ) )
            {
                m_newSpecReq.setMarginHeightFromStr( jsRoot->getValue<std::string>( "marginHeight" ) );
            }
        }
        catch( Poco::Exception ex )
        {
            std::cout << "HNLPPrinterAction::decodeRequest exception: " << ex.displayText() << std::endl;
            HTTPResponseCode = HNR_HTTP_INTERNAL_SERVER_ERROR;
            return HNLP_AR_RESULT_REQUEST_REJECT;
        }
    }
    // DELETE "/hnode2/"
    else if( "deleteLabelSpecification" == opID )
    {

    }

    return HNLP_AR_RESULT_REQUEST_REJECT;
}

HNLP_AR_RESULT_T
HNLPSpecAction::generateRspContent( HNLPLabelSpecManager *specMgr )
{   
    std::string opID = getOpID();

    std::cout << "=== OP: " << opID << " ===" << std::endl;

    // GET "/hnode2/"
    if( "getLabelSpecifications" == opID )
    {
        std::vector< HNLPLabelSpec* > lsList;

        HNLP_LS_RESULT_T result = specMgr->getLabelSpecificationList( lsList );

        if( result != HNLP_LS_RESULT_SUCCESS )
        {

        }
  
        // Set response content type
        std::stringstream& ostr = setResponseStringStream( "application/json" );

        // Create a json status object
        pjs::Array jsLabelSpecList;

        for( std::vector< HNLPLabelSpec* >::iterator it = lsList.begin(); it != lsList.end(); it++ )
        {
            pjs::Object jsLabelSpec;

            jsLabelSpec.set( "id", (*it)->getID() );

            jsLabelSpecList.add( jsLabelSpec );
        }

        // Render response content
        try{ 
            pjs::Stringifier::stringify( jsLabelSpecList, ostr, 1 ); 
        } catch( ... ) {
            std::cout << "ERROR: Exception while serializing comment" << std::endl;
        }

        // Set the HTTP response code
        setResponseCode( HNR_HTTP_OK );

    }
    // POST "/hnode2/"
    else if( "createLabelSpecification" == opID )
    {
        HNLPLabelSpec *newSpec = NULL;

        //if( specMgr->createNewSpecification( &newSpec ) != HNLP_LS_RESULT_SUCCESS )
        //{
        //    return HNLP_AR_RESULT_REQUEST_REJECT;
        //}
    }
    // DELETE "/hnode2/"
    else if( "deleteLabelSpecification" == opID )
    {
        //printMgr->clearActivePrinter();
        setConfigChange( true );
        setResponseCode( HNR_HTTP_OK );
    }
    else
    {
        return HNLP_AR_RESULT_REQUEST_REJECT;
    }

    return HNLP_AR_RESULT_SUCCESS;
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

#if 0
HNLP_AR_RESULT_T
HNLPLayoutAction::generateRspContent( HNLPLabelSpecManager *printMgr )
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
    else if( "getActivePrinterInfo" == getOpID() )  
    {
        HNLPPrinter *activePrinter = NULL;
        pjs::Object jsRsp;
        pjs::Object jsPrintInfo;

        if( printMgr->getActivePrinter( &activePrinter ) == HNLP_PM_RESULT_SUCCESS )
        {
            jsPrintInfo.set( "id", activePrinter->getID() );
            jsPrintInfo.set( "model", activePrinter->getModel() );
            jsPrintInfo.set( "info", activePrinter->getInfo() );
            jsPrintInfo.set( "location", activePrinter->getLocation() );
            jsPrintInfo.set( "uri", activePrinter->getURI() );
            jsPrintInfo.set( "accepting", activePrinter->isAccepting() ? true : false );
            jsPrintInfo.set( "shared", activePrinter->isShared() ? true : false );

            jsRsp.set( "activePrinterID", activePrinter->getID() );
            jsRsp.set( "activePrinterInfo", jsPrintInfo );    
        }
        else
        {
            jsRsp.set( "activePrinterID", "" );
            jsRsp.set( "activePrinterInfo", jsPrintInfo );    
        }

        // Set response content type
        std::stringstream& ostr = setResponseStringStream( "application/json" );

        // Render response content
        try{ 
            pjs::Stringifier::stringify( jsRsp, ostr, 1 ); 
        } catch( ... ) {
            std::cout << "ERROR: Exception while serializing comment" << std::endl;
        }

        // Set the HTTP response code
        setResponseCode( HNR_HTTP_OK );
    }
    else if( "putActivePrinter" == getOpID() )
    {
        if( printMgr->setActivePrinterByID( m_newID ) == HNLP_PM_RESULT_SUCCESS )
        {
            setConfigChange( true );

            // Set the HTTP response code
            setResponseCode( HNR_HTTP_OK );
        }
        else
        {
            // Set the HTTP response code
            setResponseCode( HNR_HTTP_NOT_FOUND );
        }
    }
    else if( "clearActivePrinter" == getOpID() )
    {
        printMgr->clearActivePrinter();
        setConfigChange( true );
        setResponseCode( HNR_HTTP_OK );
    }
    else
    {
        return HNLP_AR_RESULT_REQUEST_REJECT;
    }

    return HNLP_AR_RESULT_SUCCESS;
}
#endif

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
    std::cout << "=== HNLPPrinterAction::decodeRequest - OP: " << getOpID() << " ===" << std::endl;

    if( "putActivePrinter" == getOpID() )
    {
        m_newID.clear();

        std::istream& rs = opData->requestBody();

        try 
        {
            // Attempt to parse the json
            pjs::Parser parser;
            pdy::Var varRoot = parser.parse( rs );

            // Get a pointer to the root object
            pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

            if( jsRoot->has( "activePrinterID" ) )
                m_newID = jsRoot->getValue<std::string>( "activePrinterID" );
        }
        catch( Poco::Exception ex )
        {
            std::cout << "HNLPPrinterAction::decodeRequest exception: " << ex.displayText() << std::endl;
            HTTPResponseCode = HNR_HTTP_INTERNAL_SERVER_ERROR;
            return HNLP_AR_RESULT_REQUEST_REJECT;
        }
    }

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
    else if( "getActivePrinterInfo" == getOpID() )  
    {
        HNLPPrinter *activePrinter = NULL;
        pjs::Object jsRsp;
        pjs::Object jsPrintInfo;

        if( printMgr->getActivePrinter( &activePrinter ) == HNLP_PM_RESULT_SUCCESS )
        {
            jsPrintInfo.set( "id", activePrinter->getID() );
            jsPrintInfo.set( "model", activePrinter->getModel() );
            jsPrintInfo.set( "info", activePrinter->getInfo() );
            jsPrintInfo.set( "location", activePrinter->getLocation() );
            jsPrintInfo.set( "uri", activePrinter->getURI() );
            jsPrintInfo.set( "accepting", activePrinter->isAccepting() ? true : false );
            jsPrintInfo.set( "shared", activePrinter->isShared() ? true : false );

            jsRsp.set( "activePrinterID", activePrinter->getID() );
            jsRsp.set( "activePrinterInfo", jsPrintInfo );    
        }
        else
        {
            jsRsp.set( "activePrinterID", "" );
            jsRsp.set( "activePrinterInfo", jsPrintInfo );    
        }

        // Set response content type
        std::stringstream& ostr = setResponseStringStream( "application/json" );

        // Render response content
        try{ 
            pjs::Stringifier::stringify( jsRsp, ostr, 1 ); 
        } catch( ... ) {
            std::cout << "ERROR: Exception while serializing comment" << std::endl;
        }

        // Set the HTTP response code
        setResponseCode( HNR_HTTP_OK );
    }
    else if( "putActivePrinter" == getOpID() )
    {
        if( printMgr->setActivePrinterByID( m_newID ) == HNLP_PM_RESULT_SUCCESS )
        {
            setConfigChange( true );

            // Set the HTTP response code
            setResponseCode( HNR_HTTP_OK );
        }
        else
        {
            // Set the HTTP response code
            setResponseCode( HNR_HTTP_NOT_FOUND );
        }
    }
    else if( "clearActivePrinter" == getOpID() )
    {
        printMgr->clearActivePrinter();
        setConfigChange( true );
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
    else if( "clearActivePrinter" == opID )
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