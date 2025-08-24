NOTE TO THE COMMENTS IN THE CODE:
The comments are in Hungarian, as this was for a university project.

EXPLANATION FOR THE PROJECT:
This was a school project we had to do when we were learning about embedded C. We were given a replica of a traffic intersection and we needed to design an algorithm that was going through a sequence just like in real life. There were variables like night mode, where when I push a button on the board every lamp had to blink with PWM in yellow. Or there was a rail crossing right in front of one of the roads in the intersection, so we had to take into consideration that too. For example the rail was coming, so i could not allow the opposite lamp to turn green, as those who were going straight were blocked by the railway crossing, so i could only allow the turning lanes to go (there was a separate LED for those). We were required to use interrupts, as we did with all of our other demo codes during the semester.
