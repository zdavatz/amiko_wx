//  customURLConnection.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 15 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <wx/wx.h>
#include <wx/url.h>
#include <wx/sstream.h>
#include <wx/protocol/http.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/datstrm.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include "wx/fs_zip.h"
#include <wx/zipstrm.h>

#include "customURLConnection.hpp"

void downloadTextFileWithName(wxString filename)
{
    wxString PILLBOX_ODDB_ORG("http://pillbox.oddb.org/");
	wxString dir = wxStandardPaths::Get().GetUserDataDir();

    wxURL url(PILLBOX_ODDB_ORG + filename);
    if (url.GetError() != wxURL_NOERR)
        return;

    wxString htmldata;
    {
        wxInputStream *in = url.GetInputStream();
        if (in && in->IsOk()) {
            wxStringOutputStream html_stream(&htmldata);
            in->Read(html_stream);
            //wxLogMessage(htmldata);
        }

        delete in;
    }

    wxFile file(dir + wxFILE_SEP_PATH + filename, wxFile::write);
    if ( file.IsOpened() ) {
       bool ok = file.Write( htmldata);
       file.Close();
    }
}

void downloadFileWithName(wxString filename)
{
    wxString server("pillbox.oddb.org");  // No "http://" scheme prefix !
    wxString dir = wxStandardPaths::Get().GetUserDataDir();

    wxHTTP http;
    //http.SetHeader(_T("Content-type"), "text/html; charset=utf-8");
    http.SetTimeout(10);

    wxString localFilePath( dir + wxFILE_SEP_PATH + filename);
    wxFileOutputStream output(localFilePath);
    wxDataOutputStream store(output);

//    while (!http.Connect(server))  // only the server, no pages here yet ...
//    wxSleep(5);

    if (!http.Connect(server, 80))
    {
        std::clog << "Line " << __LINE__ << " Connect fail" << std::endl;
        output.Close();
        return;
    }

    wxInputStream *stream;
    stream = http.GetInputStream("/" + filename);
    if (stream == nullptr) {
        std::clog << "Line " << __LINE__ << " GetInputStream fail" << std::endl;
        output.Close();
        return;
    }

    unsigned char buffer[1024];
    size_t byteRead;

    // receive stream
    while (!stream->Eof()) {
        stream->Read(buffer, sizeof(buffer));
        store.Write8(buffer, sizeof(buffer));
        byteRead = stream->LastRead();
        if (byteRead == 0)
            break;
        if (!stream->IsOk())
            break;
    }

    output.Close();
    
    // Unzip file, see connectionDidFinishLoading
    // For now we assume each zip file contains one file only

    if (wxFileName(filename).GetExt() != "zip")
        return;
    
    if (!wxFileName::Exists(localFilePath))
        return;

    wxFileInputStream in(localFilePath);
    wxZipInputStream zip(in);
    
    if (zip.GetTotalEntries() == 0)
        return;
    
    wxZipEntry* pZIPEntry = zip.GetNextEntry();
    wxString localUnzippedFilePath( dir + wxFILE_SEP_PATH + pZIPEntry->GetName());
    if (!zip.OpenEntry(*pZIPEntry))
        return;
    
    if (!zip.CanRead())
        return;

    wxFileOutputStream theOutputFile(localUnzippedFilePath);
    if (!
        theOutputFile.IsOk())
        return;

    zip.Read(theOutputFile);
}
