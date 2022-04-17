const User = require('./../models/userModel');
const catchAsync = require('./../utils/catchAsync');
const AppError = require('./../utils/appError');
const jwt = require('jsonwebtoken');
const {promisify} = require('util');

const signToken = id => {
    return jwt.sign({id}, process.env.JWT_SECRET, {
        expiresIn: process.env.JWT_EXPIRES_IN
    });
}
const createSendToken = (user, statusCode, res) => {
    const token = signToken(user._id);
    const cookieOptions = {
        expires: new Date(Date.now() + process.env.JWT_COOKIE_EXPIRES_IN * 24 * 60 * 60 *1000),
        httpOnly: true
    };
    if(process.env.NODE_ENV==='production') cookieOptions.secure = true;

    res.cookie('jwt', token, cookieOptions);
    user.password = undefined;
    res.status(statusCode).json({
        satus: 'success',
        token,
        data: {
            user
        }
    });
};

exports.signup = catchAsync(async(req,res,next) => {
    const newUser = await User.create({
        name: req.body.name,
        email: req.body.email,
        password: req.body.password,
        passwordConfirm: req.body.passwordConfirm,
        role: req.body.role
    });

    createSendToken(newUser, 201, res);
    
});

exports.getUsers = catchAsync(async (req,res,next)=>{
    

})

exports.deleteUser = catchAsync(async (req,res,next)=>{
    

})

exports.login = catchAsync(async (req,res,next) => {
    const {email, password} = req.body;

    // 1) Check if email and password exist
    if(!email || !password){
        return next(new AppError('Please provide email and password!', 400));
    }
    // 2) Check if the user exists && password is correct
    const user = await User.findOne({email}).select('+password');


    if(!user || !(await user.correctPassword(password, user.password))) {
        return next(new AppError('Incorrect Email or Password', 401));
    }
    createSendToken(user, 200, res);
    
});

exports.restrictTo = (...roles) => {
    
    return (req,res,next) => {
        // roles ['admin', 'lead-guide']. role= 'user
        console.log(req.user.role);
        if(!roles.includes(req.user.role)){
            return next(new AppError('You do not have permission to perform this action', 403));
        }

        next();
    }
} 
exports.protect = catchAsync(async(req,res,next)=> {
    // 1) Getting token and cheking if it exists
    let token;
    if (req.headers.authorization && req.headers.authorization.startsWith('Bearer')) {
        token = req.headers.authorization.split(' ')[1];
    }
    if(!token) {
        return next(new AppError('You are not logged in! Please log in to get access.', 401));
    }

    // 2) Verification token
    const decoded = await promisify(jwt.verify)(token, process.env.JWT_SECRET);

    // 3) Check if User still exists
    const freshUser = await User.findById(decoded.id);
    if(!freshUser) {
        return next(new AppError('The User belonging to this token does no longer exist', 401));
    }

    // 4) Check if user changed password after the token was issued
    if(freshUser.changePasswordAfter(decoded.iat)){
        return next( new AppError('User recently changed password! Please log in again', 401));
    }

    //Grant Access to protected Route
    req.user = freshUser;
    next();
});

exports.getAllUsers = catchAsync(async(req,res,next) => {
    const users = await User.find()

    createSendToken(users, 200, res);
    
});
