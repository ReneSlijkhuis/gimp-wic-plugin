///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    MIT License
//
//    Copyright(c) 2017 René Slijkhuis
//
//    Permission is hereby granted, free of charge, to any person obtaining a copy
//    of this software and associated documentation files (the "Software"), to deal
//    in the Software without restriction, including without limitation the rights
//    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the Software is
//    furnished to do so, subject to the following conditions:
//
//    The above copyright notice and this permission notice shall be included in all
//    copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//    SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "libgimp/stdplugins-intl.h"

#include "wic.h"

#include "WIC_Decoder.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char LOAD_PROCEDURE[] = "file-wic-load";
const char BINARY_NAME[]    = "file-wic";

// Predeclare our entrypoints
void query( );
void run( const gchar*, gint, const GimpParam*, gint*, GimpParam** );

// Declare our plugin entry points
GimpPlugInInfo PLUG_IN_INFO =
{
    NULL,
    NULL,
    query,
    run
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MAIN( )

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void query( )
{
    // Load arguments
    static const GimpParamDef load_arguments[] =
    {
        { GIMP_PDB_INT32,  "run-mode",     "The run mode { RUN-INTERACTIVE (0), RUN-NONINTERACTIVE (1) }" },
        { GIMP_PDB_STRING, "filename",     "The name of the file to load" },
        { GIMP_PDB_STRING, "raw-filename", "The name entered" }
    };

    // Load return values
    static const GimpParamDef load_return_values[] =
    {
        { GIMP_PDB_IMAGE, "image", "Output image" }
    };

    // Install the load procedure
    gimp_install_procedure( LOAD_PROCEDURE,                                                         // name
                            "Loads all images supported by the Windows Imaging Component (WIC)",    // blurb
                            "Loads all images supported by the Windows Imaging Component (WIC)",    // help
                            "Rene Slijkhuis",                                                       // author
                            "Rene Slijkhuis",                                                       // copyright
                            "2017",                                                                 // date
                            "WIC Supported images",                                                 // menu label
                            NULL,                                                                   // image types
                            GIMP_PLUGIN,                                                            // type
                            G_N_ELEMENTS( load_arguments ),                                         // n params
                            G_N_ELEMENTS( load_return_values ),                                     // n return vals
                            load_arguments,                                                         // params
                            load_return_values );                                                   // return vals

    char* pExtensions = supported_extensions( );

    // Register the load handlers
    gimp_register_file_handler_mime( LOAD_PROCEDURE, "image/feiraw" );
    //gimp_register_load_handler( LOAD_PROCEDURE, "webp", "" );
    gimp_register_magic_load_handler( LOAD_PROCEDURE,
                                      pExtensions, // syntax: "mrc,emi,cr2", -> No spaces, dots, etc
                                      "",
                                      "" );

    free( pExtensions );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void run( const gchar* name,
          gint nparams,
          const GimpParam* param,
          gint* nreturn_vals,
          GimpParam** return_vals )
{
    // Create the return value.
    static GimpParam return_values[2];
    *nreturn_vals = 1;
    *return_vals  = return_values;

    // Set the return value to success by default
    return_values[0].type          = GIMP_PDB_STATUS;
    return_values[0].data.d_status = GIMP_PDB_SUCCESS;

    // Check to see if this is the load procedure
    if ( strcmp( name, LOAD_PROCEDURE ) == 0 )
    {
        OutputDebugString( L"[WIC_Plugin] LOAD_PROCEDURE is called" );
        
        // Check to make sure all parameters were supplied
        if ( nparams != 3 )
        {
            return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
            return;
        }

        // Now read the image
        int imageId = read_wic( param[1].data.d_string );

        // Check for an error
        if( imageId == -1 )
        {
            return_values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
            return;
        }

        // Fill in the second return value
        *nreturn_vals = 2;

        return_values[1].type         = GIMP_PDB_IMAGE;
        return_values[1].data.d_image = imageId;
    }
    else
    {
        OutputDebugString( L"[WIC_Plugin] Unsupported procedure is called!" );
        
        return_values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////