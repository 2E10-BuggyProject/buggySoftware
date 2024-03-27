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



## Gold 
Design an autonomous buggy capable of navigating a more complex line-following course. Will have to navigate junctions and change behaviour based on tags placed on the course. 

Tags:
 - Train the autonomous buggy to recognize and interpret four different April tags. These tags should be readable by the HuskyLens vision sensor.
 - The tags will encode instructions to the buggies during the challenge.
 - The positioning of the tags on the course will be done by the challenge assessor.
 - The tags should be used to generate the behaviours below as the buggy drives towards and recognises them. 
 
Features:
 - Turn left at the next junction
 - Turn right at the next junction
 - Observe a Speed Limit (the buggy should adopt a slower speed by the time it reaches this marker)
 - Go as Fast as Possible (the buggy should be moving as fast as it can safely do so once it reaches this sign)
