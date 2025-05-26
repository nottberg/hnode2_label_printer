#include <iostream>
#include <cstdio>
#include <mutex>

#include <cairo.h>
#include <cairo/cairo-pdf.h>
#include <pango/pangocairo.h>

#include "HNLPLabelRender.h"

#define PTS_PER_MILLIMETER  2.8346456693

static double
convert_mm_to_points( double millimeters )
{
    return (millimeters * POINTS_PER_MILLIMETER);
}

HNLPRectangle::HNLPRectangle()
{
    m_x = 0.0;
    m_y = 0.0;

    m_width  = 0.0;
    m_length = 0.0;
}

HNLPRectangle::~HNLPRectangle()
{

}

void
HNLPRectangle::setDimensions( double x, double y, double width, double length )
{
    m_x = x;
    m_y = y;

    m_width  = width;
    m_length = length;
}

double
HNLPRectangle::getX()
{
    return m_x;
}

double
HNLPRectangle::getY()
{
    return m_y;
}

double
HNLPRectangle::getW()
{
    return m_width;
}

double
HNLPRectangle::getL()
{
    return m_length;
}

HNLPLabelRenderTextRegion::HNLPLabelRenderTextRegion()
{

}

HNLPLabelRenderTextRegion::~HNLPLabelRenderTextRegion()
{

}

void 
HNLPLabelRenderTextRegion::setContent( std::string value )
{
    m_content = value;
}

void
HNLPLabelRenderTextRegion::setFont( std::string fontName, double minSize, double maxSize, bool autoSize )
{
    if( fontName.empty() == false )
        m_fontName = fontName;

    m_fontMinSize = minSize;
    m_fontMaxSize = maxSize;
    
    m_fontAutoSize = autoSize;
}

void
HNLPLabelRenderTextRegion::setDimensions( double x, double y, double width, double height )
{
    m_x = x;
    m_y = y;

    m_width  = width;
    m_height = height;
}

std::string
HNLPLabelRenderTextRegion::getContent()
{
    return m_content;
}

HNLPLabelRender::HNLPLabelRender()
{

}

HNLPLabelRender::~HNLPLabelRender()
{

}

/*
void
HNLPLabelRender::setPrintInfo( std::string mediaName, double widthMM, double heightMM )
{
    m_mediaName = mediaName;

    m_width = convert_mm_to_points( widthMM );
    m_height = convert_mm_to_points( heightMM );
}

uint
HNLPLabelRender::createTextRegion()
{
    HNLPLabelRenderTextRegion newRegion;
    m_textRegions.push_back( newRegion );
    return m_textRegions.size() - 1;
}

void
HNLPLabelRender::setTextRegionContent( uint regionID, std::string textData )
{
    m_textRegions[ regionID ].setContent( textData );
}

void
HNLPLabelRender::setTextRegionFont( uint regionID, std::string fontName, double minSize, double maxSize, bool autoSize )
{
    m_textRegions[ regionID ].setFont( fontName, minSize, maxSize, autoSize );
}

void
HNLPLabelRender::setTextRegionDimensions( uint regionID, double x, double y, double width, double height )
{
    m_textRegions[ regionID ].setDimensions( x, y, width, height );
}


void
HNLPLabelRender::setTextRegionAlignment( uint regionID )
{

}

void
HNLPLabelRender::setTextRegionOrientation( uint regionID )
{

}
*/


HNLP_LR_RESULT_T 
HNLPLabelRender::applyTextRegionsPDF( HNLPLabelLayout *labelLayout, HNLPLabelRequest *request, void *context )
{
    cairo_t *cr = (cairo_t *) context;

    for( int i = 0; i < labelLayout->GetContentAreaCount(); i++ )
    {
        HNLPLabelContentArea *cArea = labelLayout->GetContentAreaByIndex( i );

        applyTextRegionPDF( labelLayout, request, cArea, context );
    }

    return HNLP_LR_RESULT_SUCCESS;
}

HNLP_LR_RESULT_T
HNLPLabelRender::applyTextRegionPDF( HNLPLabelLayout *labelLayout, HNLPLabelRequest *request, HNLPLabelContentArea *contentArea, void *context )
{
    PangoLayout *layout;
    PangoFontDescription *desc;

    cairo_t *cr = (cairo_t *) context;

    /* Create a PangoLayout, set the font and text */
    layout = pango_cairo_create_layout( cr );

    pango_layout_set_text( layout, "Test Text For Label", -1 );

    desc = pango_font_description_from_string( "Sans Bold" );
    //pango_font_description_set_absolute_size( desc, FONT_SIZE * DEVICE_DPI * PANGO_SCALE / (72.0 * TWEAKABLE_SCALE) );
    pango_font_description_set_size( desc, 14 * PANGO_SCALE );

    printf( "PANGO_SCALE = %d\n", PANGO_SCALE );
    pango_layout_set_font_description( layout, desc );
    pango_font_description_free( desc );

    pango_layout_set_alignment( layout, PANGO_ALIGN_CENTER );

    cairo_save( cr );



    /* Inform Pango to re-layout the text with the new transformation */
    pango_cairo_update_layout( cr, layout );

    int loWidth, loLength;
    pango_layout_get_size( layout, &loLength, &loWidth );

    double loWidthPts = loWidth / PANGO_SCALE;
    double loLengthPts = loLength / PANGO_SCALE;

    printf( "PDF - text width(pts): %f,  length(pts): %f\n", loWidthPts, loLengthPts );

    //double centerX = m_width / 2.0;
    //double centerY = m_height / 2.0;
    /*
    double centerX = 250.0 / 2.0;
    double centerY = 635.0 / 2.0;

    printf( "move_dx: %f  move_dy: %f\n", move_dx, move_dy );
    */
    double textWidthPts  = (79 / 2.0); 
    double textLengthPts = (252 / 2.0);

    printf( "PDF - text final width(pts): %f,  length(pts): %f\n", textWidthPts, textLengthPts );

    double centerWidthInsetPts  = loWidthPts / 2.0;
    double centerLengthInsetPts = loLengthPts / 2.0;

    printf( "PDF - text center width(pts): %f,  length(pts): %f\n", centerWidthInsetPts, centerLengthInsetPts );

    // Move to the center of the text
    cairo_move_to( cr, centerWidthInsetPts + 10.5, centerLengthInsetPts + 98 );
    
    // Rotate text to the correct orientation
    cairo_rotate( cr, 270 * G_PI / 180. );

    /* Inform Pango to re-layout the text with the new transformation */
    pango_cairo_update_layout( cr, layout );    
    
    // Set text color
    cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );

    // Render
    pango_cairo_show_layout( cr, layout );

    cairo_restore( cr );

    /* free the layout object */
    g_object_unref( layout );

    return HNLP_LR_RESULT_SUCCESS;
}

HNLP_LR_RESULT_T
HNLPLabelRender::renderTemporaryPDF( HNLPLabelSpec *spec, HNLPLabelLayout *layout, HNLPLabelRequest *request, std::string outFile )
{
    cairo_surface_t *surf;
    cairo_t *cr;

    if( spec->isBoundary( HNLP_AB_TYPE_SQUARE ) == false )
    {
        std::cerr << "Currently only square boundaries are supported." << std::endl;
        return HNLP_LR_RESULT_FAILURE;
    }

    double aspectRatio = spec->getBoundaryLength()/spec->getBoundaryWidth();

    std::cout << "PDF - AspectRatio: " << aspectRatio << std::endl;

    double widthPts  = spec->getBoundaryWidth() * PTS_PER_MILLIMETER; 
    double lengthPts = spec->getBoundaryLength() * PTS_PER_MILLIMETER;
    
    std::cout << "PDF - widthPts: " << widthPts << std::endl;
    std::cout << "PDF - lengthPts: " << lengthPts << std::endl;

    surf = cairo_pdf_surface_create( outFile.c_str(), widthPts, lengthPts );
    cr = cairo_create( surf );

    //cairo_scale( cr, width, height );
    cairo_set_line_width( cr, 0.01 );

    cairo_rectangle( cr, 0, 0, widthPts, lengthPts );
    cairo_set_source_rgb( cr, 1, 1, 1 );
    cairo_fill( cr );

    applyTextRegionsPDF( layout, request, cr );

    /* write output and clean up */
    cairo_surface_finish(surf);

    cairo_destroy( cr );
    cairo_surface_destroy( surf );

    return HNLP_LR_RESULT_SUCCESS;
}

HNLP_LR_RESULT_T 
HNLPLabelRender::applyTextRegionsPNG( HNLPLabelLayout *labelLayout, HNLPLabelRequest *request, void *context )
{
    PangoLayout *layout;
    PangoFontDescription *desc;

    cairo_t *cr = (cairo_t *) context;

    // Move to the upper-left corner of the text region
    cairo_translate( cr, 0, 0 );

    /* Create a PangoLayout, set the font and text */
    layout = pango_cairo_create_layout( cr );

    pango_layout_set_text( layout, "Test Text For Label", -1 );

    desc = pango_font_description_from_string( "Sans Bold" );
    //pango_font_description_set_absolute_size( desc, FONT_SIZE * DEVICE_DPI * PANGO_SCALE / (72.0 * TWEAKABLE_SCALE) );
    pango_font_description_set_size( desc, 20 * PANGO_SCALE );

    printf( "PANGO_SCALE = %d\n", PANGO_SCALE );
    pango_layout_set_font_description( layout, desc );
    pango_font_description_free( desc );

    pango_layout_set_alignment( layout, PANGO_ALIGN_CENTER );

    cairo_save( cr );

    /* Gradient from red at angle == 60 to blue at angle == 240 */
    //red   = (1 + cos ((angle - 60) * G_PI / 180.)) / 2;
    cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );


    /* Inform Pango to re-layout the text with the new transformation */
    pango_cairo_update_layout( cr, layout );

    int loWidth, loHeight;
    pango_layout_get_size( layout, &loWidth, &loHeight );

    printf( "width: %u,  height: %u\n", loWidth, loHeight );

    double move_dx = ((double) loWidth) / PANGO_SCALE;
    move_dx /= 2.0;

    double move_dy = ((double) loHeight) / PANGO_SCALE;
    move_dy /= 2.0;

    //double centerX = m_width / 2.0;
    //double centerY = m_height / 2.0;
    double centerX = 250.0 / 2.0;
    double centerY = 635.0 / 2.0;

    printf( "move_dx: %f  move_dy: %f\n", move_dx, move_dy );

//    cairo_move_to( cr, centerX, centerY );

    cairo_move_to( cr, centerX - move_dy, centerY + move_dx );
    
    cairo_rotate( cr, 270 * G_PI / 180. );

    pango_cairo_show_layout( cr, layout );

    cairo_restore( cr );

    /* free the layout object */
    g_object_unref( layout );

    return HNLP_LR_RESULT_SUCCESS;
}

static cairo_status_t
ostream_write_function( void *userdata, const unsigned char *data, unsigned length )
{
    std::ostream *outStream = (std::ostream *) userdata;

    outStream->write( (const char*)data, length );
    
    return CAIRO_STATUS_SUCCESS;
}

HNLP_LR_RESULT_T
HNLPLabelRender::renderPreviewToPNGStream( HNLPLabelSpec *spec, 
                                           HNLPLabelLayout *layout,
                                           HNLPLabelRequest *request,
                                           std::ostream *outStream )
{
    cairo_surface_t *surf;
    cairo_t *cr;

    if( spec->isBoundary( HNLP_AB_TYPE_SQUARE ) == false )
    {
        std::cerr << "Currently only square boundaries are supported." << std::endl;
        return HNLP_LR_RESULT_FAILURE;
    }

    double aspectRatio = spec->getBoundaryLength()/spec->getBoundaryWidth();

    std::cout << "Preview - AspectRatio: " << aspectRatio << std::endl;

    double widthPixel  = 200;
    double lengthPixel = widthPixel * aspectRatio;
    
    std::cout << "Preview - widthPX: " << widthPixel << std::endl;
    std::cout << "Preview - lengthPX: " << lengthPixel << std::endl;

    double PixelPerMillimeter = widthPixel / spec->getBoundaryWidth();

    std::cout << "Preview - pixel per mm: " << PixelPerMillimeter << std::endl;

    double padWidth  = 50;
    double paddedWidth = widthPixel + padWidth;
    double padLength = 50;
    double paddedLength = lengthPixel + padLength;

    std::cout << "Preview - paddedWidth: " << paddedWidth << std::endl;
    std::cout << "Preview - paddedLength: " << paddedLength << std::endl;

    double ux=2, uy=2;

    surf = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, paddedWidth, paddedLength );
    cr = cairo_create( surf );

    //cairo_scale( cr, width, height );
    cairo_set_line_width( cr, 0.01 );

    cairo_rectangle( cr, 0, 0, paddedWidth, paddedLength );
    cairo_set_source_rgb( cr, 0.184, 0.309, 0.309 );
    cairo_fill( cr );

    //draw_diagram( name, cr );

    //cairo_device_to_user_distance( cr, &ux, &uy );
    //if( ux < uy )
    //    ux = uy;
    //cairo_set_line_width( cr, ux );
    cairo_rectangle( cr, padWidth/2, padLength/2, widthPixel, lengthPixel );
    cairo_set_source_rgb( cr, 1, 1, 1 );
    cairo_fill( cr );
    //cairo_stroke( cr );

    applyTextRegionsPNG( layout, request, cr );

    /* write output and clean up */
    cairo_surface_write_to_png_stream( surf, ostream_write_function, (void *)outStream );
    cairo_destroy( cr );
    cairo_surface_destroy( surf );

    return HNLP_LR_RESULT_SUCCESS;
}

