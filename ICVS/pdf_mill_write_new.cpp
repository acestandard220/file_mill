#include "includes/pdf_mill.h"

namespace PDF_MILL
{
    void WriteNewPDF()
    {
        global_data->cur_new_write = std::make_shared<NewFileData>();
    }
}