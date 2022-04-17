const mongoose = require('mongoose');

const averageSchema = new mongoose.Schema({
    spo2Average: {
        type: Number,
    },
    heartRateAverage: {
        type:Number,

    },
    user: {
        type: mongoose.Schema.ObjectId,
        ref: 'User',
        required: [true, 'Heart Rate must belong to a user']
    }
});

const AverageRate = mongoose.model('averageRate', averageSchema);


module.exports = AverageRate;