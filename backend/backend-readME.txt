Backend Application:

Make sure npm and node is installed

run 'npm install' to download all the dependencies

run 'npm i -g nodemon' to download nodemon globally to run the server continously

run 'npm start' to start the application on port 8080

use (GET)'http://localhost:8080/api/v1/users/' to get all users and use the user ID for other endpoints

use (POST)'http://localhost:8080/api/v1/users/login' for logging in
body: {
    "email":"xyz@xyz.com",
    "password":"password1234"
}
use (POST)'http://localhost:8080/api/v1/users/signup' for signup
body: {
    "name":"xyz",
    "email":"xyz@xyz.com",
    "password":"password1234",
    "passwordConfirm":"password1234"
}

use (POST) 'http://localhost:8080/api/v1/rates/sendmail' for sending email 
body: {
    "user":"userID" 
}

use (POST) 'http://localhost:8080/api/v1/rates' for sending SPO2 and Heart Rate
body: {
    "spo2":"99",
    "heartRate":"60",
    "user":"userID"
}
user (GET) http://localhost:8080/api/v1/rates for getting SPO2 and heart Rate of a user
body: {
    user":"userID" 
}

use (POST)'http://localhost:8080/api/v1/rates/sendmail' for sending email to the user.
body: {
    "user":"userID"
}
 

