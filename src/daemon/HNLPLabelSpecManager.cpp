#include <iostream>
#include <cstdio>
#include <mutex>

#include "HNLPLabelSpecManager.h"

HNLPAreaBoundary::HNLPAreaBoundary()
{

}

HNLPAreaBoundary::~HNLPAreaBoundary()
{

}

HNLPSquareBoundary::HNLPSquareBoundary()
{

}

HNLPSquareBoundary::~HNLPSquareBoundary()
{

}

HNLP_AB_TYPE_T
HNLPSquareBoundary::getType()
{
    return HNLP_AB_TYPE_SQUARE;
}

/*
    "square":{
        "width":28,
        "length":89
    }
*/
HNLP_LS_RESULT_T
HNLPSquareBoundary::initFromJSONObject( Poco::JSON::Object::Ptr defObj )
{
    Poco::Dynamic::Var tmpVar;

    tmpVar = defObj->get("width");
    if( tmpVar.isEmpty() == false )
    {
        m_width = tmpVar.convert<double>();
    }

    tmpVar = defObj->get("length");
    if( tmpVar.isEmpty() == false )
    {
        m_length = tmpVar.convert<double>();
    }

    return HNLP_LS_RESULT_SUCCESS;
}

double
HNLPSquareBoundary::getWidth()
{
    return m_width;
}

double
HNLPSquareBoundary::getLength()
{
    return m_length;
}

void
HNLPSquareBoundary::debugPrint()
{
    std::cout << "  Boundary Type: square" << std::endl;
    std::cout << "     width: " << m_width << std::endl;
    std::cout << "    length: " << m_length << std::endl;
}

HNLPLabelSpec::HNLPLabelSpec()
{
    m_areaBoundary = NULL;
}

HNLPLabelSpec::~HNLPLabelSpec()
{

}

void
HNLPLabelSpec::clear()
{
    m_vendorName.clear();
    m_vendorLabelID.clear();
    m_vendorDescription.clear();
    m_color.clear();

    if( m_areaBoundary != NULL )
    {
        delete m_areaBoundary;
        m_areaBoundary = NULL;
    }

}

/*
{
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
HNLP_LS_RESULT_T
HNLPLabelSpec::initFromJSONObject( Poco::JSON::Object::Ptr defObj )
{
    Poco::Dynamic::Var tmpVar;
    
    tmpVar = defObj->get("vendorName");
    if( tmpVar.isEmpty() == false )
    {
        m_vendorName = tmpVar.convert<std::string>();
    }

    tmpVar = defObj->get("vendorLabelID");
    if( tmpVar.isEmpty() == false )
    {
        m_vendorLabelID = tmpVar.convert<std::string>();
    }

    tmpVar = defObj->get("vendorDescription");
    if( tmpVar.isEmpty() == false )
    {
        m_vendorDescription = tmpVar.convert<std::string>();
    }

    tmpVar = defObj->get("color");
    if( tmpVar.isEmpty() == false )
    {
        m_color = tmpVar.convert<std::string>();
    }

    // A printable area definition is required
    if( defObj->has("printableAreaBoundary") == false )
        return HNLP_LS_RESULT_FAILURE;

    // Check the name of the subobject to see how the
    // printable area is being defined.
    Poco::JSON::Object::Ptr paObj = defObj->getObject("printableAreaBoundary");

    if( paObj->has("square") )
    {
        HNLPSquareBoundary *sqBound = new HNLPSquareBoundary;

        if( sqBound->initFromJSONObject( paObj->getObject("square") ) != HNLP_LS_RESULT_SUCCESS )
        {
            delete sqBound;
            return HNLP_LS_RESULT_FAILURE;
        }

        m_areaBoundary = sqBound;
    }
    else
    {
        // Printable area definition method is not supported
        return HNLP_LS_RESULT_FAILURE;
    }

    return HNLP_LS_RESULT_SUCCESS;
}

void
HNLPLabelSpec::setID( std::string value )
{
   m_id = value;
}

std::string
HNLPLabelSpec::getID()
{
   return m_id;
}

void
HNLPLabelSpec::setDescription( std::string value )
{
    //m_desc = value;
}

std::string
HNLPLabelSpec::getDescription()
{
    //return m_desc;
}

void
HNLPLabelSpec::setVendor( std::string value )
{
    m_vendorName = value;
}

std::string
HNLPLabelSpec::getVendor()
{
    return m_vendorName;
}

void
HNLPLabelSpec::setVendorRefNum( std::string value )
{
    //m_vendorRefNum = value;
}

std::string
HNLPLabelSpec::getVendorRefNum()
{
    //return m_vendorRefNum;
}

HNLP_AB_TYPE_T 
HNLPLabelSpec::getBoundaryType()
{
    if( m_areaBoundary )
        return m_areaBoundary->getType();

    return HNLP_AB_TYPE_NOTSET;
}

bool
HNLPLabelSpec::isBoundary( HNLP_AB_TYPE_T type )
{
    if( m_areaBoundary && (m_areaBoundary->getType() == type) )
        return true;

    return false;
}

double
HNLPLabelSpec::getBoundaryWidth()
{
    if( m_areaBoundary )
    {
        switch( m_areaBoundary->getType() )
        {
            case HNLP_AB_TYPE_SQUARE:      
                return ((HNLPSquareBoundary *) m_areaBoundary)->getWidth();
            break;
        }
    }

    return 0;
}

double
HNLPLabelSpec::getBoundaryLength()
{
    if( m_areaBoundary )
    {
        switch( m_areaBoundary->getType() )
        {
            case HNLP_AB_TYPE_SQUARE:      
                return ((HNLPSquareBoundary *) m_areaBoundary)->getLength();
            break;
        }
    }

    return 0;
}

void
HNLPLabelSpec::debugPrint()
{
    std::cout << "  vendorName: " << m_vendorName << std::endl;
    std::cout << "  vendorLabelID: " << m_vendorLabelID << std::endl;
    std::cout << "  vendorDescription: " << m_vendorDescription << std::endl;
    std::cout << "  color: " << m_color << std::endl;

    if( m_areaBoundary != NULL )
    {
        m_areaBoundary->debugPrint();
    }
}

HNLPLabelSpecManager::HNLPLabelSpecManager()
{
    m_nextSpecIndex = 0;
}

HNLPLabelSpecManager::~HNLPLabelSpecManager()
{

}

HNLP_LS_RESULT_T
HNLPLabelSpecManager::init()
{
    return HNLP_LS_RESULT_SUCCESS;
}

HNLP_LS_RESULT_T
HNLPLabelSpecManager::initConfigSection( HNodeConfig &cfg )
{
    HNCSection *secPtr;

    // Scope lock
    //const std::lock_guard<std::mutex> lock(m_accessMutex);

    //cfg.updateSection( "printerManager", &secPtr );
    
    //if( m_activePrinter )
    //    secPtr->updateValue( "activePrinterID", m_activePrinter->getID() );

    return HNLP_LS_RESULT_SUCCESS;
}

HNLP_LS_RESULT_T
HNLPLabelSpecManager::readConfigSection( HNodeConfig &cfg )
{
    HNCSection  *secPtr;
    std::string printerID;

    std::cout << "rc1" << std::endl;

    // Scope lock
    //const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Aquire a pointer to the "printerManager" section
    //cfg.updateSection( "printerManager", &secPtr );

    //secPtr->getValueByName( "activePrinterID", printerID );

    //if( printerID.empty() ==  false )
    //    setActivePrinterByID( printerID );

    return HNLP_LS_RESULT_SUCCESS;
}

HNLP_LS_RESULT_T
HNLPLabelSpecManager::updateConfigSection( HNodeConfig &cfg )
{
    HNCSection *secPtr;

    // Scope lock
    //const std::lock_guard<std::mutex> lock(m_accessMutex);

    //cfg.updateSection( "printerManager", &secPtr );
    
    //if( m_activePrinter )
    //    secPtr->updateValue( "activePrinterID", m_activePrinter->getID() );

    return HNLP_LS_RESULT_SUCCESS;
}

HNLP_LS_RESULT_T
HNLPLabelSpecManager::getLabelSpecificationList( std::vector< HNLPLabelSpec* > &list )
{
    for( std::map< std::string, HNLPLabelSpec* >::iterator mip = m_specList.begin(); mip != m_specList.end(); mip++ )
    {
        list.push_back( mip->second );
    }

    return HNLP_LS_RESULT_SUCCESS;
}

HNLP_LS_RESULT_T
HNLPLabelSpecManager::resetDefinitions()
{
    return HNLP_LS_RESULT_SUCCESS;
}

std::string
HNLPLabelSpecManager::getNextUniqueID()
{
    char tmpID[32];
    sprintf(tmpID, "ls%u", m_nextSpecIndex );
    m_nextSpecIndex += 1;
    return tmpID;
}

HNLP_LS_RESULT_T
HNLPLabelSpecManager::defineSpecificationFromJSONObject( Poco::JSON::Object::Ptr defObj )
{
    HNLPLabelSpec *newSpec = new HNLPLabelSpec;

    if( newSpec->initFromJSONObject(defObj) != HNLP_LS_RESULT_SUCCESS )
        return HNLP_LS_RESULT_FAILURE;

    newSpec->setID( getNextUniqueID() );

    m_specList.insert( std::pair< std::string, HNLPLabelSpec* >( newSpec->getID(), newSpec ) );

    return HNLP_LS_RESULT_SUCCESS;
}

HNLPLabelSpec*
HNLPLabelSpecManager::getSpec( std::string id )
{
    std::map< std::string, HNLPLabelSpec* >::iterator it = m_specList.find(id);

    if( it == m_specList.end() )
        return NULL;

    return it->second;
}

void
HNLPLabelSpecManager::debugPrint()
{
    std::cout << std::endl << "==== Label Spec Definitions ====" << std::endl;

    for( std::map<std::string, HNLPLabelSpec*>::iterator it = m_specList.begin(); it != m_specList.end(); it++ )
    {
        std::cout << "  === Spec Definition (id: " << it->second->getID() << " )===" << std::endl;
        it->second->debugPrint();
        std::cout << std::endl;
    }

    std::cout << "================================" << std::endl << std::endl;
}

/*
HNLP_LS_RESULT_T
HNLPLabelSpecManager::createNewSpecification( HNLPLabelSpec **rtnPtr )
{
    char idStr[64];
    std::map< std::string, HNLPLabelSpec* >::iterator it;

    bool found = true;
    while( found == true )
    {
        sprintf( idStr, "ls%u", m_nextIDValue );

        it = m_specList.find( idStr );
        if( it == m_specList.end() )
            found = false;

        m_nextIDValue += 1;
    }

    HNLPLabelSpec *newPtr = new HNLPLabelSpec;
    newPtr->setID( idStr );
    m_specList.insert( std::pair< std::string, HNLPLabelSpec* >( idStr, newPtr ) );
    
    *rtnPtr = newPtr;

    return HNLP_LS_RESULT_SUCCESS;
}
*/

#if 0
HNLPPrinter*
HNLPLabelSpecManager::getOrCreatePrinter( std::string idBuf )
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
#endif
