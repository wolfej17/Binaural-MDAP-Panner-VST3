/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   azi0el90_wav;
    const int            azi0el90_wavSize = 1068;

    extern const char*   azi0el270_wav;
    const int            azi0el270_wavSize = 3116;

    extern const char*   azi45el0_wav;
    const int            azi45el0_wavSize = 3116;

    extern const char*   azi135el0_wav;
    const int            azi135el0_wavSize = 3116;

    extern const char*   azi225el0_wav;
    const int            azi225el0_wavSize = 3116;

    extern const char*   azi315el0_wav;
    const int            azi315el0_wavSize = 3116;

    extern const char*   FronHead_png;
    const int            FronHead_pngSize = 29626;

    extern const char*   SideHead_png;
    const int            SideHead_pngSize = 21561;

    extern const char*   TopHead_png;
    const int            TopHead_pngSize = 14319;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 9;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
