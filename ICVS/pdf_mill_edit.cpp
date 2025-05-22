#include "includes/pdf_mill.h"

namespace PDF_MILL
{
    void UpdateVersion(version_index version)
    {
        global_data->cur_file_read->version = version_string[version];
    }
}