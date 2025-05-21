#include <iostream>
#include <cstdio>
#include <mutex>

#include "HNLPLabelLayoutManager.h"

HNLPLabelLayout::HNLPLabelLayout()
{

}

HNLPLabelLayout::~HNLPLabelLayout()
{

}

void
HNLPLabelLayout::setID( std::string value )
{
    m_id = value;
}

std::string
HNLPLabelLayout::getID()
{
    return m_id;
}

HNLPLabelLayoutManager::HNLPLabelLayoutManager()
{

}

HNLPLabelLayoutManager::~HNLPLabelLayoutManager()
{

}

HNLP_LL_RESULT_T
HNLPLabelLayoutManager::init()
{
    return HNLP_LL_RESULT_SUCCESS;
}

HNLP_LL_RESULT_T
HNLPLabelLayoutManager::initConfigSection( HNodeConfig &cfg )
{
    HNCSection *secPtr;

    // Scope lock
    //const std::lock_guard<std::mutex> lock(m_accessMutex);

    //cfg.updateSection( "printerManager", &secPtr );
    
    //if( m_activePrinter )
    //    secPtr->updateValue( "activePrinterID", m_activePrinter->getID() );

    return HNLP_LL_RESULT_SUCCESS;
}

HNLP_LL_RESULT_T
HNLPLabelLayoutManager::readConfigSection( HNodeConfig &cfg )
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

    return HNLP_LL_RESULT_SUCCESS;
}

HNLP_LL_RESULT_T
HNLPLabelLayoutManager::updateConfigSection( HNodeConfig &cfg )
{
    HNCSection *secPtr;

    // Scope lock
    //const std::lock_guard<std::mutex> lock(m_accessMutex);

    //cfg.updateSection( "printerManager", &secPtr );
    
    //if( m_activePrinter )
    //    secPtr->updateValue( "activePrinterID", m_activePrinter->getID() );

    return HNLP_LL_RESULT_SUCCESS;
}

HNLP_LL_RESULT_T
HNLPLabelLayoutManager::resetDefinitions()
{
    return HNLP_LL_RESULT_SUCCESS;
}

#if 0
HNLPLabelLayout*
HNLPLabelLayoutManager::getOrCreatePrinter( std::string idBuf )
{
    std::map< std::string, HNLPLabelLayout* >::iterator mip = m_printerList.find( idBuf );

    if( mip == m_printerList.end() )
    {
        HNLPLabelLayout *newPtr = new HNLPLabelLayout;
        newPtr->setID( idBuf );
        m_printerList.insert( std::pair< std::string, HNLPLabelLayout* >( idBuf, newPtr ) );
        return newPtr;
    }

    return mip->second;
}
#endif
