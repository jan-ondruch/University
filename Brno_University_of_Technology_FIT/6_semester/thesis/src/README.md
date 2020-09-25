# Sporterizer
Sporterizer is a web application for modern sports training, that utilizes data from a real-time locating system. The project was developed as a bachelor thesis.

## Installation
1. Install Node.js, npm, Angular CLI and MySQL.
2. Create a local MySQL database and populate it with the content of the enclosed `db.sql` file. Then change the database connection settings in the `src/server/config/connection.js` accordingly.
3. Change to the `src` folder and run `npm install` and then `ng build`.
4. Run `npm start` and visit localhost:3000 to start the application.
5. Login with the test acount - `email: lorem@ipsum.dolor`; `password: secret`. Or register and create a new account.