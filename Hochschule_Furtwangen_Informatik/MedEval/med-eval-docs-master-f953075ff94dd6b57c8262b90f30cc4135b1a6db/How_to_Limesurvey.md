# How to set up Limesurvey on Ubuntu

1. Eine VM mit Ubuntu aufsetzen (dieses Beispiel wurde mit Ubuntu 14.04 durchgeführt)
2. Updaten und Upgraden von Ubuntu `sudo apt-get update && sudo apt-get -y upgrade`
3. MariaDB installieren `sudo apt-get install -y mariadb-server`
4. Datenbank anlegen
  - `mysql -uroot -p`
  - `MariaDB [(none)]> CREATE DATABASE limesurvey;`
  - `MariaDB [(none)]> GRANT ALL PRIVILEGES ON limesurvey.* TO 'limeuser'@'localhost' IDENTIFIED BY 'your-password';`
  - `MariaDB [(none)]> FLUSH PRIVILEGES;`
  - `MariaDB [(none)]> \q`
5. Apache Installieren`
  - sudo apt-get install apache2`
6. PHP installieren
  - `sudo apt-get install php7.0 libapache2-mod-php7.0 php7.0-mcrypt php7.0-mysql` (Versionen können veraltet sein)
7. Limesurvey herunterldaen und aufsetzen.
  - `wget https://www.limesurvey.org/stable-release?download=1884:limesurvey2543%20161014zip -O limesurvey.zip`
  - `sudo unzip limesurvey.zip`
  - `sudo mv limesurvey/ /var/www/html/limesurvey/`
  - `sudo chown www-data:www-data -R /var/www/html/limesurvey`
8. Limesurvey config file erstellen
  - `sudo touch /etc/apache2/sites-available/limesurvey.conf`
  - `sudo ln -s /etc/apache2/sites-available/limesurvey.conf /etc/apache2/sites-enabled/limesurvey.conf`
  - `sudo nano /etc/apache2/sites-available/limesurvey.conf`
9. Limesurvey config file schreiben.

```
<VirtualHost *:80>
ServerAdmin admin@yourdomain.com
DocumentRoot /var/www/html/limesurvey/
ServerName your-domain.com
ServerAlias www.your-domain.com
<Directory /var/www/html/limesurvey/>
Options FollowSymLinks
AllowOverride All
</Directory>
ErrorLog /var/log/apache2/your-domain.com-error_log
CustomLog /var/log/apache2/your-domain.com-access_log common
</VirtualHost>`
```

Da alles lokal bearbeitet wird, sollte man "your-domain.com" durch 127.0.0.1 ersetzen.
also so:

```
<VirtualHost *:80>
ServerAdmin istauch@egal.com
DocumentRoot /var/www/html/limesurvey/
ServerName 127.0.0.1
ServerAlias 127.0.0.1
<Directory /var/www/html/limesurvey/>
Options FollowSymLinks
AllowOverride All
</Directory>
ErrorLog /var/log/apache2/127.0.0.1:80-error_log
CustomLog /var/log/apache2/127.0.0.1:80-access_log common
</VirtualHost>
```

10. Im Browser [127.0.0.1:80/admin](127.0.0.1:80/admin) aufrufen und mit der
    Limesurvey einrichtung fortfahren. Dies gescheiht durch ein Webinterface
    und führt durch verschiedenen Schritte. Sollte dies nicht funktionieren.
    Den Apache mit `sudo service apache2 restart`neu starten
  - Festlegen der Sprache in der das Interface später angezeigt werden soll
  - Lizenz... akzeptieren und weiter
  - Weboberfläche prüft ob auch alles Nötige (PHP, DB etc.) installiert ist.
    Normalerweise sollte alles dabei sein was zum starten von Limesurvey
    benötigt wird.
  - Datenbank Einstellungen. Username etc. eingeben.
![Limesurvey DB config](http://linuxpitstop.com/wp-content/uploads/2015/08/44.png)
  - Optionales Einstellungen & festlegen des Admin Accounts.
![Limesurvey opt config](http://linuxpitstop.com/wp-content/uploads/2015/08/54.png)

Fertig! Jetzt wirst du zum Loginscreen weitergeleitet.
Dort kannst du dich über den Admin account einloggen und die
Limesurvey Dienste ausprobieren.

Ab jetzt wird [127.0.0.1:80/admin](127.0.0.1:80/admin) dich direkt zum
Login Screen weiterleiten.

![Limesurvey Login](http://linuxpitstop.com/wp-content/uploads/2015/08/85.png)
