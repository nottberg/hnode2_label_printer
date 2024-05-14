#include <iostream>
#include <cstdio>

#include "HNLPPrinterManager.h"

HNLPPrinter::HNLPPrinter()
{

}

HNLPPrinter::~HNLPPrinter()
{

}

void
HNLPPrinter::setID( std::string value )
{
    m_id = value;
}

std::string
HNLPPrinter::getID()
{
    return m_id;
}

void
HNLPPrinter::setModel( std::string value )
{
    m_model = value;
}

std::string
HNLPPrinter::getModel()
{
    return m_model;
}

void
HNLPPrinter::setInfo( std::string value )
{
    m_info = value;
}

std::string
HNLPPrinter::getInfo()
{
    return m_info;
}

void
HNLPPrinter::setLocation( std::string value )
{
    m_location = value;
}

std::string
HNLPPrinter::getLocation()
{
    return m_location;
}

void
HNLPPrinter::setURI( std::string value )
{
    m_uri = value;
}

std::string
HNLPPrinter::getURI()
{
    return m_uri;
}

void
HNLPPrinter::setAccepting( bool value )
{
    m_accepting = value;
}

bool
HNLPPrinter::isAccepting()
{
    return m_accepting;
}

void
HNLPPrinter::setShared( bool value )
{
    m_shared = value;
}

bool
HNLPPrinter::isShared()
{
    return m_shared;
}

HNLPPrinterManager::HNLPPrinterManager()
{

}

HNLPPrinterManager::~HNLPPrinterManager()
{

}

int 
HNLPPrinterManager::printerInfoCallback( void *user_data, unsigned flags, cups_dest_t *dest )
{
    HNLPPrinterManager *prtMgr = (HNLPPrinterManager *) user_data;
    cups_size_t size;
    int i;
    char idBuf[512];

    if( dest->instance )
      sprintf( idBuf,"%s/%s", dest->name, dest->instance );
    else
      sprintf( idBuf, "%s", dest->name );

    HNLPPrinter *printer = prtMgr->getOrCreatePrinter( idBuf );

    const char *model = cupsGetOption( "printer-make-and-model", dest->num_options, dest->options );
    printer->setModel( model );

    const char *pinfo = cupsGetOption( "printer-info", dest->num_options, dest->options );
    printer->setInfo( pinfo );

    const char *accept = cupsGetOption( "printer-is-accepting-jobs", dest->num_options, dest->options );
    if( strcmp( "true", accept ) == 0 )
        printer->setAccepting( true );
    else
        printer->setAccepting( false );

    const char *shared = cupsGetOption( "printer-is-shared", dest->num_options, dest->options );
    if( strcmp( "true", accept ) == 0 )
        printer->setShared( true );
    else
        printer->setShared( false );

    const char *location = cupsGetOption( "printer-location", dest->num_options, dest->options );
    printer->setLocation( location );

    const char *puri = cupsGetOption( "printer-uri-supported", dest->num_options, dest->options );
    printer->setURI( puri );

    return 1;
}

void
HNLPPrinterManager::executePrinterListRefresh()
{
    cupsEnumDests( CUPS_DEST_FLAGS_NONE, 
                    1000, 
                    NULL, 
                    CUPS_PRINTER_LOCAL | CUPS_PRINTER_VARIABLE, CUPS_PRINTER_DISCOVERED | CUPS_PRINTER_VARIABLE, 
                    HNLPPrinterManager::printerInfoCallback, 
                    this );

    std::cout << "Found " << m_printerList.size() << " printers" << std::endl;
}

HNLP_PM_RESULT_T
HNLPPrinterManager::init()
{
    executePrinterListRefresh();

    return HNLP_PM_RESULT_SUCCESS;
}

HNLP_PM_RESULT_T
HNLPPrinterManager::getAvailablePrinterList( std::vector< HNLPPrinter* > &list )
{
    for( std::map< std::string, HNLPPrinter* >::iterator mip = m_printerList.begin(); mip != m_printerList.end(); mip++ )
    {
        list.push_back( mip->second );
    }

    return HNLP_PM_RESULT_SUCCESS;
}

HNLPPrinter*
HNLPPrinterManager::getOrCreatePrinter( std::string idBuf )
{
    std::map< std::string, HNLPPrinter* >::iterator mip = m_printerList.find( idBuf );

    if( mip == m_printerList.end() )
    {
        HNLPPrinter *newPtr = new HNLPPrinter;
        newPtr->setID( idBuf );
        m_printerList.insert( std::pair< std::string, HNLPPrinter* >( idBuf, newPtr ) );
        return newPtr;
    }

    return mip->second;
}
