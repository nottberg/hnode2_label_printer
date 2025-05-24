#include <iostream>
#include <cstdio>
#include <mutex>

#include <cairo.h>
#include <cairo/cairo-pdf.h>
#include <pango/pangocairo.h>

#include "HNLPLabelRender.h"

static double
convert_mm_to_points( double millimeters )
{
    return (millimeters * POINTS_PER_MILLIMETER);
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

HNLP_LR_RESULT_T 
HNLPLabelRender::applyTextRegion( uint regionID )
{
    PangoLayout *layout;
    PangoFontDescription *desc;
#if 0
    // Move to the upper-left corner of the text region
    cairo_translate( cr, 0, 0 );

    /* Create a PangoLayout, set the font and text */
    layout = pango_cairo_create_layout( cr );

    pango_layout_set_text( layout, rgnPtr->getContent().c_str(), -1 );

    desc = pango_font_description_from_string( FONT );
    //pango_font_description_set_absolute_size( desc, FONT_SIZE * DEVICE_DPI * PANGO_SCALE / (72.0 * TWEAKABLE_SCALE) );
    pango_font_description_set_size( desc, 10 * PANGO_SCALE );

    printf( "PANGO_SCALE = %d\n", PANGO_SCALE );
    pango_layout_set_font_description( layout, desc );
    pango_font_description_free( desc );

    pango_layout_set_alignment( layout, PANGO_ALIGN_CENTER );

    cairo_save( cr );

    /* Gradient from red at angle == 60 to blue at angle == 240 */
    //red   = (1 + cos ((angle - 60) * G_PI / 180.)) / 2;
    cairo_set_source_rgb( cr, 0.0, 0.0, 0.0 );

    //cairo_rotate( cr, angle * G_PI / 180. );

    /* Inform Pango to re-layout the text with the new transformation */
    pango_cairo_update_layout( cr, layout );

    pango_layout_get_size( layout, &loWidth, &loHeight );

    printf( "width: %u,  height: %u\n", loWidth, loHeight );

    double move_dx = ((double) loWidth) / PANGO_SCALE;
    move_dx /= 2.0;

    double move_dy = ((double) loHeight) / PANGO_SCALE;
    move_dy /= 2.0;

    double centerX = m_width / 2.0;
    double centerY = m_height / 2.0;

    printf( "move_dx: %f  move_dy: %f\n", move_dx, move_dy );

    cairo_move_to( cr, centerX - move_dx, centerY - move_dy );
    
    pango_cairo_show_layout( cr, layout );

    cairo_restore( cr );

    /* free the layout object */
    g_object_unref( layout );

#endif
}

HNLP_LR_RESULT_T
HNLPLabelRender::renderPreviewPNG()
{
    cairo_surface_t *surf;
    cairo_t *cr;

    double width=120, height=120;
    double ux=2, uy=2;

//	char svg_filename[50];
    char png_filename[50];
//	strcpy (svg_filename, "tst");
    strcpy (png_filename, "tst");
//	strcat (svg_filename, ".svg");
    strcat (png_filename, ".png");

    surf = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, width, height );
    cr = cairo_create( surf );

    cairo_scale( cr, width, height );
    cairo_set_line_width( cr, 0.01 );

    cairo_rectangle( cr, 0, 0, 1, 1 );
    cairo_set_source_rgb( cr, 1, 1, 1 );
    cairo_fill( cr );

    //draw_diagram( name, cr );

    cairo_device_to_user_distance( cr, &ux, &uy );
    if( ux < uy )
        ux = uy;
    cairo_set_line_width( cr, ux );
    cairo_set_source_rgb( cr, 0, 0, 0 );
    cairo_rectangle( cr, 0, 0, 1, 1 );
    cairo_stroke( cr );

    /* write output and clean up */
    cairo_surface_write_to_png( surf, png_filename );
    cairo_destroy( cr );
    cairo_surface_destroy( surf );

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

    /* write output and clean up */
    cairo_surface_write_to_png_stream( surf, ostream_write_function, (void *)outStream );
    cairo_destroy( cr );
    cairo_surface_destroy( surf );

    return HNLP_LR_RESULT_SUCCESS;
}

