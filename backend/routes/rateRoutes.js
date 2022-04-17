const express = require('express');
const rateController = require('../controllers/rateController');


const router = express.Router();

router
    .route('/')
    .get(rateController.getRates)
    .post(rateController.createOne);

router
    .route('/sendmail')
    .post(rateController.mail)


module.exports = router;