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
...
        },
        "textProperties":{
...
        }
    }
*/
HNLP_LL_RESULT_T
HNLPLabelTextContent::initFromJSONObject( Poco::JSON::Object::Ptr defObj )
{
    Poco::JSON::Object::Ptr bbObj;
    Poco::JSON::Object::Ptr tpObj;

    if( defObj->has("boundingBox") == false )
    {
        std::cout << "In a textbox content area, a boundingBox suboject is required." << std::endl;
        return HNLP_LL_RESULT_FAILURE;
    }

    bbObj = defObj->getObject("boundingBox");
    initBoundingBoxFromJSONObject(bbObj);

    if( defObj->has("textProperties") == false )
    {
        std::cout << "In a textbox content area, a textProperties suboject is required." << std::endl;
        return HNLP_LL_RESULT_FAILURE;
    }

    tpObj = defObj->getObject("textProperties");
    initTextPropertiesFromJSONObject(tpObj);

    return HNLP_LL_RESULT_SUCCESS;
}

/*
"boundingBox":{
    "widthInset":2,
    "lengthInset":2,
    "width":24,
    "length":85,
    "orientation":0
},
*/
HNLP_LL_RESULT_T
HNLPLabelTextContent::initBoundingBoxFromJSONObject( Poco::JSON::Object::Ptr bbObj )
{
    Poco::Dynamic::Var tmpVar;

    tmpVar = bbObj->get("widthInset");
    if( tmpVar.isEmpty() == false )
        m_bbWidthInset = tmpVar.convert<double>();

    tmpVar = bbObj->get("lengthInset");
    if( tmpVar.isEmpty() == false )
        m_bbLengthInset = tmpVar.convert<double>();

    tmpVar = bbObj->get("width");
    if( tmpVar.isEmpty() == false )
        m_bbWidth = tmpVar.convert<double>();

    tmpVar = bbObj->get("length");
    if( tmpVar.isEmpty() == false )
        m_bbLength = tmpVar.convert<double>();

    tmpVar = bbObj->get("orientation");
    if( tmpVar.isEmpty() == false )
        m_bbOrientation = tmpVar.convert<double>();

    return HNLP_LL_RESULT_SUCCESS;
}

/*
"textProperties":{
    "format":"{text}",
    "font":"Sans Bold 27",
    "fontSize":36
}
*/
HNLP_LL_RESULT_T
HNLPLabelTextContent::initTextPropertiesFromJSONObject( Poco::JSON::Object::Ptr tpObj )
{
    Poco::Dynamic::Var tmpVar;

    tmpVar = tpObj->get("format");
    if( tmpVar.isEmpty() == false )
        m_tpFormatStr = tmpVar.convert<std::string>();

    tmpVar = tpObj->get("font");
    if( tmpVar.isEmpty() == false )
        m_tpFont = tmpVar.convert<std::string>();

    tmpVar = tpObj->get("fontSize");
    if( tmpVar.isEmpty() == false )
        m_tpFontSize = tmpVar.convert<double>();

    return HNLP_LL_RESULT_SUCCESS;
}

void
HNLPLabelTextContent::debugPrint()
{
    std::cout << "    == Bounding Box ==" << std::endl;
    std::cout << "       WidthInset : " << m_bbWidthInset << std::endl;
    std::cout << "       LengthInset: " << m_bbLengthInset << std::endl;
    std::cout << "       Width      : " << m_bbWidth << std::endl;
    std::cout << "       Length     : " << m_bbLength << std::endl;
    std::cout << "       Orientation: " << m_bbOrientation << std::endl;

    std::cout << "    == Text Properties ==" << std::endl;
    std::cout << "       Format    : " << m_tpFormatStr << std::endl;
    std::cout << "       Font      : " << m_tpFont << std::endl;
    std::cout << "       Font Size : " << m_tpFontSize << std::endl;
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

        std::cout << "caArr: " << caArr->size() << std::endl;
        
        for( size_t i = 0; i < caArr->size(); ++i ) {
            HNLPLabelTextContent *cArea; 

            Poco::JSON::Object::Ptr childObj = caArr->getObject(i);

            tmpVar = childObj->get("areaType");
            if( tmpVar.isEmpty() == true )
                continue;
            
            std::cout << "1" << std::endl;

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

            std::cout << "2" << std::endl;

            tmpVar = childObj->get("areaID");
            if( tmpVar.isEmpty() == true )
            {
                std::cout << "Content areas require a 'areaID' attribute." << std::endl;
                delete cArea;
                continue;
            }

            std::cout << "3" << std::endl;

            cArea->setID( tmpVar.convert<std::string>() );
            
            cArea->initFromJSONObject( childObj );

            m_contentAreaList.push_back( cArea );
        }
    }

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

uint
HNLPLabelLayout::GetContentAreaCount()
{
    return m_contentAreaList.size();
}
    
HNLPLabelContentArea*
HNLPLabelLayout::GetContentAreaByIndex( uint index )
{
    if( index >= m_contentAreaList.size() )
        return NULL;

    return m_contentAreaList[ index ]; 
}

void
HNLPLabelLayout::debugPrint()
{
    std::cout << "  layoutID: " << m_layoutID << std::endl;
    std::cout << "  layoutName: " << m_layoutName << std::endl;
    std::cout << "  layoutDescription: " << m_layoutDescription << std::endl;

    for( std::vector< HNLPLabelTarget >::iterator it = m_tgtLabelList.begin(); it != m_tgtLabelList.end(); it++ )
        std::cout << "Target Label" << std::endl;

    for( std::vector< HNLPLabelContentArea* >::iterator it = m_contentAreaList.begin(); it != m_contentAreaList.end(); it++ )
    {
        std::cout << "Content Area" << std::endl;        
        (*it)->debugPrint();
    }
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

HNLPLabelLayout*
HNLPLabelLayoutManager::getLayout( std::string id )
{
    std::map< std::string, HNLPLabelLayout* >::iterator it = m_layoutList.find(id);

    if( it == m_layoutList.end() )
        return NULL;

    return it->second;
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
