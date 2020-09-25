# Feature Name
Author: Axel Butz

## Introduction
Einsatz von haproxy (http://www.haproxy.org/) um dem Umfragesystem
Hochverfügbarkeit und Lastverteilung Möglichkeiten hinzu zufügen.

## Motivation
Sicherstellung des Verfügbarkeit des Systems, sowie dessen Skalierbarkeit
und Möglichkeit der Wartung der Systeme ohne Ausfallzeiten.

## Open Questions
-In welchen Budget Rahmen soll das System verfügbar gehalten werden?

## Proposed solution
1. Günstige Lösung

    ![Single Loadbalancer]
    (https://assets.digitalocean.com/articles/HAProxy/layer_4_load_balancing.png)

    Umleitung des Traffic auf verfügbare Services basiernd auf auf IP

    Nachteil: Der Loadbalancer/haproxy in diesem Setup ist der SinglePoint of Failure.
    Somit ist DDOS ist immer noch möglich, sowie Überlastung (Dies ist aber erst ab einer
    gleichzeitigen Useranzahl von mehr als 10.000 Usern zu erwarten)




2. Teuere, aber sehr skalierbare Lösung

    ![Floating IP, multi Loadbalancer]
    (https://assets.digitalocean.com/articles/high_availability/ha-diagram-animated.gif)

    Nutzung von automatischen Failover, um einen ausgefallen haproxy durch einen Ersatzproxy zu ersetzen

