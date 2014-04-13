# About Surveillance Mode

Surveillance Mode provides Spot with real-time motion detection capabilities. Surveillance mode is ideal for when you are away from the surveilled premises, and want to protect yourself against unwanted intruders. 

## How It Works

When Surveillance Mode is activated on a channel, Spot will begin detecting motion on that channel's video camera.
When motion is detected the process is as follows:

*   A **Motion Detected** event will be created, which will raise the alarm and send notifications to you or a designated third party. Notifications can be sent via SMS, email or Twitter. See the **Event Notifiers** page on your dashboard to setup notifications. 
*   A video will be recorded which captures the interval of motion. Videos can be optionally synchronized with your online storage. Video synchronization is recommended for Surveillance Mode, as it enables you to view recorded motion videos from your dashboard when the alarm is triggered.

## Avoiding False Alarms

False alarms can be costly and annoying, so it is important to consider your surveillance environment and position of your cameras accordingly. The motion detector is designed adapt to gradual changes in luminosity, but sudden changes must be avoided as they will trigger the alarm:

*   **Sudden light changes**: Car headlights, clouds, light switches
*   **Background motion**: Waving trees, curtains
*   **Background geometry**: Parking cars, moved items

## Configuring Surveillance Mode

To configure the default settings for *all* channels, go to the **Recording Mode** tab of the **Client Configuration** menu.
Alternatively, to configure each channel's settings individually, you can use the **Mode Configuration** menu to override default settings. For more help configuring modes and locating the different menus please see [this help entry](http://anionu.com/help/dashboard#configuring-modes).

Open the **Mode Configuration** page to configure Surveillance Mode for a channel. At the top of the page is a real-time motion video which enables you to visualize motion exactly as Spot sees it. You can use the video as a guide while you fine tune the motion detector settings below.
In the top left hand corner of the video are three values: Motion Threshold, Motion Level and Motion State. When you move in front of the camera you will notice the Motion Level rise. If the Motion Level exceeds the Motion Threshold, the alarm will trigger. 

The Motion State informs you of the current state of the motion detector:

*   **Waiting**: The motion detector is waiting to become Vigilant.
*   **Vigilant**: The motion detector is actively detecting motion.
*   **Triggered**: Motion has been detected, and the alarm is triggered.

Below is a description of each configuration option available for Surveillance Mode:

### Motion Threshold

This setting directly affects the sensitivity of the motion detector. 
A lower value means greater sensitivity; if the Motion Level exceeds this value, then motion is detected.

### Pre Surveillance Delay

This is the delay time (in seconds) before motion detection will actually commence.
Set this to the amount of time you need to vacate the surveilled area after activating Surveillance Mode.

### Minimum Triggered Duration

This is minimum duration of time that the motion detector can remain in the **Triggered** state.
This is also the minimum duration of any recorded videos

### Maximum Triggered Duration

This is the maximum amount of time that the motion detector can remain in the **Triggered** state. 
This is also the maximum duration of any recorded videos.

### Stable Motion Frames

In order to avoid false alarms, the motion detector must detect a certain number of motion frames inside a time interval before the alarm will trigger. 
This setting determines how many motion frames to detect before motion is considered stable.

### Stable Motion Lifetime

This is the duration of time (in seconds) that motion frames remain valid.

### Synchronize Videos

This setting determines weather or not recorded videos will be synchronized with your online storage.