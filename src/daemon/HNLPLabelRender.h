#ifndef __HNLP_LABEL_RENDER_H__
#define __HNLP_LABEL_RENDER_H__

#include <string>
#include <map>
#include <vector>

#include <cups/cups.h>

#include <hnode2/HNodeConfig.h>

#define FONT "Sans Bold 27"
#define FONT_SIZE 36
#define DEVICE_DPI 72

//#define TWEAKABLE_SCALE ((double) 0.1)

//#define LABEL_WIDTH    89.0
//#define LABEL_HEIGHT   28.0

#define TEXT_REGION1_HORIZONTAL_INSET  1.0
#define TEXT_REGION1_VERTICAL_INSET    1.0
#define TEXT_REGION1_WIDTH             87.0
#define TEXT_REGION1_HEIGHT            26.0

#define POINTS_PER_MILLIMETER  ((double)2.8346456693)

typedef enum HNLPLabelRenderResult
{
    HNLP_LR_RESULT_SUCCESS,
    HNLP_LR_RESULT_FAILURE  
}HNLP_LR_RESULT_T;

class HNLPLabelRenderTextRegion
{
    public:
        HNLPLabelRenderTextRegion();
        ~HNLPLabelRenderTextRegion();

        void setContent( std::string value );
        void setFont( std::string fontName, double minSize, double maxSize, bool autoSize );
        void setDimensions( double x, double y, double width, double height );

        std::string getContent();

    private:

        std::string   m_content;

        std::string m_fontName;
        double m_fontMinSize;
        double m_fontMaxSize;
        bool m_fontAutoSize;
        double m_fontCalcSize;

        double m_x;
        double m_y;
        double m_width;
        double m_height;

};

class HNLPLabelRender
{
    public:
        HNLPLabelRender();
       ~HNLPLabelRender();

        void setPrintInfo( std::string mediaName, double widthMM, double heightMM );

        uint createTextRegion();

        HNLP_LR_RESULT_T applyTextRegion( uint regionID );

        void setTextRegionContent( uint regionID, std::string textData );

        void setTextRegionFont( uint regionID, std::string fontName, double minSize, double maxSize, bool autoSize );

        void setTextRegionDimensions( uint regionID, double x, double y, double width, double height );

        void setTextRegionAlignment( uint regionID );

        void setTextRegionOrientation( uint regionID );

        HNLP_LR_RESULT_T renderPreviewPNG();
        
        HNLP_LR_RESULT_T renderPreviewToPNGStream(std::ostream *outStream);

    private:

        std::string m_mediaName;

        double m_width;
        double m_height;

        std::string m_filename;

        std::vector< HNLPLabelRenderTextRegion > m_textRegions;    
};

#endif // __HNLP_LABEL_RENDER_H__
