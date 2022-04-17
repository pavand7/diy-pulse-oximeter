const express = require('express');
const morgan = require('morgan');

const userRouter = require('./routes/userRoutes');
const rateRouter = require('./routes/rateRoutes')

const app = express();

//1. Global Middlewares
if(process.env.NODE_ENV === 'development') {
    app.use(morgan('dev'));
}
app.use(express.json());
app.use((req,res,next)=> {
    console.log(req.headers);
    next();
})


//Routes
app.use('/api/v1/users', userRouter);
app.use('/api/v1/rates', rateRouter);



app.get('/', (req,res)=> {
    res.status(404).json({message: 'Hello from the server side!', app: 'IOT'});
})
app.all('*', (req,res,next)=> {
    next(new AppError(`Can't find ${req.originalUrl} on this server!`, 404));
});


module.exports = app;