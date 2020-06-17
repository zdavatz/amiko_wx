//  Utilities.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "Utilities.hpp"

namespace UTI
{

// 64
const char * appLanguage()
{
    if (APP_NAME == "CoMed")
        return "fr";

    return "de";
}

}
