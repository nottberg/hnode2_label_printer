#include <unistd.h>
#include <string.h>
#include <syslog.h>

#include <iostream>
#include <sstream>
#include <thread>

#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Checksum.h"
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/StreamCopier.h>
#include "Poco/DirectoryIterator.h"
#include "Poco/FileStream.h"

#include <hnode2/HNodeDevice.h>

#include "HNLPLabelRender.h"
#include "HNLabelPrinterDevicePrivate.h"

using namespace Poco::Util;

namespace pjs = Poco::JSON;
namespace pdy = Poco::Dynamic;

// Forward declaration
extern const std::string g_HNode2TestRest;

void 
HNLabelPrinterDevice::defineOptions( OptionSet& options )
{
    ServerApplication::defineOptions( options );

    options.addOption(
              Option("help", "h", "display help").required(false).repeatable(false));

    options.addOption(
              Option("debug","d", "Enable debug logging").required(false).repeatable(false));

    options.addOption(
              Option("instance", "", "Specify the instance name of this daemon.").required(false).repeatable(false).argument("name"));

}

void 
HNLabelPrinterDevice::handleOption( const std::string& name, const std::string& value )
{
    ServerApplication::handleOption( name, value );
    if( "help" == name )
        _helpRequested = true;
    else if( "debug" == name )
        _debugLogging = true;
    else if( "instance" == name )
    {
         _instancePresent = true;
         _instance = value;
    }
}

void 
HNLabelPrinterDevice::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("[options]");
    helpFormatter.setHeader("HNode2 Switch Daemon.");
    helpFormatter.format(std::cout);
}

int 
HNLabelPrinterDevice::main( const std::vector<std::string>& args )
{
    m_instanceName = "default";
    if( _instancePresent == true )
        m_instanceName = _instance;

    // Initialize the capture queue
    m_curUserAction = NULL;
    m_userActionQueue.init();

    m_hnodeDev.setDeviceType( HNODE_LABEL_PRINTER_DEVTYPE );
    m_hnodeDev.setInstance( m_instanceName );

    HNDEndpoint hndEP;

    hndEP.setDispatch( "hnode2LabelPrinter", this );
    hndEP.setOpenAPIJson( g_HNode2TestRest ); 

    m_hnodeDev.addEndpoint( hndEP );

    m_hnodeDev.setRestPort(8088);

    m_printerMgr.init();

    std::cout << "Looking for config file" << std::endl;
    
    if( configExists() == false )
    {
        initConfig();
    }

    readConfig();

    // Register some format strings
    m_hnodeDev.registerFormatString( "Error: %u", m_errStrCode );
    m_hnodeDev.registerFormatString( "This is a test note.", m_noteStrCode );

    // Setup the event loop
    m_testDeviceEvLoop.setup( this );

    m_testDeviceEvLoop.setupTriggerFD( m_configUpdateTrigger );

    // Hook the local action queue into the event loop
    if( m_testDeviceEvLoop.addFDToEPoll( m_userActionQueue.getEventFD() ) != HNEP_RESULT_SUCCESS )
    {
        // Failed to add client socket.
        std::cerr << "ERROR: Failed to add local capture queue to event loop." << std::endl;
        return Application::EXIT_SOFTWARE;
    }

#if 0
    // Enable the health monitoring and add some fake components
    m_hnodeDev.enableHealthMonitoring();

    m_hnodeDev.getHealthRef().registerComponent( "test device hc1", HNDH_ROOT_COMPID, m_hc1ID );
    std::cout << "Health Component 1 id: " << m_hc1ID << std::endl;
    m_hnodeDev.getHealthRef().registerComponent( "test device hc2", HNDH_ROOT_COMPID, m_hc2ID );
    std::cout << "Health Component 2 id: " << m_hc2ID << std::endl;
    m_hnodeDev.getHealthRef().registerComponent( "test device hc2.1", m_hc2ID, m_hc3ID );
    std::cout << "Health Component 3 id: " << m_hc3ID << std::endl;

    m_healthStateSeq = 0;
    generateNewHealthState();
#endif

    // Start accepting device notifications
    m_hnodeDev.setNotifySink( this );

    // Load the label definition files.
    loadLabelDefinitionsFromLibrary();

    // Start up the hnode device
    m_hnodeDev.start();

    // Start event processing loop
    m_testDeviceEvLoop.run();

    waitForTerminationRequest();

    return Application::EXIT_OK;
}

void
HNLabelPrinterDevice::generateNewHealthState()
{
    m_hnodeDev.getHealthRef().startUpdateCycle( time(NULL) );

    switch( m_healthStateSeq )
    {
        case 0:
          m_hnodeDev.getHealthRef().setComponentStatus( HNDH_ROOT_COMPID, HNDH_CSTAT_OK );
          m_hnodeDev.getHealthRef().setComponentStatus( m_hc1ID, HNDH_CSTAT_OK );
          m_hnodeDev.getHealthRef().setComponentStatus( m_hc2ID, HNDH_CSTAT_OK );
          m_hnodeDev.getHealthRef().setComponentStatus( m_hc3ID, HNDH_CSTAT_OK );
        break;

        case 1:
          m_hnodeDev.getHealthRef().setComponentStatus( HNDH_ROOT_COMPID, HNDH_CSTAT_OK );

          m_hnodeDev.getHealthRef().setComponentStatus( m_hc1ID, HNDH_CSTAT_FAILED );
          m_hnodeDev.getHealthRef().setComponentErrMsg( m_hc1ID, 200, m_errStrCode, 200 );
          
          m_hnodeDev.getHealthRef().setComponentStatus( m_hc2ID, HNDH_CSTAT_OK );
          m_hnodeDev.getHealthRef().setComponentStatus( m_hc3ID, HNDH_CSTAT_OK );
        break;

        case 2:
          m_hnodeDev.getHealthRef().setComponentStatus( HNDH_ROOT_COMPID, HNDH_CSTAT_OK );

          m_hnodeDev.getHealthRef().setComponentStatus( m_hc1ID, HNDH_CSTAT_OK );
          m_hnodeDev.getHealthRef().clearComponentErrMsg( m_hc1ID );

          m_hnodeDev.getHealthRef().setComponentStatus( m_hc2ID, HNDH_CSTAT_OK );

          m_hnodeDev.getHealthRef().setComponentStatus( m_hc3ID, HNDH_CSTAT_FAILED );
          m_hnodeDev.getHealthRef().setComponentErrMsg( m_hc3ID, 400, m_errStrCode, 400 );
        break;

        case 3:
          m_hnodeDev.getHealthRef().setComponentStatus( HNDH_ROOT_COMPID, HNDH_CSTAT_OK );
          m_hnodeDev.getHealthRef().setComponentStatus( m_hc1ID, HNDH_CSTAT_OK );

          m_hnodeDev.getHealthRef().setComponentStatus( m_hc2ID, HNDH_CSTAT_OK );
          m_hnodeDev.getHealthRef().setComponentNote( m_hc2ID, m_noteStrCode );

          m_hnodeDev.getHealthRef().setComponentStatus( m_hc3ID, HNDH_CSTAT_OK );
          m_hnodeDev.getHealthRef().clearComponentErrMsg( m_hc3ID );
        break;

        case 4:
          m_hnodeDev.getHealthRef().setComponentStatus( HNDH_ROOT_COMPID, HNDH_CSTAT_OK );
          m_hnodeDev.getHealthRef().setComponentStatus( m_hc1ID, HNDH_CSTAT_OK );
          m_hnodeDev.getHealthRef().setComponentStatus( m_hc2ID, HNDH_CSTAT_OK );
          m_hnodeDev.getHealthRef().clearComponentNote( m_hc2ID );
          m_hnodeDev.getHealthRef().setComponentStatus( m_hc3ID, HNDH_CSTAT_OK );
        break;
    }

    // Advance to next simulated health state.
    m_healthStateSeq += 1;
    if( m_healthStateSeq > 4 )
      m_healthStateSeq = 0;

    bool changed = m_hnodeDev.getHealthRef().completeUpdateCycle();
} 

bool 
HNLabelPrinterDevice::configExists()
{
    HNodeConfigFile cfgFile;

    return cfgFile.configExists( HNODE_LABEL_PRINTER_DEVTYPE, m_instanceName );
}

HNLPD_RESULT_T
HNLabelPrinterDevice::initConfig()
{
    HNodeConfigFile cfgFile;
    HNodeConfig     cfg;

    m_hnodeDev.initConfigSections( cfg );

    m_printerMgr.initConfigSection( cfg );

    cfg.debugPrint(2);
    
    std::cout << "Saving config..." << std::endl;
    if( cfgFile.saveConfig( HNODE_LABEL_PRINTER_DEVTYPE, m_instanceName, cfg ) != HNC_RESULT_SUCCESS )
    {
        std::cout << "ERROR: Could not save initial configuration." << std::endl;
        return HNLPD_RESULT_FAILURE;
    }

    return HNLPD_RESULT_SUCCESS;
}

HNLPD_RESULT_T
HNLabelPrinterDevice::readConfig()
{
    HNodeConfigFile cfgFile;
    HNodeConfig     cfg;

    if( configExists() == false )
        return HNLPD_RESULT_FAILURE;

    std::cout << "Loading config..." << std::endl;

    if( cfgFile.loadConfig( HNODE_LABEL_PRINTER_DEVTYPE, m_instanceName, cfg ) != HNC_RESULT_SUCCESS )
    {
        std::cout << "ERROR: Could not load saved configuration." << std::endl;
        return HNLPD_RESULT_FAILURE;
    }
  
    std::cout << "cl1" << std::endl;
    m_hnodeDev.readConfigSections( cfg );

    m_printerMgr.readConfigSection( cfg );

    std::cout << "Config loaded" << std::endl;

    return HNLPD_RESULT_SUCCESS;
}

HNLPD_RESULT_T
HNLabelPrinterDevice::updateConfig()
{
    HNodeConfigFile cfgFile;
    HNodeConfig     cfg;

    m_hnodeDev.updateConfigSections( cfg );

    m_printerMgr.updateConfigSection( cfg );

    cfg.debugPrint(2);
    
    std::cout << "Saving config..." << std::endl;
    if( cfgFile.saveConfig( HNODE_LABEL_PRINTER_DEVTYPE, m_instanceName, cfg ) != HNC_RESULT_SUCCESS )
    {
        std::cout << "ERROR: Could not save configuration." << std::endl;
        return HNLPD_RESULT_FAILURE;
    }
    std::cout << "Config saved" << std::endl;

    return HNLPD_RESULT_SUCCESS;
}

HNLPD_RESULT_T 
HNLabelPrinterDevice::loadLabelDefinitionsFromLibrary()
{
    m_specMgr.resetDefinitions();
    m_layoutMgr.resetDefinitions();

    // Scan through library directory
    std::string directoryPath = "/etc/hnode2/hnode2-label-printer-device/definitions.d"; 

    try {
        Poco::DirectoryIterator it(directoryPath);
        Poco::DirectoryIterator end;

        while (it != end) {
          Poco::JSON::Parser parser;
          Poco::JSON::Object::Ptr rootObj;

          try {
            Poco::File file(*it);
            std::cout << "Loading definition file: "<< file.path() << std::endl;

            Poco::FileInputStream ifs( file.path() );

            Poco::Dynamic::Var result = parser.parse(ifs);
      
            
            if( typeid(rootObj) == result.type() ) {
              // Get the pointer to the root object in the file
              rootObj = result.extract<Poco::JSON::Object::Ptr>();

              // Process any defined label specifications
              Poco::JSON::Array::Ptr specArr = rootObj->getArray("LabelSpecifications");
              if( specArr )
              {
                std::cout << "Found Label Specifications Array" << std::endl;

                for( size_t i = 0; i < specArr->size(); ++i ) {
                  Poco::JSON::Object::Ptr childObj = specArr->getObject(i);

                  m_specMgr.defineSpecificationFromJSONObject( childObj );
                }     
              }

              // Process any defined layout specifications
              Poco::JSON::Array::Ptr layoutArr = rootObj->getArray("LabelLayouts");
              if( layoutArr )
              {
                std::cout << "Found Label Layouts Array" << std::endl;

                for( size_t i = 0; i < layoutArr->size(); ++i ) {
                  Poco::JSON::Object::Ptr childObj = layoutArr->getObject(i);

                  m_layoutMgr.defineLayoutFromJSONObject( childObj );
                }

              }

            }
      
          } catch (Poco::FileException& e) {
            std::cerr << "Error: Could not read file - skipping: " << e.what() << std::endl;
          }

          // Next file
          it++;
        }

    } catch (Poco::FileNotFoundException& e) {
        std::cerr << "Error: Directory not found: " << e.what() << std::endl;
        return HNLPD_RESULT_FAILURE;
    } catch (Poco::Exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return HNLPD_RESULT_FAILURE;
    }

    m_specMgr.debugPrint();
    m_layoutMgr.debugPrint();
    
    return HNLPD_RESULT_SUCCESS;
}

void
HNLabelPrinterDevice::loopIteration()
{
    // std::cout << "HNManagementDevice::loopIteration() - entry" << std::endl;

}

void
HNLabelPrinterDevice::timeoutEvent()
{
    // std::cout << "HNManagementDevice::timeoutEvent() - entry" << std::endl;

}

void
HNLabelPrinterDevice::fdEvent( int sfd )
{
    std::cout << "HNManagementDevice::fdEvent() - entry: " << sfd << std::endl;

    if( m_configUpdateTrigger.isMatch( sfd ) )
    {
        std::cout << "m_configUpdateTrigger - updating config" << std::endl;
        m_configUpdateTrigger.reset();
        updateConfig();
    }
    else if( sfd == m_userActionQueue.getEventFD() )
    {
        // Start the new action
        startAction();
    }

}

void
HNLabelPrinterDevice::fdError( int sfd )
{
    std::cout << "HNManagementDevice::fdError() - entry: " << sfd << std::endl;

}

void
HNLabelPrinterDevice::hndnConfigChange( HNodeDevice *parent )
{
    std::cout << "HNManagementDevice::hndnConfigChange() - entry" << std::endl;
    m_configUpdateTrigger.trigger();
}

void
HNLabelPrinterDevice::startAction()
{
    //HNSWDPacketClient packet;
    HNID_ACTBIT_T  actBits = HNID_ACTBIT_CLEAR;

    // Verify that we are in a state that will allow actions to start
//    if( getState() != HNID_STATE_READY )
//    {
//        return;
//    }

    // Pop the action from the queue
    m_curUserAction = ( HNLPAction* ) m_userActionQueue.aquireRecord();

    std::cout << "Action aquired - type: " << m_curUserAction->getType()  << "  thread: " << std::this_thread::get_id() << std::endl;

    switch( m_curUserAction->getType() )
    {
        case HNLP_AR_TYPE_DEVICE_STATUS:
        {
            HNLPStatusAction *statAction = (HNLPStatusAction *) m_curUserAction;

            // Generate response data for return to requestor
            statAction->generateRspContent();

            if( statAction->hasConfigChange() )
              actBits = (HNID_ACTBIT_T)( actBits | HNID_ACTBIT_UPDATE );

            // Done with this request
            actBits = (HNID_ACTBIT_T)( actBits | HNID_ACTBIT_COMPLETE );
        }
        break;

        case HNLP_AR_TYPE_DEVICE_PRINTER:
        {
            HNLPPrinterAction *printerAction = (HNLPPrinterAction *) m_curUserAction;

            // Generate response data for return to requestor
            printerAction->generateRspContent( &m_printerMgr );

            if( printerAction->hasConfigChange() )
              actBits = (HNID_ACTBIT_T)( actBits | HNID_ACTBIT_UPDATE );

            // Done with this request
            actBits = (HNID_ACTBIT_T)( actBits | HNID_ACTBIT_COMPLETE );
        }
        break;

        case HNLP_AR_TYPE_LABEL_SPEC:
        {
            HNLPSpecAction *specAction = (HNLPSpecAction *) m_curUserAction;

            // Generate response data for return to requestor
            specAction->generateRspContent( &m_specMgr );

            if( specAction->hasConfigChange() )
              actBits = (HNID_ACTBIT_T)( actBits | HNID_ACTBIT_UPDATE );

            // Done with this request
            actBits = (HNID_ACTBIT_T)( actBits | HNID_ACTBIT_COMPLETE );
        }
        break;

#if 0
        case HNSD_AR_TYPE_START_SINGLE_CAPTURE:
        {
            char idStr[64];

            // Create a new capture record.
            sprintf( idStr, "hwop%u", m_nextOpID );
            m_nextOpID += 1;
            
            HNSDHardwareOperation *newOp = new HNSDHardwareOperation( idStr, HNHW_OPTYPE_SINGLE_CAPTURE );

            CaptureRequest *crPtr = newOp->getCaptureRequestPtr();

            crPtr->setImageFormat( CS_STILLMODE_YUV420, 4624, 3472 );

            m_opMap.insert( std::pair< std::string, HNSDHardwareOperation* >( newOp->getID(), newOp ) );

            // Return the newly created capture id
            m_curUserAction->setNewID( newOp->getID() );

            // Kick off the capture thread
            m_hardwareCtrl.startOperation( newOp );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        }
        break;
#endif        
    }

    // The configuration was changed so commit
    // it to persistent copy
    if( actBits & HNID_ACTBIT_UPDATE )
    {
        // Commit config update
        updateConfig();
    }

    // There was an error, complete with error
    if( actBits & HNID_ACTBIT_ERROR )
    {
        std::cout << "Failing action: " << m_curUserAction->getType() << "  thread: " << std::this_thread::get_id() << std::endl;

        // Signal failure
        m_curUserAction->complete( false );
        m_curUserAction = NULL;
        return;
    }

    // Request has been completed successfully
    if( actBits & HNID_ACTBIT_COMPLETE )
    {
        std::cout << "Completing action: " << m_curUserAction->getType() << "  thread: " << std::this_thread::get_id() << std::endl;

        // Done with this request
        m_curUserAction->complete( true );
        m_curUserAction = NULL;
    }
    
}

void 
HNLabelPrinterDevice::dispatchEP( HNodeDevice *parent, HNOperationData *opData )
{
    std::cout << "HNLabelPrinterDevice::dispatchEP() - entry" << std::endl;
    std::cout << "  dispatchID: " << opData->getDispatchID() << std::endl;
    std::cout << "  opID: " << opData->getOpID() << std::endl;
    std::cout << "  thread: " << std::this_thread::get_id() << std::endl;

    std::string opID = opData->getOpID();

    if( "postPreviewRequest" == opID )
    {
        // Set response content type
        opData->responseSetChunkedTransferEncoding( true );
        opData->responseSetContentType( "image/png" );
      
        HNLPLabelRequest tmpRequest;
        HNLPLabelRender  tmpRender;

        //tmpRender.setPrintInfo( "w79h252", 89.0, 28.0 );
    
        //uint trID = tmpRender.createTextRegion();
    
        //tmpRender.setTextRegionContent( trID, "Line 1\nLine 2\nLine 3" );
        //tmpRender.setTextRegionDimensions( trID, 1.0, 1.0, 87.0, 26.0 );
        //tmpRender.setTextRegionFont( trID, "", 12.0, 32.0, true );

        HNLPLabelSpec *spec = m_specMgr.getSpec("ls0");

        if( spec == NULL )
        {
            std::cerr << "Invalid label spec id" << std::endl;
            return;
        }

        HNLPLabelLayout *layout = m_layoutMgr.getLayout("ll0");

        if( layout == NULL )
        {
            std::cerr << "Invalid label layout id" << std::endl;
            return;
        }

        // Generate a preview layout
        std::ostream& ostr = opData->responseSend();

        tmpRender.renderTemporaryPDF( spec, layout, &tmpRequest, "/tmp/tmp.pdf" );

        tmpRender.renderAlignmentPDF( spec, layout, &tmpRequest, "/tmp/tmp2.pdf" );

        tmpRender.renderPreviewToPNGStream( spec, layout, &tmpRequest, &ostr );    

        // Request was successful
        opData->responseSetStatusAndReason( HNR_HTTP_OK );

        return;
    }    


    // Allocate a handler action
    HNLPAction *action = HNLPActionFactory::allocateAction( opID );

    if( action == NULL )
    {
        // Send back not implemented
        opData->responseSetStatusAndReason( HNR_HTTP_NOT_IMPLEMENTED );
        opData->responseSend();
        return;
    }

    // Decode the request
    uint rspCode;
    HNLP_AR_RESULT_T result = action->decodeRequest( opData, rspCode );

    // Take action based on the decode
    if( result == HNLP_AR_RESULT_REQUEST_QUEUE )
    {
        std::cout << "Start Action - client: " << action->getType() << "  thread: " << std::this_thread::get_id() << std::endl;

        // Submit the action and block for response
        m_userActionQueue.postAndWait( action );

        std::cout << "Finish Action - client" << "  thread: " << std::this_thread::get_id() << std::endl;

        // Get the status after execution
        result = action->getExecResult( rspCode );
    }

    switch( result )
    {
        case HNLP_AR_RESULT_REQUEST_REJECT:
            opData->responseSetStatusAndReason( rspCode );
        break;

        case HNLP_AR_RESULT_REQUEST_COMPLETE:
        {
            std::string cType;
            std::string objID;

            // See if response content should be generated
            if( action->hasRspContent() )
            {
                // Render any response content
                std::ostream& ostr = opData->responseSend();
            
                if( action->outputRspContent( ostr, cType ) == true )
                {
                    opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
                    opData->responseSend();
                    return;
                }

                // Set response content type
                opData->responseSetChunkedTransferEncoding( true );
                opData->responseSetContentType( cType );
            }

            // Check if a new object was created.
            if( action->hasNewObject( objID ) )
            {
                // Object was created return info
                opData->responseSetCreated( objID );
                opData->responseSetStatusAndReason( HNR_HTTP_CREATED );
            }
            else
            {
                // Request was successful
                opData->responseSetStatusAndReason( HNR_HTTP_OK );
            }
        }
        break;

        default:
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
        break;
    }

    // Free the action
    HNLPActionFactory::deallocateAction( action );

    // Return to caller
    opData->responseSend();

#if 0
    // GET "/hnode2/test/status"
    if( "getStatus" == opID )
    {
        std::cout << "=== Get Status Request ===" << std::endl;
    
        // Set response content type
        opData->responseSetChunkedTransferEncoding( true );
        opData->responseSetContentType( "application/json" );

        // Create a json status object
        pjs::Object jsRoot;
        jsRoot.set( "overallStatus", "OK" );

        // Render response content
        std::ostream& ostr = opData->responseSend();
        try{ 
            pjs::Stringifier::stringify( jsRoot, ostr, 1 ); 
        } catch( ... ) {
            std::cout << "ERROR: Exception while serializing comment" << std::endl;
        }

        // Request was successful
        opData->responseSetStatusAndReason( HNR_HTTP_OK );
    }
    // GET "/hnode2/"
    else if( "getPrinterList" == opID )
    {

    }
    // GET "/hnode2/"
    else if( "getActivePrinterInfo" == opID )
    {

    }
    // GET "/hnode2/"
    else if( "putActivePrinter" == opID )
    {

    }
    // GET "/hnode2/"
    else if( "getLabelSpecifications" == opID )
    {

    }
    // POST "/hnode2/"
    else if( "createLabelSpecification" == opID )
    {

    }
    // DELETE "/hnode2/"
    else if( "deleteLabelSpecification" == opID )
    {

    }
    // GET "/hnode2/"
    else if( "getLabelLayouts" == opID )
    {

    }
    // PUT "/hnode2/"
    else if( "putLabelTemplate" == opID )
    {

    }
    // DELETE "/hnode2/"
    else if( "deleteLabelTemplate" == opID )
    {

    }
    // POST "/hnode2/"
    else if( "postPreviewRequest" == opID )
    {
        // Generate a preview layout

    }    
    // GET "/hnode2/"
    else if( "getPrintJobs" == opID )
    {

    }
    // POST "/hnode2/"
    else if( "postPrintRequest" == opID )
    {

    }
    // DELETE "/hnode2/"
    else if( "deletePrintJob" == opID )
    {

    }

    // GET "/hnode2/test/widgets"
    else if( "getWidgetList" == opID )
    {
        std::cout << "=== Get Widget List Request ===" << std::endl;

        // Set response content type
        opData->responseSetChunkedTransferEncoding( true );
        opData->responseSetContentType( "application/json" );

        // Create a json root object
        pjs::Array jsRoot;

        pjs::Object w1Obj;
        w1Obj.set( "id", "w1" );
        w1Obj.set( "color", "red" );
        jsRoot.add( w1Obj );

        pjs::Object w2Obj;
        w2Obj.set( "id", "w2" );
        w2Obj.set( "color", "green" );
        jsRoot.add( w2Obj );

        pjs::Object w3Obj;
        w3Obj.set( "id", "w3" );
        w3Obj.set( "color", "blue" );
        jsRoot.add( w3Obj );
          
        // Render response content
        std::ostream& ostr = opData->responseSend();
        try{ 
            pjs::Stringifier::stringify( jsRoot, ostr, 1 ); 
        } catch( ... ) {
            std::cout << "ERROR: Exception while serializing comment" << std::endl;
        }
            
        // Request was successful
        opData->responseSetStatusAndReason( HNR_HTTP_OK );
    }
    // GET "/hnode2/test/widgets/{widgetid}"
    else if( "getWidgetInfo" == opID )
    {
        std::string widgetID;

        if( opData->getParam( "widgetid", widgetID ) == true )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return; 
        }

        std::cout << "=== Get Widget Info Request (id: " << widgetID << ") ===" << std::endl;

        // Set response content type
        opData->responseSetChunkedTransferEncoding( true );
        opData->responseSetContentType( "application/json" );
        
        // Create a json root object
        pjs::Array jsRoot;

        pjs::Object w1Obj;
        w1Obj.set( "id", widgetID );
        w1Obj.set( "color", "black" );
        jsRoot.add( w1Obj );
          
        // Render response content
        std::ostream& ostr = opData->responseSend();
        try{ 
            pjs::Stringifier::stringify( jsRoot, ostr, 1 ); 
        } catch( ... ) {
            std::cout << "ERROR: Exception while serializing comment" << std::endl;
        }            
        // Request was successful
        opData->responseSetStatusAndReason( HNR_HTTP_OK );

    }
    // POST "/hnode2/test/widgets"
    else if( "createWidget" == opID )
    {
        std::istream& rs = opData->requestBody();
        std::string body;
        Poco::StreamCopier::copyToString( rs, body );
        
        std::cout << "=== Create Widget Post Data ===" << std::endl;
        std::cout << body << std::endl;

        // Object was created return info
        opData->responseSetCreated( "w1" );
        opData->responseSetStatusAndReason( HNR_HTTP_CREATED );
    }
    // PUT "/hnode2/test/widgets/{widgetid}"
    else if( "updateWidget" == opID )
    {
        std::string widgetID;

        // Make sure zoneid was provided
        if( opData->getParam( "widgetid", widgetID ) == true )
        {
            // widgetid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }
        
        std::istream& rs = opData->requestBody();
        std::string body;
        Poco::StreamCopier::copyToString( rs, body );
        
        std::cout << "=== Update Widget Put Data (id: " << widgetID << ") ===" << std::endl;
        std::cout << body << std::endl;

        // Request was successful
        opData->responseSetStatusAndReason( HNR_HTTP_OK );
    }    
    // DELETE "/hnode2/test/widgets/{widgetid}"
    else if( "deleteWidget" == opID )
    {
        std::string widgetID;

        // Make sure zoneid was provided
        if( opData->getParam( "widgetid", widgetID ) == true )
        {
            // widgetid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        std::cout << "=== Delete Widget Request (id: " << widgetID << ") ===" << std::endl;

        // Request was successful
        opData->responseSetStatusAndReason( HNR_HTTP_OK );
    }
    // PUT "/hnode2/test/health"
    else if( "putTestHealth" == opID )
    {
        // Get access to payload
        std::istream& rs = opData->requestBody();

        // Parse the json body of the request
        try
        {
            std::string component = HNDH_ROOT_COMPID;
            std::string status = "OK";
            uint errCode = 200;

            // Attempt to parse the json
            pjs::Parser parser;
            pdy::Var varRoot = parser.parse( rs );

            // Get a pointer to the root object
            pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

            if( jsRoot->has( "component" ) )
                component = jsRoot->getValue<std::string>( "component" );

            if( jsRoot->has( "status" ) )
                status = jsRoot->getValue<std::string>( "status" );

            if( jsRoot->has( "errCode" ) )
                errCode = jsRoot->getValue<uint>( "errCode" );

            m_hnodeDev.getHealthRef().startUpdateCycle( time(NULL) );

            if( status == "OK" )
            {
                m_hnodeDev.getHealthRef().setComponentStatus( component, HNDH_CSTAT_OK );
                m_hnodeDev.getHealthRef().clearComponentErrMsg( component );
                m_hnodeDev.getHealthRef().clearComponentNote( component );
            }
            else if( status == "UNKNOWN" )
            {
                m_hnodeDev.getHealthRef().setComponentStatus( component, HNDH_CSTAT_UNKNOWN );
                m_hnodeDev.getHealthRef().clearComponentErrMsg( component );
            }
            else if( status == "FAILED" )
            {
                m_hnodeDev.getHealthRef().setComponentStatus( component, HNDH_CSTAT_FAILED );
                m_hnodeDev.getHealthRef().setComponentErrMsg( component, errCode, m_errStrCode, errCode );
            }
            else if( status == "NOTE" )
            {
                m_hnodeDev.getHealthRef().setComponentNote( component, m_noteStrCode );
            }

            m_hnodeDev.getHealthRef().completeUpdateCycle();

        }
        catch( Poco::Exception ex )
        {
            std::cout << "putTestHealth exception: " << ex.displayText() << std::endl;
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return;
        }

        // Request was successful
        opData->responseSetStatusAndReason( HNR_HTTP_OK );
    }        
    else
    {
        // Send back not implemented
        opData->responseSetStatusAndReason( HNR_HTTP_NOT_IMPLEMENTED );
        opData->responseSend();
        return;
    }

    // Return to caller
    opData->responseSend();
  #endif
}

const std::string g_HNode2TestRest = R"(
{
  "openapi": "3.0.0",
  "info": {
    "description": "",
    "version": "1.0.0",
    "title": ""
  },
  "paths": {
      "/hnode2/label-printer/status": {
        "get": {
          "summary": "Get overall status of label-printer device.",
          "operationId": "getStatus",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "array"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        }
      },

      "/hnode2/label-printer/printers": {
        "get": {
          "summary": "Return a list of available printers.",
          "operationId": "getPrinterList",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        }

      },

      "/hnode2/label-printer/printers/active": {
        "get": {
          "summary": "Get information about the active printer that requests will be submitted too.",
          "operationId": "getActivePrinterInfo",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },

        "put": {
          "summary": "Set and configure active printer parameters",
          "operationId": "putActivePrinter",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "array"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },

        "delete": {
          "summary": "Clear the active printer setting.",
          "operationId": "clearActivePrinter",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "array"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        }        
      },

      "/hnode2/label-printer/label-specifications": {
        "get": {
          "summary": "Get information about the defined label specifications",
          "operationId": "getLabelSpecifications",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },

        "post": {
          "summary": "Add a new label specification",
          "operationId": "createLabelSpecification",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "array"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },

        "delete": {
          "summary": "Delete a label specification",
          "operationId": "deleteLabelSpecification",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "array"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        }

      },

      "/hnode2/label-printer/label-layout": {
        "get": {
          "summary": "Get information about defined label layouts",
          "operationId": "getLabelLayouts",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },

        "post": {
          "summary": "Add a new label layout",
          "operationId": "putLabelTemplate",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "array"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },

        "delete": {
          "summary": "Delete a label layout",
          "operationId": "deleteLabelTemplate",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "array"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        }

      },

      "/hnode2/label-printer/preview": {
        "post": {
          "summary": "Request a preview layout",
          "operationId": "postPreviewRequest",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "array"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        }
      },

      "/hnode2/label-printer/jobs": {
        "get": {
          "summary": "Get information about active print jobs",
          "operationId": "getPrintJobs",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },

        "post": {
          "summary": "Submit a new print request",
          "operationId": "postPrintRequest",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "array"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },

        "delete": {
          "summary": "Cancel a print job",
          "operationId": "deletePrintJob",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "array"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        }

      }      
    }
}
)";

