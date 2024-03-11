# 2E10 Self-driving Buggy Project

This project entailed creating a self-driving buggy at different levels of capability; namely Bronze, Silver and Gold.

Members:
Ruairi Mullally,
Shane McDermott,
Labiba Mansur,
Noah Savage,

## Bronze

Overall goal of bronze: traverse a 3m track, stop for obstacles, report events wirelessly.

**Details:**

- The track will consist of a line forming a loop with a total track length (perimeter) of at least 3m. 
  - The track itself is a line on a background of contrasting colour (such that it the line can be detected by the IR sensors).
  - The track will form a loop (see image below; the track may not look precisely like this but it will approximate the shape, and in particular you will include at least two right-angle turns). The total length will be at least 3m.
    ![image](https://github.com/2E10-BuggyProject/buggySoftware/assets/123960178/00d192c2-0648-464b-9a52-a188f1b2bf6d)

- The PC control (GUI) program must:
  - Provide the user with start and stop buttons that can be used to begin and end the buggy's run on the track.
  - Provide an output area that displays telemetry received from the buggy during the run. 
- The buggy must:
  - Start the run on receiving a GO command via WiFi  from the controlling PC.
  - Stop the run on receiving a STOP command via WiFi from the controlling PC.
  - Traverse the track twice without derailing, using the IR sensors to follow the line of the track
  - Pause for obstacles as detected by the US rangefinder.
  - Report to the controlling PC when obstacles are detected and cleared.
  - Report distance travelled from wheel encoders.
 


 ## Silver

 In addition to the Bronze challenge requirements , in the silver challenge:

- It's speed is to be controlled through a PID controller.
- Two modes of speed control are to be implemented:
  1. via entering the reference speed into the GUI (should be possible to update at any time during the demo) and
  2. via reference object placed in sight of the ultrasonic distance sensor.
- The object is moving forward at varying speeds.
- The telemetry reported back to the GUI should contain the distance of the object, the current mode of control, the current reference speed, and the current speed of the buggy.
