#include <iostream>
#include <cstdio>
#include <mutex>

#include "HNLPLabelLayoutManager.h"

HNLPLabelContentArea::HNLPLabelContentArea()
{

}

HNLPLabelContentArea::~HNLPLabelContentArea()
{

}

void
HNLPLabelContentArea::setID( std::string value )
{
    m_ID = value;
}

std::string
HNLPLabelContentArea::getID()
{
    return m_ID;
}

HNLPLabelTextContent::HNLPLabelTextContent()
{
    m_bbWidthInset   = 0.0;
    m_bbLengthInset  = 0.0;
    m_bbWidth        = 0.0;
    m_bbLength       = 0.0;
    m_bbOrientation  = 0.0;

    m_tpFontSize = 0;
}

HNLPLabelTextContent::~HNLPLabelTextContent()
{

}

HNLP_LLCA_TYPE_T 
HNLPLabelTextContent::getType()
{
    return HNLP_LLCA_TYPE_TEXTBOX;
}

/*
    {
        "areaID":"text1",
        "areaType":"textbox",
        "boundingBox":{
            "widthInset":2,
            "lengthInset":2,
            "width":24,
            "length":85,
            "orientation":0
        },
        "textProperties":{
            "font":"Sans Bold 27",
            "font-size":36
        }
    }
*/
HNLP_LL_RESULT_T
HNLPLabelTextContent::initFromJSONObject( Poco::JSON::Object::Ptr defObj )
{
    Poco::JSON::Object::Ptr bbObj;

    Poco::Dynamic::Var tmpVar;

    if( defObj->has("boundingBox") == false )
    {
        std::cout << "In a textbox content area, a boundingBox suboject is required." << std::endl;
        return HNLP_LL_RESULT_FAILURE;
    }

    bbObj = defObj->getObject("boundingBox");

    /*
    tmpVar = defObj->get("width");
    if( tmpVar.isEmpty() == false )
    {
        //m_width = tmpVar.convert<double>();
    }

    tmpVar = defObj->get("length");
    if( tmpVar.isEmpty() == false )
    {
        //m_length = tmpVar.convert<double>();
    }
    */

    return HNLP_LL_RESULT_SUCCESS;
}

HNLP_LL_RESULT_T
HNLPLabelTextContent::initBoundingBoxFromJSONObject( Poco::JSON::Object::Ptr bbObj )
{

    return HNLP_LL_RESULT_SUCCESS;
}

HNLP_LL_RESULT_T
HNLPLabelTextContent::initTextParametersFromJSONObject( Poco::JSON::Object::Ptr tpObj )
{

    return HNLP_LL_RESULT_SUCCESS;
}

void
HNLPLabelTextContent::debugPrint()
{

}

HNLPLabelTarget::HNLPLabelTarget()
{

}

HNLPLabelTarget::~HNLPLabelTarget()
{

}

void
HNLPLabelTarget::setVendorName( std::string value )
{
    m_vendorName = value;
}

void
HNLPLabelTarget::setVendorLabelID( std::string value )
{
    m_vendorLabelID = value;
}

HNLPLabelLayout::HNLPLabelLayout()
{

}

HNLPLabelLayout::~HNLPLabelLayout()
{

}

/*
{
    "layoutID":"dymo-30252-centered-box",
    "layoutName":"Dymo 30252 Single Text",
    "layoutDescription":"A single text box in center of the label.",

    "targetLabels":[
        {
            "vendorName":"Dymo",
            "vendorLabelID":"30252"
        }
    ],

    "contentAreas":[
        {
            "areaID":"text1",
            "areaType":"textbox",
            "boundingBox":{
                "widthOffset":2,
                "lengthOffset":2,
                "width":24,
                "length":85,
                "orientation":0
            },
            "text-properties":{
                "font":"Sans Bold 27",
                "font-size":36
            }
        }
    ]
}
*/

HNLP_LL_RESULT_T
HNLPLabelLayout::initFromJSONObject( Poco::JSON::Object::Ptr defObj )
{
    Poco::Dynamic::Var tmpVar;

    tmpVar = defObj->get("layoutID");
    if( tmpVar.isEmpty() == false )
    {
        m_layoutID = tmpVar.convert<std::string>();
    }

    tmpVar = defObj->get("layoutName");
    if( tmpVar.isEmpty() == false )
    {
        m_layoutName = tmpVar.convert<std::string>();
    }

    tmpVar = defObj->get("layoutDescription");
    if( tmpVar.isEmpty() == false )
    {
        m_layoutDescription = tmpVar.convert<std::string>();
    }

    /*
    "targetLabels":[
    {
        "vendorName":"Dymo",
        "vendorLabelID":"30252"
    }
    ],
    */

    tmpVar = defObj->get("targetLabels");
    if( tmpVar.isEmpty() == false )
    {
        Poco::JSON::Array::Ptr tgtsArr = defObj->getArray("targetLabels");
        
        for( size_t i = 0; i < tgtsArr->size(); ++i ) {
            HNLPLabelTarget tgtLabel;

            Poco::JSON::Object::Ptr childObj = tgtsArr->getObject(i);

            tmpVar = defObj->get("vendorName");
            if( tmpVar.isEmpty() == false )
            {
                tgtLabel.setVendorName( tmpVar.convert<std::string>() );
            }

            tmpVar = defObj->get("vendorLabelID");
            if( tmpVar.isEmpty() == false )
            {
                tgtLabel.setVendorLabelID( tmpVar.convert<std::string>() );
            }
            
            m_tgtLabelList.push_back( tgtLabel );
        }
    }

/*
    "contentAreas":[
        {
            "areaID":"text1",
            "areaType":"textbox",
...
        }
    ]

*/
    tmpVar = defObj->get("contentAreas");
    if( tmpVar.isEmpty() == false )
    {
        Poco::JSON::Array::Ptr caArr = defObj->getArray("contentAreas");
        
        for( size_t i = 0; i < caArr->size(); ++i ) {
            HNLPLabelTextContent *cArea; 

            Poco::JSON::Object::Ptr childObj = caArr->getObject(i);

            tmpVar = defObj->get("areaType");
            if( tmpVar.isEmpty() == true )
                continue;
            
            std::string type = tmpVar.convert<std::string>();

            if( type == "textbox" )
            {
                cArea = (HNLPLabelTextContent *) new HNLPLabelTextContent();
            }
            else
            {
                std::cout << "Unsupportted content area type, skipping: " << type << std::endl;
                continue;
            }

            tmpVar = defObj->get("areaID");
            if( tmpVar.isEmpty() == true )
            {
                std::cout << "Content areas require a 'areaID' attribute." << std::endl;
                delete cArea;
                continue;
            }

            cArea->setID( tmpVar.convert<std::string>() );
            
            m_contentAreaList.push_back( cArea );
        }
    }

    //tmpVar = defObj->get("color");
    //if( tmpVar.isEmpty() == false )
    //{
    //    m_color = tmpVar.convert<std::string>();
    //}

    // A printable area definition is required
    //if( defObj->has("printableAreaBoundary") == false )
    //    return HNLP_LS_RESULT_FAILURE;

    // Check the name of the subobject to see how the
    // printable area is being defined.
    //Poco::JSON::Object::Ptr paObj = defObj->getObject("printableAreaBoundary");

    //if( paObj->has("square") )
    //{
    //    HNLPSquareBoundary *sqBound = new HNLPSquareBoundary;

    //    if( sqBound->initFromJSONObject( paObj->getObject("square") ) != HNLP_LS_RESULT_SUCCESS )
    //    {
    //        delete sqBound;
    //        return HNLP_LS_RESULT_FAILURE;
    //    }

    //    m_areaBoundary = sqBound;
    //}
    //else
    //{
        // Printable area definition method is not supported
    //    return HNLP_LS_RESULT_FAILURE;
    //}

    return HNLP_LL_RESULT_SUCCESS;
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

void
HNLPLabelLayout::debugPrint()
{
    std::cout << "  layoutID: " << m_layoutID << std::endl;
    std::cout << "  layoutName: " << m_layoutName << std::endl;
    std::cout << "  layoutDescription: " << m_layoutDescription << std::endl;
    /*
    std::cout << "  color: " << m_color << std::endl;

    if( m_areaBoundary != NULL )
    {
        m_areaBoundary->debugPrint();
    }
    */
}

HNLPLabelLayoutManager::HNLPLabelLayoutManager()
{
    m_nextLayoutIndex = 0;
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

std::string
HNLPLabelLayoutManager::getNextUniqueID()
{
    char tmpID[32];
    sprintf(tmpID, "ll%u", m_nextLayoutIndex );
    m_nextLayoutIndex += 1;
    return tmpID;
}

HNLP_LL_RESULT_T
HNLPLabelLayoutManager::defineLayoutFromJSONObject( Poco::JSON::Object::Ptr defObj )
{
    char tmpID[64];

    HNLPLabelLayout *newLayout = new HNLPLabelLayout;

    if( newLayout->initFromJSONObject(defObj) != HNLP_LL_RESULT_SUCCESS )
        return HNLP_LL_RESULT_FAILURE;

    newLayout->setID( getNextUniqueID() );

    m_layoutList.insert( std::pair< std::string, HNLPLabelLayout* >( newLayout->getID(), newLayout ) );

    return HNLP_LL_RESULT_SUCCESS;
}

void
HNLPLabelLayoutManager::debugPrint()
{
    std::cout << std::endl << "==== Label Layout Definitions ====" << std::endl;

    for( std::map<std::string, HNLPLabelLayout*>::iterator it = m_layoutList.begin(); it != m_layoutList.end(); it++ )
    {
        std::cout << "  === Layout Definition (id: " << it->second->getID() << " )===" << std::endl;
        it->second->debugPrint();
        std::cout << std::endl;
    }

    std::cout << "================================" << std::endl << std::endl;
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
