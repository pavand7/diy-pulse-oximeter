const Rate = require('../models/userRate');
const Average = require('../models/averageModel')
const catchAsync = require('../utils/catchAsync');
const sendEmail = require('./../utils/email');
const User = require('../models/userModel');
const AppError = require('../utils/appError')

exports.mail = catchAsync(async(req,res,next) => {
    try{
        const user = await User.findOne({ _id: req.body.user});

        await sendEmail({
        email: user.email,
        subject: `Emergency!!!!! for ${user.name}`,
        message: `${user.name} just got a dangerously low reading for either oxygen level or heart rate please check on him/her as soon as possible`
    });
    res.status(200).json({
        status: 'success',
        message: 'Email Sent to the user!'});
    }catch(err){
        console.log(err);
    }

}
);

exports.createOne = catchAsync(async(req,res,next) => {
    

  
    const stats = await Rate.aggregate([
            {
                $group:
                {
                  _id: "$user",
                  heartRateAverage: { $avg: "$heartRate"},
                  spo2Average: {$avg:"$spo2"}
                }
            }
        ])
        const average = await Average.create({
            user: req.body.user,
            spo2Average: stats[0].spo2Average,
            heartRateAverage: stats[0].heartRateAverage
        });
    
        const newRate = await Rate.create(req.body);
    
    
    
    
    res.status(201).json({
        status: 'success',
        data: {
            userRate: newRate,
            averageRate: average
        }
    })
});

exports.getRates = catchAsync(async(req,res,next)=> {
    const rates = await Rate.findOne({_id:req.body.user});

    res.status(200).json({
        satus: 'success',
        token,
        data: {
            rates
        }
    });
})