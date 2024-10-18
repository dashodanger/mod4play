# mod4play
This is a unified interface for the ft2play (https://github.com/8bitbubsy/ft2play) and it2play (https://github.com/8bitbubsy/it2play) libraries by Olav Sørensen for embedding in game engines or other applications.
SDL and WinMM dependencies have been removed;
the library is intended to receive an in-memory IT, S3M, MOD, or XM format module and produce samples on demand. All mixing drivers other than SB16 have been removed for simplicity.

# Compilation
A simple `CMakeLists.txt` file has been included showing which files need to be compiled. Include `m4p.h` in the source file that you are using to handle IT/S3M/MOD/XM playback.

To compile a test program, please use the regular CMakeLists file in this directory. It will build a small program that displays a window and replays an embedded MOD file.
  - The test program uses the Sokol libraries which are zlib-licensed and is based on the mod player example from sokol-samples, which is MIT licensed. Neither of these licenses affect mod4play when compiled on its own.

# Usage
- (Optional step, this will also be done internally when attempting to load the module) Call `m4p_TestFromData` with a pointer to a memory buffer containing the tracker module and its length as parameters to test if the module is a format that is compatible with mod4play. A result of zero indicates that it is an unknown format and should not be used.
- Load a supported module with the `m4p_LoadFromData` function, passing a pointer to a memory buffer containing the module, its length, the desired frequency/sample rate, and the desired mixing buffer size as parameters. The mixing buffer size should correspond to the size of the buffer you are planning on using as output for generated samples. This function will return `false` if a replayer was not successfully initialized.
  - Once successfully loaded, the memory buffer that you passed to this function can be safely freed if you are not otherwise using it.
- Prepare the module for playback with the `m4p_PlaySong` function. This does not require any parameters and will not generate any audio yet.
- In an appropriate place in your program, call the `m4p_GenerateSamples` function, passing a pointer to an initialized buffer to store the generated samples and the number of desired samples to generate as parameters. The number of samples to generate should be no more than the size of the buffer divided by `sizeof(int16_t)`. Generated samples will be in the form of pairs of signed 16-bit integers.
  - Alternatively, `m4p_GenerateFloatSamples` can be used in a conjunction with a number of samples representing the buffer size divided by `sizeof(float)` to produce floating-point output.
- When finished with playback, call the `m4p_Close` function to reset the internal replayer, and the `m4p_FreeSong` function to free the internally allocated buffer containing the module. Neither function requires any parameters.
