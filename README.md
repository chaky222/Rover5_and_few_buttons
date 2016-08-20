# Rover5_and_few_buttons
This project was done for my client when I worked on odesk.com.

In this project connected:
 1) 4 motors connected to Rower5 board or 4 simple H-bridges;
 2) 4 optical encoders (such as KY-040 or some others) for read distance and speed;
 3) 2 variable resistors (1st - speed, 2nd - distance);
 4) few buttons (stop, resume, go forward, go back, save position to EEPROM to get this position on power on in next time);
 5) arduino 2560 with formware from this repository with my own system for speed regulator for this motors.


how to connect travel/max_speed variable resistors:
![Alt text](/img/1.jpg "how to connect travel/max_speed variable resistors")<br><br>

And you can use this debug app for see steps readed from encoders, current PWM to each motors, values from travel/max_speed variable resistors and control it from gui:
![Alt text](/img/2.jpg "debug app")<br><br>