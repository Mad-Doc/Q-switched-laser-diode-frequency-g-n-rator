#QCW Laser diode Frequency Generator

un code assez simple pour pouvoir généré un signal PWM complexe pour controller efficacement un laser-diode QCW 

utilise 
-un arduino nano 328P (pas le 168 ! )
-un ecran Oled ssd1315
-un encodeur rotatif 

ce code genere un pulse d'une frequence de 0 a 20khz fluide.
genere un duty cycle de 0 a 20% max
genere une largeur de pulse de 10ns a 300ns max 
et cela auto adaptatif selon la fréquence ou la largeur du pulse

il se controle via un encodeur rotatif ou en commande serie.
originalement codé pour une diode JDSU QCW-250Watt de chez jenoptik (diode laser QCW fibrée) et son drivers qcw adapter 

d'autre application possible, pour qui a besoin d'un signal pwm "compliquer" mais stable !

pour une bonne stabilité du signal il faut branché un condensateur au tantal en // a la sortie "pwm" et la masse et une petite self de quelque µH en serie entre le µC et le bnc/usage sinon 
le signal dans les 20khz sera moins propre, un blindage de la partie sortie sera bénéfique aussi !
il faut utiliser une bonne alimentation 5V tres propre, evité les psu a découpage "bruyante" ! 
le top etant des batterie ou un transformateur torique et une bonne régulation 5V :)
ou prévoir un tres bon filtrage de l'alimentation !  

