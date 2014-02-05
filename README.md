TT3
===

MQTT Paho C client for Windows

To run: Download the binary TT3.exe and the paho-mqtt3c.dll from the "release" directory.  you might also need the mfc100.dll from the same dir.

Documentation - not complete at the moment
-------------
### The main window
The main window is represented by the following picture:

![](https://raw2.github.com/francoisvdm/TT3/master/TT3Screenshot.JPG)

To restart the application with most of your supplied information filled in, remember to click the "save" button
after changes.  This will update the associated .ini file

### Replay Messages
To save all the messages in the "Subscriptions Received" listbox click on the "Save Msgs" button.  This will create a file TT3replay.mqtt with the following layout:

![](https://raw2.github.com/francoisvdm/TT3/master/TT3replay.JPG)

To replay the messages in the TT3replay.mqtt file, click the "Replay" button.  If you edit the file and changes the [seconds] to a value bigger than zero, it will wait that amount of seconds before it publishes the message.  Great for doing demos.  

If you edit the soure file by hand please ensure to press enter at the end of the last line.

Please note that due to some timing issues, do not subscribe to the same topics in the same running instance of TT3, start up a seperate TT3 instance to do the replay from.  Sorry.

To change the source file name and/or to replay in a loop, click on "Replay Setup" and then complete the popup window.
![](https://raw2.github.com/francoisvdm/TT3/master/TT3replaySettings.JPG)
### Performance Tests
To set up some performance tests, click on "Settings"

![](https://raw2.github.com/francoisvdm/TT3/master/Settings.JPG)

### Receive Alerts
To receive alerts when specific text strings appear in a message received, click on "Alert".

![](https://raw2.github.com/francoisvdm/TT3/master/Alert.jpg)

To add a new alert complete the string to search for and what beep frequency and length the alert must be.  Optionally add an application that must be called when this alert trigger.  This application will be called with parameters Topic,Message.  Your application must interpred the parameters.  Enable the "Active" checkbox and "Add" the alert.  The alert will be dsiplayed on the control listbox at the top of the screen.  When clicking the "Start" button all the "active" alerts in the listbox will be enabled and alert monitoring will start.

The "Save" button will add the alerts to the TT3.ini file so that next time TT3 starts it will populate the listbox with the entries from the ini file.

### Publish Messages on a regular basis
To publish messages on a regular basis, click on "Exerciser".

![](https://raw2.github.com/francoisvdm/TT3/master/Exerciser.JPG)
