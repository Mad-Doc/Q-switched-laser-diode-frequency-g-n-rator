simple code for generating complexe pwm signal for controlling Qcw-laser diode
use arduino nano 328p and ssd1315 Oled + rotary encoder 

the code generate pulse a 0 to 20khz frequency at 20% ON of duty cycle auto calculated and linear response !
and 10 to 300ns pulse max
control this to encoder or serial command , originaly for jdsu Qcw-250watt jenoptik laser fibre diode control + laser diode drivers adapted of course !

possible other application for complex and stable pwm signal 

for good stability of signal , please use  maximum 10nf ceramic or tantal capaçitor on // to output and gnd , and possible small self in serie for filtering upper 20khz harmonic signal ! 
and use really good 5V psu ! not switching , toroid transfo and good regulation 5V , but only battery is the best ! 
