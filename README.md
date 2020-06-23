# Quantizer

Quantizer is a desktop application that applies automatic editing to .wav files.
The app is built using the JUCE framework.

The user can:

- Drag and drop any .wav file within a dedicated area.
- Play/pause/stop the the currently loaded audio file.
- modify the output path and output file name. A standard path is generated whenever a new file is dragged and dropped.

In order to edit/time align the file, the user **must**:

- Set the Samplerate.
- Set the track BPM.
- Set the output bit depth.
- Set the time resolution (i.e. 16th, 32nd, ...)
- Specify if the audio file contains triplets

If all the above parameters are correctly set next steps are:

- choose the threshold level using the vertical slider Bar and draw the transients by clicking the **PEAK** button;
- once the transient are drawn within the thumbnail, click the **Quantize** button to save the nre edited .wav file in the selected path.

