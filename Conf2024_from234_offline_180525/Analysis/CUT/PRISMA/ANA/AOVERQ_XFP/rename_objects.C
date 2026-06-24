// --------------------------------------------------------------
// rename_objects.C
// Kjør med:
//    root -l -b -q "rename_objects.C()"
//
// Skriptet håndterer ALLE filene i listen nedenfor:
//   – bytter prefiks i FILNAVNET fra "aoqxfp" til "aoverqxf"
//   – bytter prefiks i *alle* TNamed‑objekter (TCut, TGraph, TH1, …)
// --------------------------------------------------------------

#include <TFile.h>
#include <TKey.h>
#include <TObject.h>
#include <TNamed.h>
#include <TString.h>
#include <vector>
#include <iostream>

// ------------------------------------------------------------------
// Kopier innholdet fra én ROOT‑fil til en ny fil med oppdatert
// objektnavn (kun for objekter som arver fra TNamed).
// ------------------------------------------------------------------
void copy_and_rename(const char* oldFile,
                     const char* newFile,
                     const char* oldPref,
                     const char* newPref)
{
    // Åpne kildefilen (read‑only)
    TFile *src = TFile::Open(oldFile, "READ");
    if (!src || src->IsZombie()) {
        std::cerr << " *** ERROR: cannot open " << oldFile << std::endl;
        return;
    }

    // Opprett (eller overskriv) målfilen
    TFile *dst = TFile::Open(newFile, "RECREATE");
    if (!dst || dst->IsZombie()) {
        std::cerr << " *** ERROR: cannot create " << newFile << std::endl;
        src->Close();
        return;
    }

    // Gå gjennom alle nøkler/objekter i kildefilen
    TIter next(src->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)next())) {
        TObject *obj = key->ReadObj();            // les objektet
        TString   name = obj->GetName();          // nåværende navn

        // Endre navn kun dersom objektet er et TNamed‑subclass og
        // navnet starter med den gamle prefiksen.
        if (obj->InheritsFrom("TNamed") && name.BeginsWith(oldPref)) {
            TNamed *named = static_cast<TNamed*>(obj);
            // Bytt prefiks i navnet
            TString newName = name;
            newName.Replace(0, std::strlen(oldPref), newPref); // erstatter første forekomst
            named->SetName(newName.Data());
            named->SetTitle(newName.Data()); // holder Title i sync
        }

        // Skriv (eventuelt med nytt navn) til målfilen
        dst->WriteTObject(obj);
        delete obj;   // unngå minnelekkasje
    }

    src->Close();
    dst->Close();

    std::cout << "  > " << oldFile << "  →  " << newFile
              << "   (objektnavn oppdatert)" << std::endl;
}

// ------------------------------------------------------------------
// Hovedfunksjon – kjøres når macro‑et blir kalt fra ROOT.
// ------------------------------------------------------------------
void rename_objects()
{
    const char* oldPref = "aoverqxfp";
    const char* newPref = "aoqvsxfp";

    // --------------------------------------------------------------
    // 1️⃣  LISTE OVER FILENE DU VIL BEHANDLE
    // --------------------------------------------------------------
    std::vector<TString> files = {
        "aoverqxfp__30_26__70.root",
        "aoverqxfp__26_20__63.root",
        "aoverqxfp__26_18__63.root",
        "aoverqxfp__26_19__63.root",
        "aoverqxfp__26_21__63.root",
        "aoverqxfp__26_22__65.root",
        "aoverqxfp__29_20__68.root",
        "aoverqxfp__27_20__65.root",
        "aoverqxfp__30_21__70.root",
        "aoverqxfp__29_21__69.root",
        "aoverqxfp__28_21__66.root",
        "aoverqxfp__27_21__64.root",
        "aoverqxfp__30_22__70.root",
        "aoverqxfp__29_22__68.root",
        "aoverqxfp__28_22__66.root",
        "aoverqxfp__27_22__65.root",
        "aoverqxfp__30_24__70.root",
        "aoverqxfp__29_24__68.root",
        "aoverqxfp__28_24__68.root",
        "aoverqxfp__30_25__70.root",
        "aoverqxfp__29_25__69.root",
        "aoverqxfp__30_20__70.root",
        "aoverqxfp__30_23__70.root",
        "aoverqxfp__29_23__68.root",
        "aoverqxfp__28_20__65.root",
        "aoverqxfp__28_23__67.root",
        "aoverqxfp__27_23__66.root"
    };

    // --------------------------------------------------------------
    // 2️⃣  BEHANDLE HVER FIL
    // --------------------------------------------------------------
    for (const auto& oldName : files) {
        // Bytt prefiks i filnavnet
        TString newName = oldName;
        newName.Replace(0, std::strlen(oldPref), newPref);   // aoqxfp → aoverqxf

        // Kopier innholdet + rename objektnavn
        copy_and_rename(oldName.Data(), newName.Data(), oldPref, newPref);

        // // ---------------------------------------------------------
        // // 3️⃣  SLETT GJAMLE FIL (valgfritt – kommenter ut hvis du vil beholde)
        // // ---------------------------------------------------------
        // if (gSystem->AccessPathName(newName.Data())) {
        //     // Ny fil ble ikke opprettet – behold den gamle
        //     std::cerr << " *** WARN: " << newName << " ble IKKE opprettet – beholder " << oldName << std::endl;
        // } else {
        //     if (gSystem->Remove(oldName.Data()) != 0) {
        //         std::cerr << " *** WARN: klarte ikke å slette " << oldName << std::endl;
        //     } else {
        //         std::cout << "    (slettet gammel fil: " << oldName << ")" << std::endl;
        //     }
        // }
    }

    std::cout << "\n=== ALL DONE ===\n";
}
