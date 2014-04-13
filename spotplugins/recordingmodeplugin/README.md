# About Recording Mode

Recording Mode provides Spot with the ability to record a constant stream of videos on any surveillance channel. If you require 24/7 surveillance with video backup, then Recording Mode is what you need.

When Recording Mode is activated on a channel, Spot will begin recording a constant stream of videos using that channel's video and audio feeds. Recorded videos can be stored locally (on your local computer), or synchronized with your online storage. 

## Configuring Recording Mode

To configure the default settings for *all* channels, go to the **Recording Mode** tab of the **Client Configuration** menu.
Alternatively, to configure each channel's settings individually, you can use the **Mode Configuration** menu to override default settings. For more help configuring modes and locating the different menus please see [this help entry](http://anionu.com/help/dashboard#configuring-modes).

Below is a description of each configuration option available for Recording Mode:

### Synchronize Videos

This setting determines weather or not recorded videos will be synchronized with your online storage.
Synchronized videos are uploaded to the Anionu server, and can be accessed from your dashboard, while non-synchronized can only be accessed from the local computer they were recorded on. Uploading a constant stream of video footage requires a lot of bandwidth and storage capacity, so appropriate care should be taken when considering this option.

### Video Segment Duration

This option determines the length of each recorded video. If you are planning to synchronize recorded videos then it is recommended that you keep the video duration to under 10 minutes in order to avoid any upload issues. Alunder ternatively, if you are storing videos locally, then you can specify any duration up to 1 hour. It is recommended that you have at least 1 TB of available HDD space for uninterrupted surveillance.

## Video Formats

Due to the licensing restrictions we can only provide you with a couple of basic video formats by default.
To add support for more video formats you can install the Media Plugin, which adds support for many other widely used proprietary formats like H.264 and XviD.
If you are, or have access to a developer, you can also compile the Media Plugin yourself in order to enable recording in any video format supported by FFmpeg.
