# Quantizer

Quantizer is a desktop application which perforrms automatic audio editing.
It only works with .wav files format.
The app is built with the [JUCE](https://www.juce.com) framework, starting from the audio playback demo template.

The user can:

- Drag and drop any .wav file on a specific thumnail area
- Play/pause/stop the currently loaded audio file
- modify the output path and output file name, even if a standard path is generated whenever a new file is dragged and dropped
- zoom in/out the wave form using a horizontal slider or the scrollbar

Before applying the time alignment algorithm the user **has to** edit the following parameters:

- Samplerate (an integer value is expected)
- BPM (an integer value is expected)
- Output bit depth (16 or 24 bit)
- Time resolution (the shortest time span of any note of the loaded audio file. For instance, 1/4th, 1/8th, 1/16th, 1/32nd ... of single bar)
- a toggle button to specify if the audio file contains triplets

Once all the above parameters are correctly set, the user needs to:

- draw the peaks;
- click the **Quantize** button to save the new edited .wav file at the selected path.

