#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <errno.h>
#include "CUnicodeDatabase.h"

SUtf8Table CUnicodeDatabase::utf8Table[] = {
    { 0x80, 0x00, 0 * 6, 0x00000000, 0x0000007F, /* 1 byte sequence */ },
    { 0xE0, 0xC0, 1 * 6, 0x00000080, 0x000007FF, /* 2 byte sequence */ },
    { 0xF0, 0xE0, 2 * 6, 0x00000800, 0x0000FFFF, /* 3 byte sequence */ },
    { 0xF8, 0xF0, 3 * 6, 0x00010000, 0x001FFFFF, /* 4 byte sequence */ },
    { 0xFC, 0xF8, 4 * 6, 0x00200000, 0x03FFFFFF, /* 5 byte sequence */ },
    { 0xFE, 0xFC, 5 * 6, 0x04000000, 0x7FFFFFFF, /* 6 byte sequence */ },
    { 0, /* end of table    */ }
};
 
CUnicodeDatabase::CUnicodeDatabase() {

    // General Category 
    mapGeneralCategory[""] = EGC_default;
    mapGeneralCategory["Lu"] = EGC_Lu;
    mapGeneralCategory["Ll"] = EGC_Ll;
    mapGeneralCategory["Lt"] = EGC_Lt;
    mapGeneralCategory["Lm"] = EGC_Lm;
    mapGeneralCategory["Lo"] = EGC_Lo;
    mapGeneralCategory["Mn"] = EGC_Mn;
    mapGeneralCategory["Mc"] = EGC_Mc;
    mapGeneralCategory["Me"] = EGC_Me;
    mapGeneralCategory["Nd"] = EGC_Nd;
    mapGeneralCategory["Nl"] = EGC_Nl;
    mapGeneralCategory["No"] = EGC_No;
    mapGeneralCategory["Pc"] = EGC_Pc;
    mapGeneralCategory["Pd"] = EGC_Pd;
    mapGeneralCategory["Ps"] = EGC_Ps;
    mapGeneralCategory["Pe"] = EGC_Pe;
    mapGeneralCategory["Pi"] = EGC_Pi;
    mapGeneralCategory["Pf"] = EGC_Pf;
    mapGeneralCategory["Po"] = EGC_Po;
    mapGeneralCategory["Sm"] = EGC_Sm;
    mapGeneralCategory["Sc"] = EGC_Sc;
    mapGeneralCategory["Sk"] = EGC_Sk;
    mapGeneralCategory["So"] = EGC_So;
    mapGeneralCategory["Zs"] = EGC_Zs;
    mapGeneralCategory["Zl"] = EGC_Zl;
    mapGeneralCategory["Zp"] = EGC_Zp;
    mapGeneralCategory["Cc"] = EGC_Cc;
    mapGeneralCategory["Cf"] = EGC_Cf;
    mapGeneralCategory["Cs"] = EGC_Cs;
    mapGeneralCategory["Co"] = EGC_Co;
    mapGeneralCategory["Cn"] = EGC_Cn;

    // Bidi Class
    mapBidiClass[""] = EBC_default;
    mapBidiClass["L"] = EBC_L;
    mapBidiClass["LRE"] = EBC_LRE;
    mapBidiClass["LRO"] = EBC_LRO;
    mapBidiClass["R"] = EBC_R;
    mapBidiClass["AL"] = EBC_AL;
    mapBidiClass["RLE"] = EBC_RLE;
    mapBidiClass["RLO"] = EBC_RLO;
    mapBidiClass["PDF"] = EBC_PDF;
    mapBidiClass["EN"] = EBC_EN;
    mapBidiClass["ES"] = EBC_ES;
    mapBidiClass["ET"] = EBC_ET;
    mapBidiClass["AN"] = EBC_AN;
    mapBidiClass["CS"] = EBC_CS;
    mapBidiClass["NSM"] = EBC_NSM;
    mapBidiClass["BN"] = EBC_BN;
    mapBidiClass["B"] = EBC_B;
    mapBidiClass["S"] = EBC_S;
    mapBidiClass["WS"] = EBC_WS;
    mapBidiClass["ON"] = EBC_ON;

    // Decomposition Type
    mapDecompositionType[""] = EDT_default;
    mapDecompositionType["<font>"] = EDT_font;
    mapDecompositionType["<noBreak>"] = EDT_noBreak;
    mapDecompositionType["<initial>"] = EDT_initial;
    mapDecompositionType["<medial>"] = EDT_medial;
    mapDecompositionType["<final>"] = EDT_final;
    mapDecompositionType["<isolated>"] = EDT_isolated;
    mapDecompositionType["<circle>"] = EDT_circle;
    mapDecompositionType["<super>"] = EDT_super;
    mapDecompositionType["<sub>"] = EDT_sub;
    mapDecompositionType["<vertical>"] = EDT_vertical;
    mapDecompositionType["<wide>"] = EDT_wide;
    mapDecompositionType["<narrow>"] = EDT_narrow;
    mapDecompositionType["<small>"] = EDT_small;
    mapDecompositionType["<square>"] = EDT_square;
    mapDecompositionType["<fraction>"] = EDT_fraction;
    mapDecompositionType["<compat>"] = EDT_compat;

    // Bidi Mirrored
    mapBidiMirrored[""] = EBM_default;
    mapBidiMirrored["Y"] = EBM_Y;
    mapBidiMirrored["N"] = EBM_N;
   
};

CUnicodeCharacterData *CUnicodeDatabase::unicodeCharacterDataFind(UINT32 code) {
    int i;
    int j;
    int n;    

    i = 0;
    j = dataTable.size()-1;
    while(i <= j) {
        n = (i+j)/2;
        if(dataTable[n].code == code) {
            return &dataTable[n];
        }
        else if(dataTable[n].code > code) {
            j=n-1;
        }
        else {
            i=n+1;
        }
    }
    return 0;
}

int split(vector<string>& v, const string& s, const char delimiter) {
    string t = "";
    unsigned int i;

    v.clear();
    for(i = 0; i < s.length(); i++) {
        if(s[i] != delimiter)
            t += s[i];
        else {
            v.push_back(t);
            t = "";
        }
    }
    if(t != "") {
        v.push_back(t);
    }

    return 0;
}

vector<UINT32> readCodes(vector<string>& v) {

    vector<UINT32> codes;

    for (int i = 0; i < v.size(); i++) {
        char *ep = 0;
        long l = strtol(v[i].c_str(),&ep,16);
        if(*ep != 0 || errno == EINVAL || errno == ERANGE) {
            continue;
        }
        codes.push_back(l);
    }

    return codes;
}

int CUnicodeDatabase::parse(CUnicodeCharacterData& r, const string& s) {
    vector<string> v;
    vector<string> tv;
    vector<UINT32> codes;
    string t;
    char *ep;

    split(v, s, ';');

    for(int i = 0; i < v.size(); i++) {
        string& value = v[i];     
        switch (i) {
            case 0: // Code
                split(tv,value,' ');
                codes = readCodes(tv);
                if(codes.size() == 1) {
                    r.code = codes[0];
                }
                break;
            case 1: // Name
                r.name = value;
                break;
            case 2: // General Category
                r.generalCategory = mapGeneralCategory[value];
                break;
            case 3: // Canonical Combining Class
                ep = 0;
                r.canonicalCombiningClass = strtol(value.c_str(),&ep,10);
                break;
            case 4: // Bidi Class
                r.bidiClass = mapBidiClass[value];
                break;
            case 5: // Decomposition Type + Decomposition Mapping
                r.decompositionType = EDT_default;
                split(tv,value,' ');
                if(tv.size() > 0) {
                    r.decompositionType = mapDecompositionType[tv[0]];
                    if(r.decompositionType != EDT_default) {
                        tv.erase(tv.begin(),tv.begin()+1);
                    }
                }
                codes = readCodes(tv);
                r.decompositionMapping = codes;
                break;
            case 6: // Numeric Value 1
                r.numericType = ENT_default;
                r.numericValue = "";

                if(r.numericValue == "" && value != "") {
                    r.numericType = ENT_decimalDigit;
                    r.numericValue = value;
                }
                break;
            case 7: // Numeric Value 2
                if(r.numericValue == "" && value != "") {
                    r.numericType = ENT_digit;
                    r.numericValue = value;
                }
                break;
            case 8: // Numeric Value 3
                if(r.numericValue == "" && value != "") {
                    r.numericType = ENT_numeric;
                    r.numericValue = value;
                }
                break;
            case 9: // Bidi Mirrored
                r.bidiMirrored = mapBidiMirrored[value];
                break;
            case 10: // Unicode 1.0 Name
                r.unicode1Name = value;
                break;
            case 11: // ISO Commment
                r.isoComment = value;
                break;
            case 12: // Simple Uppercase Mapping
                split(tv,value,' ');
                codes = readCodes(tv);
                r.simpleUppecaseMapping = codes;
                break;
            case 13: // Simple Lowercase Mapping
                split(tv,value,' ');
                codes = readCodes(tv);
                r.simpleLowercaseMapping = codes;
                break;
            case 14: // Simple Titlecase Mapping
                split(tv,value,' ');
                codes = readCodes(tv);
                r.simpleTitlecaseMapping = codes;
                break;
            default:
                return 1;
        }
    }

    return 0;
}

int CUnicodeDatabase::uCharDecompose(vector<UINT32>& codes, UINT32 code) {
    CUnicodeCharacterData* r;
    r = unicodeCharacterDataFind(code);
    if (r && r->decompositionMapping.size() > 0) {
        for (int i = 0; i < r->decompositionMapping.size(); i++) {
            if (uCharDecompose(codes, r->decompositionMapping[i])) {
                return 1;
            }
        }
    }
    else {
        codes.push_back(code);
    }

    return 0;
}

int CUnicodeDatabase::unicodeDecompose(vector<UINT32>& output, const vector<UINT32>& input) {
    for(int i=0; i < input.size(); i++) {
        if (uCharDecompose(output, input[i])) {
            return 1;
        }
    }

    return 0;
}

int CUnicodeDatabase::load(const string& fileName) {

    CUnicodeCharacterData row;
    string s;
    ifstream in (fileName.c_str());

    if(!in) {
        return 1;
    }

    while(in && getline (in, s)) {
        if ( parse(row,s) == 0 ) {
            dataTable.push_back(row);
        }
        else {
            in.close();
            return 1;
        }
    }

    in.close();

    return 0;
}


int CUnicodeDatabase::utf8ToUnicode(vector<UINT32>& output, const string& input) {

    UINT8 c;
    UINT32 uc;
    const SUtf8Table *t;
    short bytes;

    for(int i=0; i < input.size(); i++) {

        c = (UINT8)input[i];
        uc = c;

        if (c & 0x80) {
            for (t = utf8Table; t->cmask && (c & t->cmask) != t->cval; t++);
            if(t->cmask) {
                for( c <<= 1; c & 0x80; c <<= 1) {
                    i++;
                    if(i >= input.size() || ((UINT8)input[i] & 0xC0) != 0x80) {
                        return 1;
                    }
                    uc = (uc << 6) | ((UINT8)input[i] & 0x3F);
                }
                uc &= t->lmask;
            }
        }
        output.push_back(uc);
    }
    return 0;
}

int CUnicodeDatabase::unicodeToUtf8(string &output,const  vector<UINT32>& input) {

    UINT32 uc;
    UINT8 mask;
    short bits;
    const SUtf8Table *t;

    for(int i=0; i < input.size(); i++) {

        uc = input[i];

        if ( uc <= 0x7F) {
            output += (UINT8)uc;
        }
        else {
            for (t = utf8Table; t->cmask && uc > t->lmask; t++);
            if(t->cmask) {
                bits = t->bits;
                output += (UINT8)((uc >> bits) | t->cval);
                while (bits > 0) {
                    bits -= 6;
                    output += (UINT8)(0x80 | ((uc >> bits) & 0x3F));
                }
            }
            else {
                return 1;
            }
        }

    }

    return 0;

}

int CUnicodeDatabase::utf8ToAscii(string& output, const string& input) {

    vector<UINT32> v1;
    vector<UINT32> v2;

    utf8ToUnicode(v1,input);
    unicodeDecompose(v2, v1);
    for(int i=0; i <v2.size(); i++) {
        if(v2[i] <= 0x7F) {
            output += (UINT8)v2[i];
        }
    }

    return 0;    
}
