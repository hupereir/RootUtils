#ifndef Streamer_h
#define Streamer_h

// $Id: Streamer.h,v 1.47 2010/09/15 02:27:25 hpereira Exp $

#include <TROOT.h>
#include <TCut.h>
#include <TH1.h>
#include <TObject.h>

#include <iostream>
#include <string>

class Stream:public TObject
{

    public:

    //* constructor (doing nothing, needed for root)
    Stream():TObject()
    {}

    //* string replacement
    static std::string ReplaceAll( const std::string& in, const std::string& c0, const std::string& c1 );

    //* print vector
    static void PrintVector(
        TString name,
        const Double_t* values,
        Int_t size,
        TString format = "%f" )
    { PrintVector( std::cout, name, values, size, format ); }

    //* print vector
    static void PrintVector(
        std::ostream& out,
        TString name,
        const Double_t* values,
        Int_t size,
        TString format = "%f" );

    //* print vector
    static void PrintIntVector(
        TString name,
        const Int_t* values,
        Int_t size,
        TString format = "%i" )
    { PrintIntVector( std::cout, name, values, size, format ); }

    //* print vector
    static void PrintIntVector(
        std::ostream& out,
        TString name,
        const Int_t* values,
        Int_t size,
        TString format = "%i" );

    //* print vector
    static void PrintVector2D(
        std::ostream& out,
        TString name,
        const Double_t* values,
        Int_t size_1,
        Int_t size_2,
        TString format = "%f" );

    //* print vector
    static void PrintVector2D(
        TString name,
        const Double_t* values,
        Int_t size_1,
        Int_t size_2,
        TString format = "%f" )
    { PrintVector2D( std::cout, name, values, size_1, size_2, format ); }

    //* print vector
    static void PrintIntVector2D(
        TString name,
        const Int_t* values,
        Int_t size_1,
        Int_t size_2,
        TString format = "%i" )
    { PrintIntVector2D( std::cout, name, values, size_1, size_2, format ); }

    //* print vector
    static void PrintIntVector2D(
        std::ostream& out,
        TString name,
        const Int_t* values,
        Int_t size_1,
        Int_t size_2,
        TString format = "%i" );

    //* print vector
    static void PrintVector3D(
        std::ostream& out,
        TString name,
        const Double_t* values,
        Int_t size_1,
        Int_t size_2,
        Int_t size_3,
        TString format = "%f" );

    //* print vector
    static void PrintVector3D(
        TString name,
        const Double_t* values,
        Int_t size_1,
        Int_t size_2,
        Int_t size_3,
        TString format = "%f" )
    { PrintVector3D( std::cout, name, values, size_1, size_2, size_3, format ); }

    //* print vector
    static void PrintVector4D(
        TString name,
        const Double_t* values,
        Int_t size_1,
        Int_t size_2,
        Int_t size_3,
        Int_t size_4,
        TString format = "%f" )
    { PrintVector4D( std::cout, name, values, size_1, size_2, size_3, size_4, format ); }

    //* print vector
    static void PrintVector4D(
        std::ostream& out,
        TString name,
        const Double_t* values,
        Int_t size_1,
        Int_t size_2,
        Int_t size_3,
        Int_t size_4,
        TString format = "%f" );

    //* print cuts, formated
    static void PrintCuts( const TCut& cut );

//     //* print histogram content
//     static void PrintContent( TH1* );

    //* print histogram axis
    static void PrintAxis( TH1* );

    protected:

    #ifndef __CINT__

    //* print vector
    template<typename T>
        static void PrintVector(
        std::ostream& out,
        TString type,
        TString name,
        const T* values,
        Int_t size,
        TString format = "%f" )
    {
        out << type << " " << name << "["<<size<< "] = {";
        char value[20];
        for( Int_t i=0; i<size; ++i )
        {
            sprintf( value, format.Data(), values[i] );
            out << value;
            if( i != size-1 ) out << ", ";
            else out << "};";
        }

        out << std::endl;
    }

    //* print vector
    template<typename T>
        static void PrintVector2D(
        std::ostream& out,
        TString type,
        TString name,
        const T* values,
        Int_t size_1,
        Int_t size_2,
        TString format = "%f" )
    {
        out << type << " " << name << "[" << size_1 << "][" << size_2 << "] = {";
        if( size_1 > 1 ) out << std::endl;
        char value[20];
        for( Int_t i=0; i<size_1; ++i )
        {
            out << "  {";
            for( Int_t j=0; j<size_2; j++ )
            {
                sprintf( value, format.Data(), (Double_t) values[i*size_2+j] );
                out << value;
                if( j != size_2-1 ) out << ", ";
                else out << "}";
            }
            if( i != size_1-1 ) out << "," << std::endl;
            else out << " };" << std::endl;
        }

        return;
    }


    //* print vector 3D
    template<typename T> static void PrintVector3D(
        std::ostream& out,
        TString type,
        TString name,
        const T* values,
        Int_t size_1,
        Int_t size_2,
        Int_t size_3,
        TString format = "%f" )
    {
        out << type << " " << name << "[" << size_1 << "][" << size_2 << "][" << size_3 << "] = {" << std::endl;
        char value[20];
        for( Int_t i=0; i<size_1; ++i )
        {
            out << "  {";
            for( Int_t j=0; j<size_2; j++ )
            {
                out << "  {";
                for( Int_t k=0; k<size_3; k++ )
                {
                    sprintf( value, format.Data(), (Double_t) values[(i*size_2+j)*size_3+k] );
                    out << value;
                    if( k != size_3-1 ) out << ", ";
                    else out << "}";
                }
                if( j != size_2-1 ) out << ", ";
                else out << "}";
            }
            if( i != size_1-1 ) out << "," << std::endl;
            else out << " };" << std::endl;
        }

        return;
    }


    //* print vector 4D
    template<typename T> static void PrintVector4D(
        std::ostream& out,
        TString type,
        TString name,
        const T* values,
        Int_t size_1,
        Int_t size_2,
        Int_t size_3,
        Int_t size_4,
        TString format = "%f" )
    {
        out << type << " " << name << "[" << size_1 << "][" << size_2 << "][" << size_3 << "][" << size_4 << "] = {" << std::endl;
        char value[20];
        for( Int_t i=0; i<size_1; ++i )
        {
            out << "  {" << std::endl;
            for( Int_t j=0; j<size_2; j++ )
            {
                out << "    { ";
                for( Int_t k=0; k<size_3; k++ )
                {
                    out << "{";
                    for( Int_t l=0; l<size_4; l++ )
                    {
                        sprintf( value, format.Data(), (Double_t) values[((i*size_2+j)*size_3+k)*size_4+l] );
                        out << value;
                        if( l != size_4-1 ) out << ", ";
                        else out << " }";
                    }

                    if( k != size_3-1 ) out << ", ";
                    else out << "    }";
                }
                if( j != size_2-1 ) out << ", " << std::endl;
                else out << std::endl << "  }";
            }
            if( i != size_1-1 ) out << "," << std::endl;
            else out << std::endl << "};" << std::endl;
        }

        return;
    }


    #endif

    ClassDef(Stream,0)

};

#endif
