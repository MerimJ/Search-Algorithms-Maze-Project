#include "Application.h"
#include <td/StringConverter.h>
#include <gui/WinMain.h>

int main(int argc, const char * argv[])
{
    Application app(argc, argv);
    auto appProperties = app.getProperties();
    td::String trLang = appProperties->getValue("translation", "EN");
    app.init(trLang);
    return app.run();
}
