//  InteractionsHtmlView.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 9 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

class InteractionsCart;
class Medication;
class InteractionsAdapter;

class InteractionsHtmlView
{
public:
    InteractionsHtmlView();
    virtual ~InteractionsHtmlView() {}
    
    void pushToMedBasket(Medication *med);
    wxString fullInteractionsHtml(InteractionsAdapter *interactions);
    
    InteractionsCart *medCart;
};
