const mongoose = require('mongoose');

const userRateSchema = new mongoose.Schema({
    heartRate: {
        type: Number,
        required: [true, 'There must be some reading']
    },
   
    spo2: {
        type: Number,
        required: [true, 'There must be some reading'] 
    },
    
    time: {
        type: Date,
        default: Date.now
    },
    user: {
        type: mongoose.Schema.ObjectId,
        ref: 'User',
        required: [true, 'Heart Rate must belong to a user']
    }
}
    ,
    {
        toJSON: {virtuals:true},
        toObject: {virtuals:true}
    }
);





const UserRate = mongoose.model('userRate', userRateSchema);

module.exports = UserRate;
