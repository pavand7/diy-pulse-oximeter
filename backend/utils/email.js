const nodemailer = require('nodemailer');

const sendEmail = async options => {
    //1) Create a transporter
    const transporter = nodemailer.createTransport({
        host: "smtp.mailtrap.io",
        port: 2525,
        auth: {
            user: "051185227f2310",
            pass: "25ca634d1fdb90"
        }
    })

    //2) Define the email options
    const mailOptions = {
        from: 'Praduyot saikia <prayotrockstar@gmail.com>',
        to: options.email,
        subject: options.subject,
        text: options.message,

    }

    //3) Actually send the email
    await transporter.sendMail(mailOptions)
};

module.exports = sendEmail;