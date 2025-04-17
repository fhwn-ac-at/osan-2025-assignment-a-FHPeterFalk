[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/H1vNwaly)
# Hello, Kevin!

In dieser Praxiseinheit haben wir uns mit:
1. dem Einlesen von Benutzereingaben über Commandline-arguments mithilfe von getopt
2. dem erstellen neuer Programme und den Umgang mit diesen innerhalb unseres eigenen C-Programmes
3. der Kommunikation zwischen Programmen mithilfe von POSIX-message passing über message queues
beschäftigt.

GETOPT
getopt vereinfacht es Benutzereingaben einzulesen.
Der getopt-Funktion wird ein selbst zusammengebastelter-string übergeben, über welchen man einstellen kann was für Argumente man annimmt.
Bei falschen Argumenten liefert getopt '?' zurück.
Man kann sowohl Argumente mit Werten, als auch welche ohne, abfragen.

ERSTELLEN VON PROGRAMMEN
fork() spaltet den laufenden Prozess in zwei.
Jeder Prozess hat eine Prozess-ID. Es ist möglich seine eigene abzufragen und diese auch zu speichern.
mit pid_t forked = fork() haben wir zwischen dem abgespaltetem und dem ursprünglichem Prozess unterschieden, da der abgespaltete noch keinen Wert in forked gespeichert hat.
mit wait() kann man einen Prozess auf seine Kinder warten lassen, auch kann man innerhalb von wait den Status, mit dem sich der Kinderprozess beendet hat, abfangen.

MESSAGE-queues
Nachrichten werden zwischen Prozessen über message queues ausgetauscht.
Diese haben alle einen bestimmten Namen, der innerhalb des Systems einzigartig ist.
Es ist möglich festzulegen, wie viele Nachrichten in der Schlange stehen können. Es ist auch möglich Nachrichten eine Priorität zu geben.
Es gibt Funktionen um Nachrichten zu senden und zu empfangen.
Nachrichten haben immer eine feste Größe, welche man selber festlegen muss.
Mann kann auf mehreren arten die message queue öffnen: Nur Lesen, Nur Schreiben, Lesen und Schreiben, etc.
Wenn man eine message queue öffnet, dann muss man sie auch wieder schließen (wie bei Malloc und free).