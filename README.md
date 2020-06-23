# Quantizer

Quantizer is a desktop application that applies automatic editing to .wav files.
The app is built using the [JUCE](https://www.juce.com) framework, starting from the audio playback demo.

The user can:

- Drag and drop any .wav file within a specific thumnail area
- Play/pause/stop the the currently loaded audio file
- modify the output path and output file name. A standard path is generated whenever a new file is dragged and dropped
- zoom in/out the wave form using the horizontal slider or the scrollbar

Before applying the time aligning algorithm the user **must** firs set the following parameters:

- Samplerate (must be an integer)
- BPM (must be an integer)
- Output bit depth (16, 24 bit)
- Time resolution (i.e. 16th, 32nd, ...)
- specify if the audio file contains triplets

If all the above parameters are correctly set, the user needs to:

- choose the threshold level using the vertical slider Bar
- click the **Peak** button to draw the transient markers;
- click the **Quantize** button to save the new edited .wav file at the selected path.

